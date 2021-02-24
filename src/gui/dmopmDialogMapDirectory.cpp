/* 
 * DMO Pack Manager
 *
 * Copyright (C) 2011, Plüss Roland ( roland@rptd.ch )
 * 
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License 
 * as published by the Free Software Foundation; either 
 * version 2 of the License, or (at your option) any later 
 * version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dmopmWindowMain.h"
#include "dmopmDialogMapDirectory.h"
#include "../config/dmopmConfig.h"
#include "../file/dmopmArchiveFile.h"
#include "../package/dmopmPackage.h"
#include "../package/dmopmPackageEntry.h"
#include "../exceptions.h"
#include <sha1.h>



// Events
///////////

FXDEFMAP( dmopmDialogMapDirectory ) dmopmDialogMapDirectoryMap[] = {
	FXMAPFUNC( SEL_COMMAND, dmopmDialogMapDirectory::ID_BTN_CANCEL, dmopmDialogMapDirectory::onBtnCancel ),
	FXMAPFUNC( SEL_TIMEOUT, dmopmDialogMapDirectory::ID_TIMER_UPDATE, dmopmDialogMapDirectory::timerUpdate ),
};



// Definitions
////////////////

enum eStates{
	/** Waiting to begin update. */
	esWaiting,
	/** Update package entries from archive. */
	esUpdateEntriesFromArchive,
	/** Update package hash values. */
	esUpdatePackageHashValues,
	/** Scan for files to check. */
	esScanForFiles,
	/** Processing a file. */
	esProcessFile,
	/** Update done. */
	esDone
};



// Class dmopmDialogMapDirectory
//////////////////////////////////

// Constructor, destructor
////////////////////////////

FXIMPLEMENT( dmopmDialogMapDirectory, FXDialogBox, dmopmDialogMapDirectoryMap, ARRAYNUMBER( dmopmDialogMapDirectoryMap ) )

dmopmDialogMapDirectory::dmopmDialogMapDirectory(){ }

dmopmDialogMapDirectory::dmopmDialogMapDirectory( FXWindow *owner, dmopmWindowMain *windowMain, const FXString &pathToMap ) :
FXDialogBox( owner, "Map files against package", DECOR_TITLE | DECOR_CLOSE, 0, 0, 500, 0, 10, 10, 10, 5 ){
	if( ! windowMain || ! windowMain->GetPackage() ){
		THROW( dueInvalidParam );
	}
	
	FXVerticalFrame *content;
	FXVerticalFrame *frameButtons;
	FXHorizontalFrame *frameLine;
	FXMatrix *block;
	
	pWindowMain = windowMain;
	pState = esWaiting;
	pPathToMap = pathToMap;
	if( ! ISPATHSEP( pPathToMap[ pPathToMap.length() - 1 ] ) ){
		pPathToMap += PATHSEPSTRING;
	}
	
	// create content
	content = new FXVerticalFrame( this, LAYOUT_SIDE_TOP | LAYOUT_FILL_X | LAYOUT_FILL_Y, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20 );
	if( ! content ) THROW( dueOutOfMemory );
	
	block = new FXMatrix( content, 2, MATRIX_BY_COLUMNS | LAYOUT_SIDE_BOTTOM | LAYOUT_FILL_X );
	
	new FXLabel( block, "File:", NULL, LABEL_NORMAL, 0, 0, 0, 0, 0, 15, 0, 0 );
	pLabFile = new FXLabel( block, " ", NULL, LABEL_NORMAL | LAYOUT_FILL_X | LAYOUT_FILL_COLUMN  | FRAME_SUNKEN );
	pLabFile->setJustify( JUSTIFY_LEFT );
	
	new FXLabel( block, "Progress:", NULL, LABEL_NORMAL, 0, 0, 0, 0, 0, 15, 0, 0 );
	pPBCurrent = new FXProgressBar( block, NULL, 0, PROGRESSBAR_HORIZONTAL | PROGRESSBAR_PERCENTAGE | LAYOUT_FILL_X | LAYOUT_FILL_COLUMN );
	
	pLabStatus = new FXLabel( content, " ", NULL, LABEL_NORMAL | LAYOUT_FILL_X | FRAME_SUNKEN );
	pLabStatus->setJustify( JUSTIFY_CENTER_X );
	
	pEditLog = new FXText( content, NULL, 0, TEXT_READONLY | TEXT_WORDWRAP | FRAME_SUNKEN | LAYOUT_FILL_X );
	pEditLog->setVisibleRows( 6 );
	
	// buttons below
	frameButtons = new FXVerticalFrame( content, LAYOUT_SIDE_TOP | LAYOUT_FILL_X, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5 );
	if( ! frameButtons ) THROW( dueOutOfMemory );
	
	new FXSeparator( frameButtons );
	
	frameLine = new FXHorizontalFrame( frameButtons, LAYOUT_CENTER_X, 0, 0, 0, 0, 0, 0, 0, 0, 10, 0 );
	if( ! frameLine ) THROW( dueOutOfMemory );
	pBtnClose = new FXButton( frameLine, "Cancel", NULL, this, ID_BTN_CANCEL, BUTTON_DEFAULT | BUTTON_INITIAL | LAYOUT_CENTER_X
		| FRAME_RAISED | JUSTIFY_NORMAL | ICON_BEFORE_TEXT, 0, 0, 0, 0, 30, 30 );
	
	// update
	UpdateStatus();
}

