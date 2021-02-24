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
#include "dmopmDialogUpdate.h"
#include "../config/dmopmConfig.h"
#include "../file/dmopmArchiveFile.h"
#include "../package/dmopmPackage.h"
#include "../package/dmopmPackageEntry.h"
#include "../package/dmopmDownloader.h"
#include "../package/dmopmMemoryStream.h"
#include "../package/dmopmZipReader.h"
#include "../exceptions.h"
#include <sha1.h>



// Events
///////////

FXDEFMAP( dmopmDialogUpdate ) dmopmDialogUpdateMap[] = {
	FXMAPFUNC( SEL_COMMAND, dmopmDialogUpdate::ID_BTN_CANCEL, dmopmDialogUpdate::onBtnCancel ),
	FXMAPFUNC( SEL_TIMEOUT, dmopmDialogUpdate::ID_TIMER_UPDATE, dmopmDialogUpdate::timerUpdate ),
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
	/** Retrieving server version. */
	esGetServerVersion,
	/** Downloading update zip file. */
	esDownloadZipFile,
	/** Processing update zip file. */
	esProcessZipFile,
	/** Update done. */
	esDone
};



// Class dmopmDialogUpdate
////////////////////////////

// Constructor, destructor
////////////////////////////

FXIMPLEMENT( dmopmDialogUpdate, FXDialogBox, dmopmDialogUpdateMap, ARRAYNUMBER( dmopmDialogUpdateMap ) )

dmopmDialogUpdate::dmopmDialogUpdate(){ }

