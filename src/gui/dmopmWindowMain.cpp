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
#include "dmopmPanelEntries.h"
#include "dmopmDialogUpdate.h"
#include "dmopmDialogEditPackage.h"
#include "dmopmDialogImportFiles.h"
#include "../config/dmopmConfig.h"
#include "../file/dmopmLoadSavePackage.h"
#include "../package/dmopmPackage.h"
#include "../package/dmopmZipReader.h"
#include "../package/dmopmPackageEntry.h"
#include "../package/dmopmDownloader.h"
#include "../package/dmopmMemoryStream.h"
#include "../exceptions.h"
#include "../file/dmopmArchiveFile.h"
#include "dmopmDialogMapDirectory.h"



// Events
///////////

FXDEFMAP( dmopmWindowMain ) dmopmWindowMainMap[] = {
	FXMAPFUNC( SEL_CLOSE, 0, dmopmWindowMain::onClose ),
	FXMAPFUNC( SEL_COMMAND, dmopmWindowMain::ID_FILE_NEW, dmopmWindowMain::onFileNew ),
	FXMAPFUNC( SEL_COMMAND, dmopmWindowMain::ID_FILE_OPEN, dmopmWindowMain::onFileOpen ),
	FXMAPFUNC( SEL_COMMAND, dmopmWindowMain::ID_FILE_SAVE, dmopmWindowMain::onFileSave ),
	FXMAPFUNC( SEL_COMMAND, dmopmWindowMain::ID_FILE_SAVEAS, dmopmWindowMain::onFileSaveAs ),
	FXMAPFUNC( SEL_COMMAND, dmopmWindowMain::ID_FILE_SETDMOPATH, dmopmWindowMain::onFileSetDMOPath ),
	FXMAPFUNC( SEL_COMMAND, dmopmWindowMain::ID_FILE_QUIT, dmopmWindowMain::onFileQuit ),
	
	FXMAPFUNC( SEL_COMMAND, dmopmWindowMain::ID_PACK_IMPORT_ARCHIVE, dmopmWindowMain::onPackImportArchive ),
	FXMAPFUNC( SEL_COMMAND, dmopmWindowMain::ID_PACK_MAP_DIRECTORY, dmopmWindowMain::onPackMapDirectory ),
	FXMAPFUNC( SEL_COMMAND, dmopmWindowMain::ID_PACK_UPDATE, dmopmWindowMain::onPackUpdate ),
	FXMAPFUNC( SEL_COMMAND, dmopmWindowMain::ID_PACK_PROPERTIES, dmopmWindowMain::onPackProperties ),
	FXMAPFUNC( SEL_COMMAND, dmopmWindowMain::ID_PACK_IMPORT_FILES, dmopmWindowMain::onPackImportFiles ),
	FXMAPFUNC( SEL_COMMAND, dmopmWindowMain::ID_PACK_IMPORT_FROM_ZIP, dmopmWindowMain::onPackImportFromZip ),
	
	FXMAPFUNC( SEL_COMMAND, dmopmWindowMain::ID_OPTIONS_VERBOSE, dmopmWindowMain::onOptionsVerbose ),
	FXMAPFUNC( SEL_UPDATE, dmopmWindowMain::ID_OPTIONS_VERBOSE, dmopmWindowMain::updateOptionsVerbose ),
};



// Class dmopmWindowMain
//////////////////////////

// Constructor, destructor
////////////////////////////

FXIMPLEMENT( dmopmWindowMain, FXMainWindow, dmopmWindowMainMap, ARRAYNUMBER( dmopmWindowMainMap ) )

dmopmWindowMain::dmopmWindowMain(){ }

void dmopmWindowMain::create(){
	FXMainWindow::create();
	
	show( PLACEMENT_SCREEN );
	
	if( pConfig->GetPathDMO().empty() ){
		FXMessageBox::information( this, MBOX_OK, "DMO Path", "There is no DMO Path set. Please select the path where the DigimonMaster.exe is located." );
		
		FXDirDialog dialog( this, "Select DMO Path" );
		
		if( dialog.execute( PLACEMENT_OWNER ) == TRUE ){
			pConfig->SetPathDMO( dialog.getDirectory() );
			
		}else{
			FXMessageBox::information( this, MBOX_OK, "DMO Path", "You have to set the DMO path otherwise this application can not function properly. Use 'File'->'DMO Path' to do this." );
		}
	}
	
	DetermineGameType();
	NewPackage();
	UpdateWindowTitle();
}