dmopmDialogMapDirectory::~dmopmDialogMapDirectory(){
	getApp()->removeTimeout( this, ID_TIMER_UPDATE );
}



// Management
///////////////

void dmopmDialogMapDirectory::UpdateStatus(){
	try{
		if( pState == esWaiting ){
			FXString text;
			
			pPBCurrent->setTotal( pWindowMain->GetPackage()->GetEntryCount() );
			
			pLabStatus->setText( "Update package entries from archive" );
			pState = esUpdateEntriesFromArchive;
			
			getApp()->addTimeout( this, ID_TIMER_UPDATE, 1 );
			
		}else if( pState == esUpdateEntriesFromArchive ){
			dmopmArchiveFile archiveFile( pWindowMain->GetConfiguration(), pWindowMain->GetPackage() );
			dmopmPackage &package = *pWindowMain->GetPackage();
			dmopmPackageEntry *newEntry = NULL;
			dmopmPackageEntry *matchingEntry;
			dmopmPackage archiveEntries;
			int i, count;
			
			try{
				archiveFile.ReadFileMap( archiveEntries );
				
				count = archiveEntries.GetEntryCount();
				for( i=0; i<count; i++ ){
					const dmopmPackageEntry &entry = *archiveEntries.GetEntryAt( i );
					
					matchingEntry = package.GetEntryWithID( entry.GetID() );
					
					if( matchingEntry ){
						matchingEntry->SetOffset( entry.GetOffset() );
						matchingEntry->SetSize( entry.GetSize() );
						matchingEntry->SetExtra( entry.GetExtra() );
						matchingEntry->SetHashValue( dmopmHashValue() );
						
					}else{
						newEntry = new dmopmPackageEntry;
						newEntry->SetID( entry.GetID() );
						newEntry->SetOffset( entry.GetOffset() );
						newEntry->SetSize( entry.GetSize() );
						newEntry->SetExtra( entry.GetExtra() );
						package.AddEntry( newEntry );
						newEntry = NULL;
					}
				}
				
			}catch( duException e ){
				if( newEntry ){
					delete newEntry;
				}
				throw;
			}
			
			package.SetChanged( true );
			
			pLabStatus->setText( "Prepare package for update" );
			pState = esUpdatePackageHashValues;
			
			getApp()->addTimeout( this, ID_TIMER_UPDATE, 1 );
			
		}else if( pState == esUpdatePackageHashValues ){
			dmopmPackage &package = *pWindowMain->GetPackage();
			const int current = pPBCurrent->getProgress();
			
			if( current < package.GetEntryCount() ){
				dmopmArchiveFile archiveFile( pWindowMain->GetConfiguration(), pWindowMain->GetPackage() );
				dmopmPackageEntry &entry = *package.GetEntryAt( current );
				dmopmHashValue hashValue;
				FXString data;
				SHA1 sha1;
				
				archiveFile.ReadFileData( entry, data );
				sha1.Reset();
				sha1.Input( data.text(), data.length() );
				hashValue.SetValue( sha1 );
				entry.SetHashValue( hashValue );
				
				pPBCurrent->setProgress( current + 1 );
				
				getApp()->addTimeout( this, ID_TIMER_UPDATE, 1 );
				
			}else{
				FXString text;
				
				pPBCurrent->setProgress( 0 );
				pPBCurrent->setTotal( 1 );
				
				pLabStatus->setText( "Scanning for files to map" );
				pState = esScanForFiles;
				
				getApp()->addTimeout( this, ID_TIMER_UPDATE, 1 );
			}
			
		}else if( pState == esScanForFiles ){
			ScanDirectory( pPathToMap );
			
			pPBCurrent->setTotal( pFilesToMap.size() );
			pIterFileToMap = pFilesToMap.begin();
			
			if( pIterFileToMap == pFilesToMap.end() ){
				pLabStatus->setText( "Done" );
				pBtnClose->setText( "Close" );
				pState = esDone;
				
			}else{
				const FXString &path = *pIterFileToMap;
				FXString text;
				
				pLabStatus->setText( "Processing Files" );
				pLabFile->setText( path );
				
				pState = esProcessFile;
				
				getApp()->addTimeout( this, ID_TIMER_UPDATE, 1 );
			}
			
		}else if( pState == esProcessFile ){
			dmopmPackage &package = *pWindowMain->GetPackage();
			int i, packageEntryCount = package.GetEntryCount();
			const FXString &path = *pIterFileToMap;
			const FXString pathAbsolute( FXPath::absolute( pPathToMap, path ) );
			FXFileStream *stream = NULL;
			dmopmHashValue hashValue;
			bool hasMatchingEntries;
			FXString data;
			FXString text;
			SHA1 sha1;
			
			try{
				stream = new FXFileStream;
				if( ! stream ) THROW( dueOutOfMemory );
				stream->open( pathAbsolute, FXStreamLoad );
				
				stream->position( 0, FXFromEnd );
				data.assign( ' ', stream->position() );
				stream->position( 0, FXFromStart );
				stream->load( ( FXchar* )data.text(), data.length() );
				
				stream->close();
				delete stream;
				
			}catch( duException ){
				if( stream ){
					delete stream;
				}
				throw;
			}
			
			sha1.Reset();
			sha1.Input( data.text(), data.length() );
			hashValue.SetValue( sha1 );
			
			hasMatchingEntries = false;
			
			for( i=0; i<packageEntryCount; i++ ){
				dmopmPackageEntry &testEntry = *package.GetEntryAt( i );
				
				if( hashValue == testEntry.GetHashValue() ){
					if( pWindowMain->GetConfiguration()->GetVerbose() ){
						text.format( "File '%s' matches ID '%u'\n", path.text(), testEntry.GetID() );
						printf( "%s", text.text() );
						pEditLog->appendText( text );
					}
					testEntry.SetPath( path );
					hasMatchingEntries = true;
				}
			}
			
			if( ! hasMatchingEntries ){
				text.format( "File '%s' not found, skipped\n", path.text() );
				printf( "%s", text.text() );
				pEditLog->appendText( text );
			}
			
			pIterFileToMap++;
			
			pPBCurrent->increment( 1 );
			
			if( pIterFileToMap == pFilesToMap.end() ){
				pLabStatus->setText( "Done" );
				pBtnClose->setText( "Close" );
				pState = esDone;
				
			}else{
				const FXString &path = *pIterFileToMap;
				FXString text;
				
				pLabFile->setText( path );
				
				pState = esProcessFile;
				
				getApp()->addTimeout( this, ID_TIMER_UPDATE, 1 );
			}
			
		}else{ // esDone
		}
		
	}catch( duException e ){
		pLabStatus->setText( "Failed :(" );
		pBtnClose->setText( "Close" );
		pState = esDone;
		pWindowMain->DisplayException( e );
		return;
	}
}

