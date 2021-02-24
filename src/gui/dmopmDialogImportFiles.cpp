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
#include "dmopmDialogImportFiles.h"
#include "../file/dmopmArchiveFile.h"
#include "../package/dmopmPackage.h"
#include "../package/dmopmPackageEntry.h"
#include "../exceptions.h"
#include "../config/dmopmConfig.h"
#include "../package/dmopmZipReader.h"



// Events
///////////

FXDEFMAP( dmopmDialogImportFiles ) dmopmDialogImportFilesMap[] = {
	FXMAPFUNC( SEL_COMMAND, dmopmDialogImportFiles::ID_BTN_CANCEL, dmopmDialogImportFiles::onBtnCancel ),
	FXMAPFUNC( SEL_TIMEOUT, dmopmDialogImportFiles::ID_TIMER_UPDATE, dmopmDialogImportFiles::timerUpdate ),
};



// Definitions
////////////////

enum eStates{
	/** Waiting to begin importing files. */
	esWaiting,
	/** Update package entries. */
	esUpdatePackageEntries,
	/** Read zip file. */
	esReadZipFile,
	/** Init temporary files. */
	esInitTempFiles,
	/** Process an entry. */
	esProcessEntry,
	/** Finalize. */
	esFinalize,
	/** Import done. */
	esDone
};



// Class dmopmDialogImportFiles
/////////////////////////////////

// Constructor, destructor
////////////////////////////

FXIMPLEMENT( dmopmDialogImportFiles, FXDialogBox, dmopmDialogImportFilesMap, ARRAYNUMBER( dmopmDialogImportFilesMap ) )

dmopmDialogImportFiles::dmopmDialogImportFiles(){ }

dmopmDialogImportFiles::dmopmDialogImportFiles( FXWindow *owner, dmopmWindowMain *windowMain ) :
FXDialogBox( owner, "Import Files", DECOR_TITLE | DECOR_CLOSE, 0, 0, 500, 0, 10, 10, 10, 5 ){
	if( ! windowMain || ! windowMain->GetPackage() ){
		THROW( dueInvalidParam );
	}
	
	FXVerticalFrame *content;
	FXVerticalFrame *frameButtons;
	FXHorizontalFrame *frameLine;
	FXMatrix *block;
	
	pWindowMain = windowMain;
	pImportZIP = NULL;
	pPatchedFilesCount = 0;
	pState = esWaiting;
	pNextEntry = 0;
	pStreamArchive = NULL;
	pStreamFilemap = NULL;
	
	// create content
	content = new FXVerticalFrame( this, LAYOUT_SIDE_TOP | LAYOUT_FILL_X | LAYOUT_FILL_Y, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20 );
	if( ! content ) THROW( dueOutOfMemory );
	
	block = new FXMatrix( content, 2, MATRIX_BY_COLUMNS | LAYOUT_SIDE_BOTTOM | LAYOUT_FILL_X );
	
	new FXLabel( block, "Progress:", NULL, LABEL_NORMAL, 0, 0, 0, 0, 0, 15, 0, 0 );
	pPBOverall = new FXProgressBar( block, NULL, 0, PROGRESSBAR_HORIZONTAL | PROGRESSBAR_PERCENTAGE | LAYOUT_FILL_X | LAYOUT_FILL_COLUMN );
	
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
	pBtnCancel = new FXButton( frameLine, "Cancel", NULL, this, ID_BTN_CANCEL, BUTTON_DEFAULT | BUTTON_INITIAL | LAYOUT_CENTER_X
		| FRAME_RAISED | JUSTIFY_NORMAL | ICON_BEFORE_TEXT, 0, 0, 0, 0, 30, 30 );
	
	// update
	UpdateStatus();
}

dmopmDialogImportFiles::~dmopmDialogImportFiles(){
	getApp()->removeTimeout( this, ID_TIMER_UPDATE );
	
	if( pImportZIP ){
		delete pImportZIP;
	}
	if( pStreamArchive ){
		delete pStreamArchive;
	}
	if( pStreamFilemap ){
		delete pStreamFilemap;
	}
}



// Management
///////////////

void dmopmDialogImportFiles::SetPathImportZIP( const FXString &path ){
	pPathImportZIP = path;
}