dmopmWindowMain::dmopmWindowMain( FXApp *app, int argc, char **argv ) :
FXMainWindow( app, "DMO Pack Manager", NULL, NULL, DECOR_ALL, 0, 0, 800, 600 ){
	pPanelEntries = NULL;
	pConfig = NULL;
	pDownloader = NULL;
	pPackage = NULL;
	
	// load configuration
	pConfig = new dmopmConfig;
	
	// create downloader
	pDownloader = new dmopmDownloader;
	
	// create menu bar
	pMenuBar = new FXMenuBar( this, LAYOUT_SIDE_TOP | LAYOUT_FILL_X );
	
	FXMenuPane *menuFile = new FXMenuPane( this );
	new FXMenuTitle( pMenuBar, "&File", NULL, menuFile );
	
	new FXMenuCommand( menuFile, "&New...\t\tNew Package", NULL, this, ID_FILE_NEW );
	new FXMenuCommand( menuFile, "&Open...\t\tOpen Package", NULL, this, ID_FILE_OPEN );
	new FXMenuCommand( menuFile, "&Save...\t\tSave Package", NULL, this, ID_FILE_SAVE );
	new FXMenuCommand( menuFile, "Save &As...\t\tSave Package under a different file", NULL, this, ID_FILE_SAVEAS );
	
	new FXMenuSeparator( menuFile );
	new FXMenuCommand( menuFile, "&Set DMO Path\t\tSet DMO Path", NULL, this, ID_FILE_SETDMOPATH );
	
	new FXMenuSeparator( menuFile );
	new FXMenuCommand( menuFile, "&Quit\t\tQuits application", NULL, this, ID_FILE_QUIT );
	
	FXMenuPane *menuPackage = new FXMenuPane( this );
	new FXMenuTitle( pMenuBar, "&Package", NULL, menuPackage );
	
	new FXMenuCommand( menuPackage, "&Package Properties...\t\tEdit Package Properties", NULL, this, ID_PACK_PROPERTIES );
	new FXMenuSeparator( menuPackage );
	new FXMenuCommand( menuPackage, "&Import Archive\t\tImport Archive Entries into Package", NULL, this, ID_PACK_IMPORT_ARCHIVE );
	new FXMenuCommand( menuPackage, "&Map Directory\t\tMap files in a directory against Package", NULL, this, ID_PACK_MAP_DIRECTORY );
	new FXMenuSeparator( menuPackage );
	new FXMenuCommand( menuPackage, "&Update...\t\tUpdate from Digitalic Server", NULL, this, ID_PACK_UPDATE );
	new FXMenuSeparator( menuPackage );
	new FXMenuCommand( menuPackage, "&Import Files...\t\tImport files marked for import", NULL, this, ID_PACK_IMPORT_FILES );
	new FXMenuCommand( menuPackage, "&Import Files From ZIP...\t\tImport files from a zip archive", NULL, this, ID_PACK_IMPORT_FROM_ZIP );
	
	FXMenuPane *menuOptions = new FXMenuPane( this );
	new FXMenuTitle( pMenuBar, "&Options", NULL, menuOptions );
	
	new FXMenuCheck( menuOptions, "&Verbose\t\tDetermines if verbose output mode is used", this, ID_OPTIONS_VERBOSE );
	
	// create content area
	FXPacker *content = new FXPacker( this, LAYOUT_FILL_X | LAYOUT_FILL_Y, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 );
	
	pStatusBar = new FXStatusBar( content, LAYOUT_SIDE_BOTTOM | LAYOUT_FILL_X );
	pStatusBar->getStatusLine()->setNormalText( "Ready" );
	
	pSBProgress = new FXProgressBar( pStatusBar, NULL, 0, PROGRESSBAR_HORIZONTAL | LAYOUT_FIX_WIDTH | LAYOUT_FILL_Y | FRAME_SUNKEN );
	pSBProgress->setWidth( 200 );
	pSBProgress->setTotal( 100 );
	pSBProgress->setProgress( 0 );
	pSBProgress->hide();
	
	pPanelEntries = new dmopmPanelEntries( this, content );
	if( ! pPanelEntries ) THROW( dueOutOfMemory );
	
	// tool tips
	pToolTip = new FXToolTip( app, 0 );
	
	// load configuration
	FXRegistry &registry = app->reg();
	const char *sectionApp = "application";
	
	pConfig->SetPathDMO( registry.readStringEntry( sectionApp, "pathDMO", "" ) );
}