dmopmDialogUpdate::dmopmDialogUpdate( FXWindow *owner, dmopmWindowMain *windowMain ) :
FXDialogBox( owner, "Update from Digitalic Server", DECOR_TITLE | DECOR_CLOSE, 0, 0, 500, 0, 10, 10, 10, 5 ){
	if( ! windowMain || ! windowMain->GetPackage() ){
		THROW( dueInvalidParam );
	}
	
	FXVerticalFrame *content;
	FXVerticalFrame *frameButtons;
	FXHorizontalFrame *frameLine;
	FXMatrix *block;
	
	pWindowMain = windowMain;
	pState = esWaiting;
	pCurVersion = -1;
	pServerVersion = -1;
	
	// create content
	content = new FXVerticalFrame( this, LAYOUT_SIDE_TOP | LAYOUT_FILL_X | LAYOUT_FILL_Y, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20 );
	if( ! content ) THROW( dueOutOfMemory );
	
	block = new FXMatrix( content, 2, MATRIX_BY_COLUMNS | LAYOUT_SIDE_BOTTOM | LAYOUT_FILL_X );
	
	new FXLabel( block, "Server Version:", NULL, LABEL_NORMAL, 0, 0, 0, 0, 0, 15, 0, 0 );
	pLabServerVersion = new FXLabel( block, "?", NULL, LABEL_NORMAL | LAYOUT_FILL_X | LAYOUT_FILL_COLUMN );
	pLabServerVersion->setJustify( JUSTIFY_LEFT );
	
	new FXLabel( block, "Package Version:", NULL, LABEL_NORMAL, 0, 0, 0, 0, 0, 15, 0, 0 );
	pLabPackageVersion = new FXLabel( block, "?", NULL, LABEL_NORMAL | LAYOUT_FILL_X | LAYOUT_FILL_COLUMN );
	pLabPackageVersion->setJustify( JUSTIFY_LEFT );
	
	new FXLabel( block, "Overall Progress:", NULL, LABEL_NORMAL, 0, 0, 0, 0, 0, 15, 0, 0 );
	pPBOverall = new FXProgressBar( block, NULL, 0, PROGRESSBAR_HORIZONTAL | PROGRESSBAR_PERCENTAGE | LAYOUT_FILL_X | LAYOUT_FILL_COLUMN );
	
	new FXLabel( block, "Current Progress:", NULL, LABEL_NORMAL, 0, 0, 0, 0, 0, 15, 0, 0 );
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

dmopmDialogUpdate::~dmopmDialogUpdate(){
	getApp()->removeTimeout( this, ID_TIMER_UPDATE );
}



// Management
///////////////

void dmopmDialogUpdate::UpdateStatus(){
	try{
		if( pState == esWaiting ){
			if( pWindowMain->GetPackage()->GetGameType() == pWindowMain->GetConfiguration()->GetGameType() ){
				FXString text;
				
				pCurVersion = pWindowMain->GetPackage()->GetVersion();
				
				text.format( "%i", pCurVersion );
				pLabPackageVersion->setText( text );
				
				pPBCurrent->setTotal( pWindowMain->GetPackage()->GetEntryCount() );
				
				pLabStatus->setText( "Update package entries from archive" );
				pState = esUpdateEntriesFromArchive;
				
				getApp()->addTimeout( this, ID_TIMER_UPDATE, 1 );
				
			}else{
				pLabStatus->setText( "Package does not match installed game type" );
				pState = esDone;
				
				getApp()->addTimeout( this, ID_TIMER_UPDATE, 1 );
			}
			
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
				
				pLabStatus->setText( "Retrieving Server Version" );
				pState = esGetServerVersion;
				
				if( package.GetGameType() == dmopmPackage::egtEnglish ){
					text = "http://patch.dmo.joymax.com/PatchInfo_GDMO.ini";
					
				}else{
					text = "http://digimonmasters.nowcdn.co.kr/S1/PatchInfo.ini";
				}
				pWindowMain->GetDownloader()->GetFileAsync( text );
				
				getApp()->addTimeout( this, ID_TIMER_UPDATE, 500 );
			}
			
		}else if( pState == esGetServerVersion ){
			dmopmDownloader &downloader = *pWindowMain->GetDownloader();
			
			downloader.UpdateAsyncDownload();
			
			if( ! downloader.GetDoesAsyncDownload() ){
				dmopmPackage &package = *pWindowMain->GetPackage();
				const FXString &data = downloader.GetDataAsync();
				int strFrom, strTo;
				FXString text;
				
				strFrom = data.find( "version=" );
				if( strFrom == -1 ){
					THROW( dueInvalidParam );
				}
				
				strTo = data.find( '\n', strFrom + 1 );
				if( strTo == -1 ){
					strTo = data.length();
				}
				
				pServerVersion = FXIntVal( data.mid( strFrom + 8, strTo - strFrom - 8 ) );
				
				text.format( "%i", pServerVersion );
				pLabServerVersion->setText( text );
				
				if( pServerVersion <= pCurVersion ){
					pLabStatus->setText( "No updates found" );
					pBtnClose->setText( "Close" );
					pState = esDone;
					return;
				}
				
				pPBOverall->setTotal( pServerVersion - pCurVersion );
				
				pCurVersion++;
				
				text.format( "Downloading %i", pCurVersion );
				pLabStatus->setText( text );
				
				if( package.GetGameType() == dmopmPackage::egtEnglish ){
					text.format( "http://patch.dmo.joymax.com/GDMO%i.zip", pCurVersion );
					
				}else{
					text.format( "http://digimonmasters.nowcdn.co.kr/S1/%i.zip", pCurVersion );
				}
				pWindowMain->GetDownloader()->GetFileAsync( text );
				
				pState = esDownloadZipFile;
			}
			
			getApp()->addTimeout( this, ID_TIMER_UPDATE, 500 );
			
		}else if( pState == esDownloadZipFile ){
			dmopmDownloader &downloader = *pWindowMain->GetDownloader();
			FXString text;
			
			downloader.UpdateAsyncDownload();
			
			if( downloader.GetDoesAsyncDownload() ){
				FXuint progress, total;
				
				downloader.GetProgressAsync( progress, total );
				
				text.format( "Downloading %i (%.2fM/%.2fM)", pCurVersion, ( float )progress / 1048576.0f, ( float )total / 1048576.0f );
				pLabStatus->setText( text );
				
				pPBCurrent->setProgress( progress );
				
				if( total != pPBCurrent->getTotal() ){
					pPBCurrent->setTotal( total );
				}
				
				getApp()->addTimeout( this, ID_TIMER_UPDATE, 500 );
				
			}else{
				text.format( "Processing %i", pCurVersion );
				pLabStatus->setText( text );
				
				pState = esProcessZipFile;
				
				getApp()->addTimeout( this, ID_TIMER_UPDATE, 1 );
			}
			
		}else if( pState == esProcessZipFile ){
			dmopmPackage &package = *pWindowMain->GetPackage();
			const FXString &contentPathPrefix = package.GetContentPathPrefix();
			const int cpflen = contentPathPrefix.length();
			const FXString &zipData = pWindowMain->GetDownloader()->GetDataAsync();
			int j, packageEntryCount = package.GetEntryCount();
			dmopmMemoryStream *stream = NULL;
			dmopmZipReader zipReader;
			bool hasMatchingEntries;
			FXString text;
			int i, count;
			
			try{
				stream = new dmopmMemoryStream;
				stream->open( FXStreamLoad, zipData.length(), ( FXuchar* )zipData.text() );
				
				if( package.GetGameType() == dmopmPackage::egtEnglish ){
					text.format( "GDMO%i.zip", pCurVersion );
					
				}else{
					text.format( "%i.zip", pCurVersion );
				}
				zipReader.Open( stream, text );
				stream = NULL;
				
				while( zipReader.ReadNextEntry() );
				
				zipReader.Close();
				
			}catch( duException e ){
				zipReader.Close();
				if( stream ){
					delete stream;
				}
				throw;
			}
			
			count = zipReader.GetEntryCount();
			for( i=0; i<count; i++ ){
				const dmopmPackageEntry &entry = *zipReader.GetEntryAt( i );
				
				if( strncmp( entry.GetPath().text(), contentPathPrefix.text(), cpflen ) == 0 ){
					hasMatchingEntries = false;
					
					for( j=0; j<packageEntryCount; j++ ){
						dmopmPackageEntry &testEntry = *package.GetEntryAt( j );
						
						if( entry.GetHashValue() == testEntry.GetHashValue() ){
							if( pWindowMain->GetConfiguration()->GetVerbose() ){
								text.format( "Update %i: Entry '%s' matches ID '%u'\n", pCurVersion, entry.GetPath().text(), testEntry.GetID() );
								printf( "%s", text.text() );
								pEditLog->appendText( text );
							}
							testEntry.SetPath( FXPath::relative( contentPathPrefix, entry.GetPath() ) );
							hasMatchingEntries = true;
						}
					}
					
					if( ! hasMatchingEntries ){
						text.format( "Update %i: Entry '%s' not found, skipped\n", pCurVersion, entry.GetPath().text() );
						printf( "%s", text.text() );
						pEditLog->appendText( text );
					}
				}
			}
			
			package.SetVersion( pCurVersion );
			package.SetChanged( true );
			
			text.format( "%i", pCurVersion );
			pLabPackageVersion->setText( text );
			
			pCurVersion++;
			
			pPBCurrent->setProgress( 0 );
			pPBCurrent->setTotal( 1 );
			
			pPBOverall->increment( 1 );
			
			if( pCurVersion > pServerVersion ){
				pLabStatus->setText( "Done" );
				pBtnClose->setText( "Close" );
				pState = esDone;
				
			}else{
				text.format( "Downloading %i", pCurVersion );
				pLabStatus->setText( text );
				
				if( package.GetGameType() == dmopmPackage::egtEnglish ){
					text.format( "http://patch.dmo.joymax.com/GDMO%i.zip", pCurVersion );
					
				}else{
					text.format( "http://digimonmasters.nowcdn.co.kr/S1/%i.zip", pCurVersion );
				}
				pWindowMain->GetDownloader()->GetFileAsync( text );
				
				pState = esDownloadZipFile;
				
				getApp()->addTimeout( this, ID_TIMER_UPDATE, 500 );
			}
			
		}else{ // esDone
		}
		
	}catch( duException e ){
		pLabStatus->setText( "Failed :(" );
		pBtnClose->setText( "Close" );
		pState = esDone;
		pWindowMain->GetDownloader()->StopGetFileAsync();
		pWindowMain->DisplayException( e );
		return;
	}
}



// Events
///////////

long dmopmDialogUpdate::onBtnCancel( FXObject *sender, FXSelector selector, void *data ){
	pWindowMain->GetDownloader()->StopGetFileAsync();
	return handle( this, FXSEL( SEL_COMMAND, ID_CANCEL ), NULL );
}

long dmopmDialogUpdate::timerUpdate( FXObject *sender, FXSelector selector, void *data ){
	UpdateStatus();
	return 0;
}