void dmopmDialogImportFiles::UpdateStatus(){
	try{
		if( pState == esWaiting ){
			pLabStatus->setText( "Prepare package for update" );
			pState = esUpdatePackageEntries;
			
			getApp()->addTimeout( this, ID_TIMER_UPDATE, 1 );
			
		}else if( pState == esUpdatePackageEntries ){
			dmopmArchiveFile archiveFile( pWindowMain->GetConfiguration(), pWindowMain->GetPackage() );
			dmopmPackage &package = *pWindowMain->GetPackage();
			dmopmPackageEntry *newEntry = NULL;
			dmopmPackageEntry *matchingEntry;
			dmopmPackage archiveEntries;
			FXString text;
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
			
			pPBOverall->setTotal( package.GetEntryCount() );
			
			if( package.GetEntryCount() == 0 ){
				pLabStatus->setText( "Empty Package" );
				pState = esDone;
				
			}else{
				if( pPathImportZIP.empty() ){
					const dmopmPackageEntry &entry = *package.GetEntryAt( 0 );
					
					pNextEntry = 0;
					
					if( entry.GetImportPath().empty() ){
						text.format( "Copy %u", entry.GetID() );
						
					}else{
						text.format( "Update %u", entry.GetID() );
					}
					pLabStatus->setText( text );
					pState = esInitTempFiles;
					
				}else{
					pLabStatus->setText( "Reading ZIP File" );
					pState = esReadZipFile;
				}
			}
			
			getApp()->addTimeout( this, ID_TIMER_UPDATE, 1 );
			
		}else if( pState == esReadZipFile ){
			FXFileStream *stream = NULL;
			
			pImportZIP = new dmopmZipReader;
			pImportZIP->SetReadData( true );
			
			try{
				stream = new FXFileStream;
				stream->open( pPathImportZIP, FXStreamLoad );
				
				pImportZIP->Open( stream, pPathImportZIP );
				stream = NULL;
				
				while( pImportZIP->ReadNextEntry() );
				
				pImportZIP->Close();
				
			}catch( duException e ){
				if( pImportZIP ){
					pImportZIP->Close();
					delete pImportZIP;
					pImportZIP = NULL;
				}
				if( stream ){
					delete stream;
				}
				throw;
			}
			
			dmopmPackage &package = *pWindowMain->GetPackage();
			const dmopmPackageEntry &entry = *package.GetEntryAt( 0 );
			FXString text;
			
			pNextEntry = 0;
			
			if( entry.GetImportPath().empty() ){
				text.format( "Copy %u", entry.GetID() );
				
			}else{
				text.format( "Update %u", entry.GetID() );
			}
			pLabStatus->setText( text );
			pState = esInitTempFiles;
			
			getApp()->addTimeout( this, ID_TIMER_UPDATE, 1 );
			
		}else if( pState == esInitTempFiles ){
			const FXString &pathDMO = pWindowMain->GetConfiguration()->GetPathDMO();
			dmopmPackage &package = *pWindowMain->GetPackage();
			FXuint valueUInt;
			
			if( pImportZIP ){
				delete pImportZIP;
				pImportZIP = NULL;
			}
			
			pStreamFilemap = new FXFileStream;
			pStreamFilemap->open( FXPath::absolute( pathDMO, package.GetPathHF() + ".temp" ), FXStreamSave );
			
			pStreamArchive = new FXFileStream;
			pStreamArchive->open( FXPath::absolute( pathDMO, package.GetPathPF() + ".temp" ), FXStreamSave );
			
			valueUInt = 16;
			*pStreamFilemap << valueUInt;
			valueUInt = package.GetEntryCount();
			*pStreamFilemap << valueUInt;
			
			valueUInt = 16;
			*pStreamArchive << valueUInt;
			valueUInt = 0;
			*pStreamArchive << valueUInt;
			
			pPadding.assign( ' ', 268 );
			
			pPBOverall->setProgress( 0 );
			
			pNextEntry = 0;
			
			pState = esProcessEntry;
			
			getApp()->addTimeout( this, ID_TIMER_UPDATE, 1 );
			
		}else if( pState == esProcessEntry ){
			dmopmArchiveFile archive( pWindowMain->GetConfiguration(), pWindowMain->GetPackage() );
			dmopmPackage &package = *pWindowMain->GetPackage();
			dmopmPackageEntry &entry = *package.GetEntryAt( pNextEntry++ );
			FXFileStream *stream = NULL;
			FXString text;
			
			// read data to use either from the archive files or from file/zip
			if( pImportZIP ){
				const int zipEntryCount = pImportZIP->GetEntryCount();
				dmopmPackageEntry *matchingEntry = NULL;
				dmopmPackageEntry *zipEntry;
				int i;
				
				for( i=0; i<zipEntryCount; i++ ){
					zipEntry = pImportZIP->GetEntryAt( i );
					
					if( comparecase( zipEntry->GetPath(), entry.GetPath() ) == 0 ){
						matchingEntry = zipEntry;
						break;
					}
				}
				
				if( matchingEntry ){
					entry.GetData() = matchingEntry->GetData();
					pPatchedFilesCount++;
					
				}else{
					archive.ReadFileData( entry, entry.GetData() );
				}
				
			}else{
				if( entry.GetImportPath().empty() ){
					archive.ReadFileData( entry, entry.GetData() );
					
				}else{
					FXString &data = entry.GetData();
					FXlong size;
					
					try{
						stream = new FXFileStream;
						if( ! stream ) THROW( dueOutOfMemory );
						stream->open( entry.GetImportPath(), FXStreamLoad );
						
						stream->position( 0, FXFromEnd );
						size = stream->position();
						stream->position( 0, FXFromStart );
						
						data.assign( ' ', size );
						stream->load( ( FXchar* )data.text(), size );
						
						delete stream;
						
					}catch( duException ){
						if( stream ){
							delete stream;
						}
						throw;
					}
					
					pPatchedFilesCount++;
				}
			}
			
			// write entry
			FXString &data = entry.GetData();
			FXuint valueUInt;
			
			pStreamArchive->save( pPadding.text(), pPadding.length() );
			entry.SetOffset( ( unsigned int )pStreamArchive->position() );
			pStreamArchive->save( data.text(), data.length() );
			entry.SetSize( ( unsigned int )data.length() );
			data = "";
			entry.SetImportPath( "" );
			package.SetChanged( true );
			
			valueUInt = 1;
			*pStreamFilemap << valueUInt;
			valueUInt = entry.GetSize();
			*pStreamFilemap << valueUInt;
			*pStreamFilemap << valueUInt;
			valueUInt = entry.GetID();
			*pStreamFilemap << valueUInt;
			valueUInt = entry.GetOffset();
			*pStreamFilemap << valueUInt;
			valueUInt = entry.GetExtra();
			*pStreamFilemap << valueUInt;
			
			entry.GetData() = "";
			
			// go to next entry
			pPBOverall->increment( 1 );
			
			if( pNextEntry == package.GetEntryCount() ){
				const FXString &pathDMO = pWindowMain->GetConfiguration()->GetPathDMO();
				
				delete pStreamArchive;
				pStreamArchive = NULL;
				
				delete pStreamFilemap;
				pStreamFilemap = NULL;
				
				FXFile::remove( FXPath::absolute( pathDMO, package.GetPathHF() ) );
				FXFile::rename( FXPath::absolute( pathDMO, package.GetPathHF() + ".temp" ), FXPath::absolute( pathDMO, package.GetPathHF() ) );
				
				FXFile::remove( FXPath::absolute( pathDMO, package.GetPathPF() ) );
				FXFile::rename( FXPath::absolute( pathDMO, package.GetPathPF() + ".temp" ), FXPath::absolute( pathDMO, package.GetPathPF() ) );
				
				pLabStatus->setText( "Finalize" );
				pState = esFinalize;
				
			}else{
				const dmopmPackageEntry &entry = *package.GetEntryAt( pNextEntry );
				
				if( entry.GetData().empty() /*entry.GetImportPath().empty()*/ ){
					text.format( "Copy %u", entry.GetID() );
					
				}else{
					text.format( "Update %u", entry.GetID() );
				}
				pLabStatus->setText( text );
			}
			
			getApp()->addTimeout( this, ID_TIMER_UPDATE, 1 );
			
		}else if( pState == esFinalize ){
			FXString text;
			
			pBtnCancel->enable();
			pBtnCancel->setText( "Close" );
			
			pLabStatus->setText( "Done" );
			pState = esDone;
			
			FXMessageBox::information( this, MBOX_OK, "Import Done", "%i files imported", pPatchedFilesCount );
			
			text = "The file map of the package has changed. You have to save the package file\n";
			text += "or import the archive the next time you open this package file.";
			
			FXMessageBox::information( this, MBOX_OK, "Import Done", "%s", text.text() );
		}
		
	}catch( duException e ){
		const FXString &pathDMO = pWindowMain->GetConfiguration()->GetPathDMO();
		dmopmPackage &package = *pWindowMain->GetPackage();
		
		pBtnCancel->enable();
		pBtnCancel->setText( "Close" );
		pLabStatus->setText( "Failed :(" );
		pState = esDone;
		
		if( pImportZIP ){
			delete pImportZIP;
			pImportZIP = NULL;
		}
		
		if( pStreamArchive ){
			delete pStreamArchive;
			pStreamArchive = NULL;
		}
		if( pStreamFilemap ){
			delete pStreamFilemap;
			pStreamFilemap = NULL;
		}
		FXFile::remove( FXPath::absolute( pathDMO, package.GetPathHF() + ".temp" ) );
		FXFile::remove( FXPath::absolute( pathDMO, package.GetPathPF() + ".temp" ) );
		
		pWindowMain->DisplayException( e );
		
		return;
	}
}



// Events
///////////

long dmopmDialogImportFiles::onBtnCancel( FXObject *sender, FXSelector selector, void *data ){
	if( pStreamArchive || pStreamFilemap ){
		return 1;
	}
	
	return handle( this, FXSEL( SEL_COMMAND, ID_CANCEL ), NULL );
}

long dmopmDialogImportFiles::timerUpdate( FXObject *sender, FXSelector selector, void *data ){
	UpdateStatus();
	return 0;
}