dmopmWindowMain::~dmopmWindowMain(){
	FXRegistry &registry = getApp()->reg();
	const char *sectionApp = "application";
	
	SetPackage( NULL );
	
	registry.writeStringEntry( sectionApp, "pathDMO", pConfig->GetPathDMO().text() );
	
	if( pDownloader ){
		delete pDownloader;
	}
	if( pConfig ){
		delete pConfig;
	}
}



// Management
///////////////

void dmopmWindowMain::DisplayException( const duException &exception ){
	FXString caption, message;
	
	caption = "Application Error";
	
	message.format( "An exception occured. File='%s', Line='%i', Reason='%s'.",
		exception.GetFile(), exception.GetLine(), exception.GetDescription() );
	
	FXMessageBox::error( this, FX::MBOX_OK, caption.text(), "%s", message.text() );
}



void dmopmWindowMain::SetProgressVisible( bool visible ){
	if( visible ){
		pSBProgress->show();
		
	}else{
		pSBProgress->hide();
	}
	
	pSBProgress->recalc();
	pStatusBar->recalc();
}

void dmopmWindowMain::SetProgress( float progress ){
	pSBProgress->setProgress( ( int )( progress * 100.0f ) );
}

void dmopmWindowMain::SetProgressText( const FXString &text ){
	FXStatusLine *statusLine = pStatusBar->getStatusLine();
	
	statusLine->setNormalText( text );
	statusLine->setText( text );
}



bool dmopmWindowMain::QuitRequest(){
	if( pPackage && pPackage->GetChanged() ){
		int result = FXMessageBox::question( this, MBOX_YES_NO_CANCEL, "Quit Application",
			"The package has changed since the last time it has been saved. Do you want to save the package before leaving?" );
		
		if( result == MBOX_CLICKED_YES ){
			if( onFileSave( this, SEL_COMMAND, NULL ) == 0 ){
				return false;
			}
			
		}else if( result == MBOX_CLICKED_CANCEL ){
			return false;
		}
	}
	
	return true;
}



void dmopmWindowMain::SetPackage( dmopmPackage *package ){
	if( package != pPackage ){
		if( pPackage ) delete pPackage;
		pPackage = package;
		
		pPanelEntries->UpdatePackage();
	}
}

void dmopmWindowMain::NewPackage(){
	dmopmPackage *package = NULL;
	
	try{
		package = new dmopmPackage;
		if( ! package ) THROW( dueOutOfMemory );
		
		SetPackage( package );
		package = NULL;
		
	}catch( duException e ){
		DisplayException( e );
	}
}

void dmopmWindowMain::SavePackage( const FXString &filename ){
	if( pPackage ){
		dmopmLoadSavePackage lspackage;
		
		lspackage.SaveToFile( filename, *pPackage );
		
		pPackage->SetFilename( filename );
		pPackage->SetSaved( true );
		pPackage->SetChanged( false );
	}
}



void dmopmWindowMain::DetermineGameType(){
	const FXString &pathDMO = pConfig->GetPathDMO();
	FXString pathAbsolute;
	
	// test for korean version
	pathAbsolute = FXPath::absolute( pathDMO, "DigimonMasters.exe" );
	if( FXStat::isFile( pathAbsolute ) ){
		pConfig->SetGameType( dmopmPackage::egtKorean );
		return;
	}
	
	// test for english version
	pathAbsolute = FXPath::absolute( pathDMO, "GDMO.exe" );
	if( FXStat::isFile( pathAbsolute ) ){
		pConfig->SetGameType( dmopmPackage::egtEnglish );
		return;
	}
	
	// nothing found which looks like a valid dmo game
	pConfig->SetGameType( -1 );
	FXMessageBox::information( this, MBOX_OK, "DMO Path", "There seems to be no valid DMO game installed at '%s'. Can not determine game type. Use 'File'->'DMO Path' to select a valid game path.", pConfig->GetPathDMO().text() );
}

void dmopmWindowMain::UpdateWindowTitle(){
	FXString text;
	
	if( pConfig->GetGameType() == dmopmPackage::egtKorean ){
		text.append( "(K)" );
		
	}else if( pConfig->GetGameType() == dmopmPackage::egtEnglish ){
		text.append( "(G)" );
		
	}else{
		text.append( "(?)" );
	}
	
	text.append( "DMO Pack Manager" );
	
	if( pPackage ){
		if( pPackage->GetSaved() ){
			text.append( " [" );
			text.append( FXPath::name( pPackage->GetFilename() ) );
			text.append( "]" );
			
		}else{
			text.append( " []" );
		}
	}
	
	setTitle( text );
}



// Events
///////////

