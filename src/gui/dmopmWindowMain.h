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

#ifndef _DMOPMWINDOWMAIN_H_
#define _DMOPMWINDOWMAIN_H_

#include "../foxtoolkit.h"

class duException;
class dmopmPackage;
class dmopmPanelEntries;
class dmopmConfig;
class dmopmDownloader;



/**
 * @brief Main Window.
 * @author Plüss Roland
 * @version 1.0
 * @date 2011
 */
class dmopmWindowMain : public FXMainWindow{
	FXDECLARE( dmopmWindowMain )
	
protected:
	dmopmWindowMain();
	
public:
	enum eFoxIDs{
		ID_LAST = FXMainWindow::ID_LAST,
		ID_FILE_NEW,
		ID_FILE_OPEN,
		ID_FILE_SAVE,
		ID_FILE_SAVEAS,
		ID_FILE_SETDMOPATH,
		ID_FILE_QUIT,
		ID_PACK_PROPERTIES,
		ID_PACK_IMPORT_ARCHIVE,
		ID_PACK_MAP_DIRECTORY,
		ID_PACK_UPDATE,
		ID_PACK_IMPORT_FILES,
		ID_PACK_IMPORT_FROM_ZIP,
		ID_OPTIONS_VERBOSE,
	};
	
private:
	FXMenuBar *pMenuBar;
	
	FXStatusBar *pStatusBar;
	FXProgressBar *pSBProgress;
	
	FXToolTip *pToolTip;
	
	dmopmPanelEntries *pPanelEntries;
	
	dmopmConfig *pConfig;
	dmopmDownloader *pDownloader;
	dmopmPackage *pPackage;
	
	FXString pPathImportZIP;
	
public:
	/** @name Constructors and Destructors */
	/*@{*/
	/** Creates a new main window. */
	dmopmWindowMain( FXApp* app, int argc, char **argv );
	/** Cleans up the main window prototype. */
	virtual ~dmopmWindowMain();
	/** Fox request for creating the window. */
	virtual void create();
	/*@}*/
	
	/** @name Management */
	/*@{*/
	/** Displays an exception error. */
	void DisplayException( const duException &exception );
	
	/** Retrieves the menu bar. */
	inline FXMenuBar *GetMenuBar() const{ return pMenuBar; }
	
	/** Sets the visibility of the progress bar in the status bar. */
	void SetProgressVisible( bool visible );
	/** Sets the progress bar progress. */
	void SetProgress( float progress );
	/** Sets the progress text. */
	void SetProgressText( const FXString &text );
	
	/** Asks the user if it is okay to quit the application. */
	bool QuitRequest();
	
	/** Retrieves the configuration. */
	inline dmopmConfig *GetConfiguration() const{ return pConfig; }
	/** Retrieves the downloader. */
	inline dmopmDownloader *GetDownloader() const{ return pDownloader; }
	/** Retrieves the package. */
	inline dmopmPackage *GetPackage() const{ return pPackage; }
	/** Sets the package. */
	void SetPackage( dmopmPackage *package );
	
	/** Create new package. */
	void NewPackage();
	/** Save package. */
	void SavePackage( const FXString &filename );
	
	/** Determines game type of DMO in DMO Path. */
	void DetermineGameType();
	/** Updates the window title. */
	void UpdateWindowTitle();
	/*@}*/
	
	/** @name Events */
	/*@{*/
	long onClose( FXObject *sender, FXSelector selector, void *data );
	
	long onFileNew( FXObject *sender, FXSelector selector, void *data );
	long onFileOpen( FXObject *sender, FXSelector selector, void *data );
	long onFileSave( FXObject *sender, FXSelector selector, void *data );
	long onFileSaveAs( FXObject *sender, FXSelector selector, void *data );
	long onFileSetDMOPath( FXObject *sender, FXSelector selector, void *data );
	long onFileQuit( FXObject *sender, FXSelector selector, void *data );
	
	long onPackProperties( FXObject *sender, FXSelector selector, void *data );
	long onPackImportArchive( FXObject *sender, FXSelector selector, void *data );
	long onPackMapDirectory( FXObject *sender, FXSelector selector, void *data );
	long onPackUpdate( FXObject *sender, FXSelector selector, void *data );
	long onPackImportFiles( FXObject *sender, FXSelector selector, void *data );
	long onPackImportFromZip( FXObject *sender, FXSelector selector, void *data );
	
	long onOptionsVerbose( FXObject *sender, FXSelector selector, void *data );
	long updateOptionsVerbose( FXObject *sender, FXSelector selector, void *data );
	/*@}*/
};

#endif