void dmopmDialogMapDirectory::ScanDirectory( const FXString &path ){
	FXString pathEntry;
	FXString name;
	FXStat info;
	FXDir dir;
	
	if( dir.open( path ) ){
		while( dir.next() ){
			name = dir.name();
			
			if( name == "." || name == ".." ){
				continue;
			}
			
			pathEntry = FXPath::absolute( path, name );
			
#ifdef WIN32
			if( ! FXStat::statFile( pathEntry, info ) ){
				continue;
			}
#else
			if( ! FXStat::statLink( pathEntry, info ) ){
				continue;
			}
			if( info.isLink() && ! FXStat::statFile( pathEntry, info ) ){
				continue;
			}
#endif
			if( info.isDirectory() ){
				ScanDirectory( pathEntry );
				
			}else if( info.isFile() ){
				pFilesToMap.push_back( FXPath::relative( pPathToMap, pathEntry ) );
			}
		}
	}else{
	}
}



// Events
///////////

long dmopmDialogMapDirectory::onBtnCancel( FXObject *sender, FXSelector selector, void *data ){
	return handle( this, FXSEL( SEL_COMMAND, ID_CANCEL ), NULL );
}

long dmopmDialogMapDirectory::timerUpdate( FXObject *sender, FXSelector selector, void *data ){
	UpdateStatus();
	return 0;
}