long dmopmWindowMain::onClose( FXObject *sender, FXSelector selector, void *data ){
	if( QuitRequest() ){
		close();
		return 0;
	}
	
	return 1;
}

long dmopmWindowMain::onFileNew( FXObject *sender, FXSelector selector, void *data ){
	if( pPackage && pPackage->GetChanged() ){
		int result = FXMessageBox::question( this, MBOX_YES_NO_CANCEL, "New Packages",
			"The package has changed since the last time it has been saved. Do you want to save the package before creating a new one?" );
		
		if( result == MBOX_CLICKED_YES ){
			if( onFileSave( this, SEL_COMMAND, NULL ) == 0 ){
				return 1;
			}
			
		}else if( result == MBOX_CLICKED_CANCEL ){
			return 1;
		}
	}
	
	NewPackage();
	UpdateWindowTitle();
	return 1;
}

long dmopmWindowMain::onFileOpen( FXObject *sender, FXSelector selector, void *data ){
	if( pPackage && pPackage->GetChanged() ){
		int result = FXMessageBox::question( this, MBOX_YES_NO_CANCEL, "Open Packages",
			"The package has changed since the last time it has been saved. Do you want to save the package before opening a new one?" );
		
		if( result == MBOX_CLICKED_YES ){
			if( onFileSave( this, SEL_COMMAND, NULL ) == 0 ){
				return 1;
			}
			
		}else if( result == MBOX_CLICKED_CANCEL ){
			return 1;
		}
	}
	
	FXFileDialog dialog( this, "Open Package" );
	
	dialog.setPatternList( "DMO Package (*.dmop)\0.dmop" );
	dialog.setCurrentPattern( 0 );
	
	if( pPackage ){
		dialog.setFilename( pPackage->GetFilename() );
		
	}else{
		dialog.setFilename( "Package.dmop" );
	}
	
	if( dialog.execute() == TRUE ){
		dmopmPackage *package = NULL;
		dmopmLoadSavePackage lspackage;
		
		try{
			package = new dmopmPackage;
			if( ! package ) THROW( dueOutOfMemory );
			
			lspackage.LoadFromFile( dialog.getFilename(), *package );
			package->SetFilename( dialog.getFilename() );
			package->SetSaved( true );
			package->SetChanged( false );
			
			SetPackage( package );
			UpdateWindowTitle();
			
		}catch( duException e ){
			if( package ) delete package;
			DisplayException( e );
		}
	}
	
	return 1;
}

long dmopmWindowMain::onFileSave( FXObject *sender, FXSelector selector, void *data ){
	if( pPackage ){
		if( pPackage->GetSaved() ){
			SavePackage( pPackage->GetFilename() );
			UpdateWindowTitle();
			return true;
			
		}else{
			FXFileDialog dialog( this, "Save Package" );
			
			dialog.setPatternList( "DMO Package (*.dmop)\0.dmop" );
			dialog.setCurrentPattern( 0 );
			dialog.setFilename( pPackage->GetFilename() );
			if( dialog.execute() == TRUE ){
				try{
					SavePackage( dialog.getFilename() );
					UpdateWindowTitle();
					
				}catch( duException e ){
					DisplayException( e );
				}
			}
		}
	}
	
	return 1;
}

long dmopmWindowMain::onFileSaveAs( FXObject *sender, FXSelector selector, void *data ){
	if( pPackage ){
		FXFileDialog dialog( this, "Save Package As" );
		
		dialog.setPatternList( "DMO Package (*.dmop)\0.dmop" );
		dialog.setCurrentPattern( 0 );
		dialog.setFilename( pPackage->GetFilename() );
		if( dialog.execute() == TRUE ){
			try{
				SavePackage( dialog.getFilename() );
				UpdateWindowTitle();
				
			}catch( duException e ){
				DisplayException( e );
			}
		}
	}
	
	return 1;
}

long dmopmWindowMain::onFileSetDMOPath( FXObject *sender, FXSelector selector, void *data ){
	FXDirDialog dialog( this, "Select DMO Path" );
	
	dialog.setDirectory( pConfig->GetPathDMO() );
	
	if( dialog.execute( PLACEMENT_OWNER ) == TRUE ){
		pConfig->SetPathDMO( dialog.getDirectory() );
		DetermineGameType();
		UpdateWindowTitle();
	}
	
	return 1;
}

long dmopmWindowMain::onFileQuit( FXObject *sender, FXSelector selector, void *data ){
	if( QuitRequest() ){
		close( TRUE );
	}
	
	return 1;
}



