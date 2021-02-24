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

#ifndef _DMOPMPANELENTRIES_H_
#define _DMOPMPANELENTRIES_H_

#include "../foxtoolkit.h"

class dmopmWindowMain;
class dmopmBrowseTreeItem;



/**
 * @brief Entries Panel.
 * @author Plüss Roland
 * @version 1.0
 * @date 2011
 */
class dmopmPanelEntries : public FXVerticalFrame{
	FXDECLARE( dmopmPanelEntries )
protected:
	dmopmPanelEntries();
	
public:
	enum eFoxIDs{
		ID_CANVAS = FXVerticalFrame::ID_LAST,
		ID_LIST_ENTRIES,
		ID_LIST_ENTRIES_HEADER,
		ID_TREE_BROWSER_DIRS,
		ID_LIST_BROWSER_FILES,
		ID_LIST_BROWSER_FILES_HEADER,
		ID_PU_FILE_PROPERTIES,
		ID_PU_EXPORT_FILE,
		ID_PU_EXPORT_DIRECTORY,
		ID_PU_IMPORT_FILE,
		ID_PU_CLEAR_IMPORT_FILE,
		ID_LAST
	};
	
private:
	dmopmWindowMain *pWindowMain;
	
	dmopmBrowseTreeItem *pRootItem;
	
	FXTabBook *pTabPanels;
	FXIconList *pListEntries;
	FXTreeList *pTreeBrowserDirs;
	FXIconList *pListBrowserFiles;
	
	FXIcon *pIconFolderBig;
	FXIcon *pIconFolderSmall;
	FXIcon *pIconFolderOpenBig;
	FXIcon *pIconFolderOpenSmall;
	FXIcon *pIconDocBig;
	FXIcon *pIconDocSmall;
	FXFileDict *pFileDict;
	
	FXString pPathExport;
	FXString pPathImport;
	
public:
	/** @name Constructors and Destructors */
	/*@{*/
	/** Creates a new panel. */
	dmopmPanelEntries( dmopmWindowMain *windowMain, FXComposite *container );
	/** Cleans up the panel. */
	virtual ~dmopmPanelEntries();
	/*@}*/
	
	/** @name Management */
	/*@{*/
	void create();
	void detach();
	void destroy();
	
	/** Retrieves the main window. */
	inline dmopmWindowMain *GetWindowMain() const{ return pWindowMain; }
	
	/** Update the package. */
	void UpdatePackage();
	
	/** Update the entries list. */
	void UpdateListEntries();
	/** Update browser directory tree. */
	void UpdateBrowserDirTree();
	/** Update browser file listing. */
	void UpdateBrowserFileList();
	
	/** Retrieves the tree item for a given directory path creating items if required. */
	dmopmBrowseTreeItem *GetItemForPath( const FXString &path );
	/*@}*/
	
	/** @name Events */
	/*@{*/
	long onListEntriesRMDown( FXObject *sender, FXSelector selector, void *data );
	long onListEntriesRMUp( FXObject *sender, FXSelector selector, void *data );
	long onListEntriesHeaderClicked( FXObject *sender, FXSelector selector, void *data );
	long onTreeBrowserDirChanged( FXObject *sender, FXSelector selector, void *data );
	long onTreeBrowserDirRMDown( FXObject *sender, FXSelector selector, void *data );
	long onTreeBrowserDirRMUp( FXObject *sender, FXSelector selector, void *data );
	long onListBrowserFilesHeaderClicked( FXObject *sender, FXSelector selector, void *data );
	long onListBrowserRMDown( FXObject *sender, FXSelector selector, void *data );
	long onListBrowserRMUp( FXObject *sender, FXSelector selector, void *data );
	long onPUFileProperties( FXObject *sender, FXSelector selector, void *data );
	long onPUExportFile( FXObject *sender, FXSelector selector, void *data );
	long onPUExportDirectory( FXObject *sender, FXSelector selector, void *data );
	long onPUImportFile( FXObject *sender, FXSelector selector, void *data );
	long onPUClearImportFile( FXObject *sender, FXSelector selector, void *data );
	/*@}*/
};

#endif