long dmopmWindowMain::onPackProperties( FXObject *sender, FXSelector selector, void *data ){
	if( pPackage ){
		dmopmDialogEditPackage dialog( this, this );
		
		if( dialog.execute() == TRUE ){
			pPackage->SetChanged( true );
		}
	}
	
	return 1;
}

long dmopmWindowMain::onPackImportArchive( FXObject *sender, FXSelector selector, void *data ){
	if( pPackage ){
		dmopmArchiveFile archiveFile( pConfig, pPackage );
		dmopmPackageEntry *newEntry = NULL;
		dmopmPackageEntry *matchingEntry;
		dmopmPackage archiveEntries;
		int i, count;
		
		try{
			archiveFile.ReadFileMap( archiveEntries );
			
			count = archiveEntries.GetEntryCount();
			for( i=0; i<count; i++ ){
				const dmopmPackageEntry &entry = *archiveEntries.GetEntryAt( i );
				
				matchingEntry = pPackage->GetEntryWithID( entry.GetID() );
				
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
					pPackage->AddEntry( newEntry );
					newEntry = NULL;
				}
			}
			
		}catch( duException e ){
			if( newEntry ){
				delete newEntry;
			}
			DisplayException( e );
		}
		
		pPanelEntries->UpdatePackage();
	}
	
	return 1;
}

long dmopmWindowMain::onPackMapDirectory( FXObject *sender, FXSelector selector, void *data ){
	FXDirDialog dialog( this, "Select Directory to map files in" );
	
	dialog.setDirectory( pConfig->GetPathDMO() );
	
	if( dialog.execute( PLACEMENT_OWNER ) == TRUE ){
		dmopmDialogMapDirectory dialog2( this, this, dialog.getDirectory() );
		
		dialog2.execute( PLACEMENT_OWNER );
		
		pPanelEntries->UpdatePackage();
		
	}
	
	return 1;
}

long dmopmWindowMain::onPackUpdate( FXObject *sender, FXSelector selector, void *data ){
	if( pPackage ){
		dmopmDialogUpdate dialog( this, this );
		
		dialog.execute( PLACEMENT_OWNER );
		
		pPanelEntries->UpdatePackage();
	}
	
	return 1;
}

long dmopmWindowMain::onPackImportFiles( FXObject *sender, FXSelector selector, void *data ){
	if( pPackage ){
		FXString text;
		
		text = "! ! !  W A R N I N G ! ! !\n";
		text += "\n";
		text += "You are about to import files into the package files.\n";
		text += "This will overwrite the filemap and archive files of this package.\n";
		text += "\n";
		text += "Are you really sure you want to continue?";
		
		if( FXMessageBox::question( this, MBOX_YES_NO, "Import Files", "%s", text.text() ) == MBOX_CLICKED_YES ){
			dmopmDialogImportFiles dialog( this, this );
			
			dialog.execute( PLACEMENT_OWNER );
			
			pPanelEntries->UpdatePackage();
		}
	}
	
	return 1;
}

long dmopmWindowMain::onPackImportFromZip( FXObject *sender, FXSelector selector, void *data ){
	if( pPackage ){
		FXFileDialog dialog( this, "Select Import ZIP File" );
		
		dialog.setFilename( pPathImportZIP );
		
		if( dialog.execute( PLACEMENT_OWNER ) == TRUE ){
			pPathImportZIP = dialog.getFilename();
			
			FXString text;
			
			text = "! ! !  W A R N I N G ! ! !\n";
			text += "\n";
			text += "You are about to import files into the package files.\n";
			text += "This will overwrite the filemap and archive files of this package.\n";
			text += "\n";
			text += "Are you really sure you want to continue?";
			
			if( FXMessageBox::question( this, MBOX_YES_NO, "Import Files", "%s", text.text() ) == MBOX_CLICKED_YES ){
				dmopmDialogImportFiles dialog( this, this );
				
				dialog.SetPathImportZIP( pPathImportZIP );
				dialog.execute( PLACEMENT_OWNER );
				
				pPanelEntries->UpdatePackage();
			}
		}
	}
	
	return 1;
}



long dmopmWindowMain::onOptionsVerbose( FXObject *sender, FXSelector selector, void *data ){
	pConfig->SetVerbose( ( ( FXuval )data ) == TRUE );
	return 1;
}

long dmopmWindowMain::updateOptionsVerbose( FXObject *sender, FXSelector selector, void *data ){
	sender->handle( this, FXSEL( SEL_COMMAND, pConfig->GetVerbose() ? ID_CHECK : ID_UNCHECK ), NULL );
	return 1;
}
