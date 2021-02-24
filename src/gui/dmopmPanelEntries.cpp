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
#include <stdint.h>

#include "fox_icons.h"
#include "dmopmWindowMain.h"
#include "dmopmPanelEntries.h"
#include "dmopmBrowseTreeItem.h"
#include "../package/dmopmPackage.h"
#include "../package/dmopmPackageEntry.h"
#include "../exceptions.h"
#include "../file/dmopmArchiveFile.h"
#include "dmopmDialogEditEntry.h"



// Events
///////////

FXDEFMAP( dmopmPanelEntries ) dmopmPanelEntriesMap[]={
	FXMAPFUNC( SEL_RIGHTBUTTONPRESS, dmopmPanelEntries::ID_LIST_ENTRIES, dmopmPanelEntries::onListEntriesRMDown ),
	FXMAPFUNC( SEL_RIGHTBUTTONRELEASE, dmopmPanelEntries::ID_LIST_ENTRIES, dmopmPanelEntries::onListEntriesRMUp ),
	FXMAPFUNC( SEL_COMMAND, dmopmPanelEntries::ID_LIST_ENTRIES_HEADER, dmopmPanelEntries::onListEntriesHeaderClicked ),
	FXMAPFUNC( SEL_COMMAND, dmopmPanelEntries::ID_TREE_BROWSER_DIRS, dmopmPanelEntries::onTreeBrowserDirChanged ),
	FXMAPFUNC( SEL_RIGHTBUTTONPRESS, dmopmPanelEntries::ID_TREE_BROWSER_DIRS, dmopmPanelEntries::onTreeBrowserDirRMDown ),
	FXMAPFUNC( SEL_RIGHTBUTTONRELEASE, dmopmPanelEntries::ID_TREE_BROWSER_DIRS, dmopmPanelEntries::onTreeBrowserDirRMUp ),
	FXMAPFUNC( SEL_COMMAND, dmopmPanelEntries::ID_LIST_BROWSER_FILES_HEADER, dmopmPanelEntries::onListBrowserFilesHeaderClicked ),
	FXMAPFUNC( SEL_RIGHTBUTTONPRESS, dmopmPanelEntries::ID_LIST_BROWSER_FILES, dmopmPanelEntries::onListBrowserRMDown ),
	FXMAPFUNC( SEL_RIGHTBUTTONRELEASE, dmopmPanelEntries::ID_LIST_BROWSER_FILES, dmopmPanelEntries::onListBrowserRMUp ),
	FXMAPFUNC( SEL_COMMAND, dmopmPanelEntries::ID_PU_FILE_PROPERTIES, dmopmPanelEntries::onPUFileProperties ),
	FXMAPFUNC( SEL_COMMAND, dmopmPanelEntries::ID_PU_EXPORT_FILE, dmopmPanelEntries::onPUExportFile ),
	FXMAPFUNC( SEL_COMMAND, dmopmPanelEntries::ID_PU_EXPORT_DIRECTORY, dmopmPanelEntries::onPUExportDirectory ),
	FXMAPFUNC( SEL_COMMAND, dmopmPanelEntries::ID_PU_IMPORT_FILE, dmopmPanelEntries::onPUImportFile ),
	FXMAPFUNC( SEL_COMMAND, dmopmPanelEntries::ID_PU_CLEAR_IMPORT_FILE, dmopmPanelEntries::onPUClearImportFile ),
};



// Functors
/////////////

struct fEqualString{
	const FXString &pString;
	
	fEqualString( const FXString &string ) : pString( string ){
	}
	
	bool operator()( FXString &string ){
		return pString == string;
	}
};



// Sorting
////////////

FXint fSortTreeItems( const FXTreeItem *item1, const FXTreeItem *item2 ){
	const dmopmBrowseTreeItem &bitem1 = *( ( dmopmBrowseTreeItem* )item1 );
	const dmopmBrowseTreeItem &bitem2 = *( ( dmopmBrowseTreeItem* )item2 );
	
	return comparecase( bitem1.GetPath(), bitem2.GetPath() );
}

FXint fSortListEntryByCol1Asc( const FXIconItem *item1, const FXIconItem *item2 ){
	return FXIntVal( item1->getText().section( '\t', 0, 1 ) ) > FXIntVal( item2->getText().section( '\t', 0, 1 ) );
}

FXint fSortListEntryByCol1Desc( const FXIconItem *item1, const FXIconItem *item2 ){
	return FXIntVal( item1->getText().section( '\t', 0, 1 ) ) < FXIntVal( item2->getText().section( '\t', 0, 1 ) );
}

FXint fSortListEntryByNameAsc( const FXIconItem *item1, const FXIconItem *item2 ){
	const dmopmPackageEntry &entry1 = *( ( dmopmPackageEntry* )item1->getData() );
	const dmopmPackageEntry &entry2 = *( ( dmopmPackageEntry* )item2->getData() );
	
	if( entry1.GetPath().length() == 0 ){
		return entry1.GetID() > entry2.GetID();
		
	}else{
		return comparecase( entry1.GetPathName().text(), entry2.GetPathName().text() );
	}
}

FXint fSortListEntryByNameDesc( const FXIconItem *item1, const FXIconItem *item2 ){
	const dmopmPackageEntry &entry1 = *( ( dmopmPackageEntry* )item1->getData() );
	const dmopmPackageEntry &entry2 = *( ( dmopmPackageEntry* )item2->getData() );
	
	if( entry1.GetPath().length() == 0 ){
		return entry1.GetID() < entry2.GetID();
		
	}else{
		return -comparecase( entry1.GetPathName().text(), entry2.GetPathName().text() );
	}
}

FXint fSortListEntryByIDAsc( const FXIconItem *item1, const FXIconItem *item2 ){
	const dmopmPackageEntry &entry1 = *( ( dmopmPackageEntry* )item1->getData() );
	const dmopmPackageEntry &entry2 = *( ( dmopmPackageEntry* )item2->getData() );
	
	return entry1.GetID() > entry2.GetID();
}

FXint fSortListEntryByIDDesc( const FXIconItem *item1, const FXIconItem *item2 ){
	const dmopmPackageEntry &entry1 = *( ( dmopmPackageEntry* )item1->getData() );
	const dmopmPackageEntry &entry2 = *( ( dmopmPackageEntry* )item2->getData() );
	
	return entry1.GetID() < entry2.GetID();
}

FXint fSortListEntryBySizeAsc( const FXIconItem *item1, const FXIconItem *item2 ){
	const dmopmPackageEntry &entry1 = *( ( dmopmPackageEntry* )item1->getData() );
	const dmopmPackageEntry &entry2 = *( ( dmopmPackageEntry* )item2->getData() );
	
	return entry1.GetSize() > entry2.GetSize();
}

FXint fSortListEntryBySizeDesc( const FXIconItem *item1, const FXIconItem *item2 ){
	const dmopmPackageEntry &entry1 = *( ( dmopmPackageEntry* )item1->getData() );
	const dmopmPackageEntry &entry2 = *( ( dmopmPackageEntry* )item2->getData() );
	
	return entry1.GetSize() < entry2.GetSize();
}

FXint fSortListEntryByOffsetAsc( const FXIconItem *item1, const FXIconItem *item2 ){
	const dmopmPackageEntry &entry1 = *( ( dmopmPackageEntry* )item1->getData() );
	const dmopmPackageEntry &entry2 = *( ( dmopmPackageEntry* )item2->getData() );
	
	return entry1.GetOffset() > entry2.GetOffset();
}

FXint fSortListEntryByOffsetDesc( const FXIconItem *item1, const FXIconItem *item2 ){
	const dmopmPackageEntry &entry1 = *( ( dmopmPackageEntry* )item1->getData() );
	const dmopmPackageEntry &entry2 = *( ( dmopmPackageEntry* )item2->getData() );
	
	return entry1.GetOffset() < entry2.GetOffset();
}

FXint fSortListEntryByPathAsc( const FXIconItem *item1, const FXIconItem *item2 ){
	const dmopmPackageEntry &entry1 = *( ( dmopmPackageEntry* )item1->getData() );
	const dmopmPackageEntry &entry2 = *( ( dmopmPackageEntry* )item2->getData() );
	
	return comparecase( entry1.GetPath(), entry2.GetPath() );
}

FXint fSortListEntryByPathDesc( const FXIconItem *item1, const FXIconItem *item2 ){
	const dmopmPackageEntry &entry1 = *( ( dmopmPackageEntry* )item1->getData() );
	const dmopmPackageEntry &entry2 = *( ( dmopmPackageEntry* )item2->getData() );
	
	return -comparecase( entry1.GetPath(), entry2.GetPath() );
}



// Class dmopmPanelEntries
////////////////////////////

FXIMPLEMENT( dmopmPanelEntries, FXVerticalFrame, dmopmPanelEntriesMap, ARRAYNUMBER( dmopmPanelEntriesMap ) )

// Constructor, destructor
////////////////////////////

dmopmPanelEntries::dmopmPanelEntries(){ }

dmopmPanelEntries::dmopmPanelEntries( dmopmWindowMain *windowMain, FXComposite *container ) :
FXVerticalFrame( container, LAYOUT_FILL_X | LAYOUT_FILL_Y | LAYOUT_SIDE_LEFT, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5 ){
	FXVerticalFrame *borderFrame;
	FXSplitter *splitter;
	
	pWindowMain = windowMain;
	
	pIconFolderBig = new FXGIFIcon( getApp(), bigfolder );
	pIconFolderSmall = new FXGIFIcon( getApp(), minifolder );
	pIconFolderOpenBig = new FXGIFIcon( getApp(), bigfolderopen );
	pIconFolderOpenSmall = new FXGIFIcon( getApp(), minifolderopen );
	pIconDocBig = new FXGIFIcon( getApp(), bigdoc );
	pIconDocSmall = new FXGIFIcon( getApp(), minidoc );
	pFileDict = new FXFileDict( getApp() );
	
	pTabPanels = new FXTabBook( this, NULL, 0, LAYOUT_FILL_X | LAYOUT_FILL_Y | TABBOOK_NORMAL );
	
	// package entry listing
	new FXTabItem( pTabPanels, "Package Entry Listing" );
	
	borderFrame = new FXVerticalFrame( pTabPanels, FRAME_RAISED | LAYOUT_FILL_X | LAYOUT_FILL_Y );
	borderFrame = new FXVerticalFrame( borderFrame, FRAME_SUNKEN | LAYOUT_FILL_X | LAYOUT_FILL_Y, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 );
	pListEntries = new FXIconList( borderFrame, this, ID_LIST_ENTRIES, LAYOUT_FILL_X | LAYOUT_FILL_Y | ICONLIST_EXTENDEDSELECT | ICONLIST_DETAILED );
	
	pListEntries->appendHeader( "Num", NULL, 60 );
	pListEntries->appendHeader( "ID", NULL, 90 );
	pListEntries->appendHeader( "Offset", NULL, 90 );
	pListEntries->appendHeader( "Size", NULL, 90 );
	pListEntries->appendHeader( "Path", NULL, 430 );
	pListEntries->setSortFunc( fSortListEntryByCol1Asc );
	pListEntries->getHeader()->setTarget( this );
	pListEntries->getHeader()->setSelector( ID_LIST_ENTRIES_HEADER );
	
	// browse content
	new FXTabItem( pTabPanels, "Browse Content" );
	
	borderFrame = new FXVerticalFrame( pTabPanels, FRAME_RAISED | LAYOUT_FILL_X | LAYOUT_FILL_Y );
	splitter = new FXSplitter( borderFrame, SPLITTER_HORIZONTAL | SPLITTER_TRACKING | LAYOUT_FILL_X | LAYOUT_FILL_Y );
	
	borderFrame = new FXVerticalFrame( splitter, FRAME_SUNKEN | LAYOUT_FILL_X | LAYOUT_FILL_Y, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 );
	pTreeBrowserDirs = new FXTreeList( borderFrame, this, ID_TREE_BROWSER_DIRS, LAYOUT_FILL_X
		| LAYOUT_FILL_Y | TREELIST_BROWSESELECT | TREELIST_SHOWS_LINES | TREELIST_SHOWS_BOXES | TREELIST_ROOT_BOXES );
	pTreeBrowserDirs->setSortFunc( fSortTreeItems );
	
	borderFrame = new FXVerticalFrame( splitter, FRAME_SUNKEN | LAYOUT_FILL_X | LAYOUT_FILL_Y, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 );
	pListBrowserFiles = new FXIconList( borderFrame, this, ID_LIST_BROWSER_FILES, LAYOUT_FILL_X
		| LAYOUT_FILL_Y | ICONLIST_EXTENDEDSELECT | ICONLIST_DETAILED );
	pListBrowserFiles->appendHeader( "Filename", NULL, 250 );
	pListBrowserFiles->appendHeader( "Size", NULL, 80 );
	pListBrowserFiles->appendHeader( "ID", NULL, 90 );
	pListBrowserFiles->appendHeader( "Offset", NULL, 80 );
	pListBrowserFiles->setSortFunc( fSortListEntryByNameAsc );
	pListBrowserFiles->getHeader()->setTarget( this );
	pListBrowserFiles->getHeader()->setSelector( ID_LIST_BROWSER_FILES_HEADER );
	
	splitter->setSplit( 0, 250 );
}

dmopmPanelEntries::~dmopmPanelEntries(){
	delete pFileDict;
	delete pIconFolderOpenBig;
	delete pIconFolderOpenSmall;
	delete pIconFolderBig;
	delete pIconFolderSmall;
	delete pIconDocBig;
	delete pIconDocSmall;
}



// Management
///////////////

void dmopmPanelEntries::create(){
	FXComposite::create();
	
	pIconFolderBig->create();
	pIconFolderSmall->create();
	pIconFolderOpenBig->create();
	pIconFolderOpenSmall->create();
	pIconDocBig->create();
	pIconDocSmall->create();
}

void dmopmPanelEntries::detach(){
	pIconFolderOpenBig->detach();
	pIconFolderOpenSmall->detach();
	pIconFolderBig->detach();
	pIconFolderSmall->detach();
	pIconDocBig->detach();
	pIconDocSmall->detach();
	
	FXComposite::detach();
}

void dmopmPanelEntries::destroy(){
	pIconFolderOpenBig->destroy();
	pIconFolderOpenSmall->destroy();
	pIconFolderBig->destroy();
	pIconFolderSmall->destroy();
	pIconDocBig->destroy();
	pIconDocSmall->destroy();
	
	FXComposite::destroy();
}



void dmopmPanelEntries::UpdatePackage(){
	UpdateListEntries();
	UpdateBrowserDirTree();
}

void dmopmPanelEntries::UpdateListEntries(){
	pListEntries->clearItems();
	
	if( pWindowMain->GetPackage() ){
		const dmopmPackage &package = *pWindowMain->GetPackage();
		const int count = package.GetEntryCount();
		dmopmPackageEntry *entry;
		FXString text;
		int i;
		
		for( i=0; i<count; i++ ){
			entry = package.GetEntryAt( i );
			
			text.format( "%i%s\t%u\t%u\t%u\t%s", i, entry->GetImportPath().empty()?"":" (*)", entry->GetID(),
				entry->GetOffset(), entry->GetSize(), entry->GetPath().text() );
			pListEntries->appendItem( text, NULL, NULL, entry );
		}
	}
	
	pListEntries->sortItems();
}

void dmopmPanelEntries::UpdateBrowserDirTree(){
	/*
	std::vector<FXString>::const_iterator iter;
	std::vector<FXString> expandedEntries;
	dmopmBrowseTreeItem *treeItem;
	int i, count;
	
	count = pTreeBrowserDirs->getNumItems();
	for( i=0; i<count; i++ ){
		treeItem = ( dmopmBrowseTreeItem* )
		
		if( pTreeBrowserDirs->isItemExpanded( treeItem ) ){
			const dmopmBrowseTreeItem &item = *( ( dmopmBrowseTreeItem* )pTreeBrowserDirs->getItemData( i ) );
			
			expandedEntries.push_back( item.GetPath() );
		}
	}
	*/
	
	pRootItem = NULL;
	pTreeBrowserDirs->clearItems();
	
	pRootItem = new dmopmBrowseTreeItem( "", "Content", pIconFolderOpenSmall, pIconFolderSmall );
	pTreeBrowserDirs->appendItem( NULL, pRootItem );
	pTreeBrowserDirs->appendItem( NULL, new dmopmBrowseTreeItem( "Unknown", pIconFolderOpenSmall, pIconFolderSmall) );
	
	if( pWindowMain->GetPackage() ){
		const dmopmPackage &package = *pWindowMain->GetPackage();
		const int count = package.GetEntryCount();
		int i;
		
		for( i=0; i<count; i++ ){
			const dmopmPackageEntry &entry = *package.GetEntryAt( i );
			
			if( entry.GetPath().length() > 0 && entry.GetPathDir().length() > 0 ){
				GetItemForPath( entry.GetPathDir() );
			}
		}
	}
	pTreeBrowserDirs->expandTree( pRootItem );
	/*
	count = pTreeBrowserDirs->getNumItems();
	for( i=0; i<count; i++ ){
		const dmopmBrowseTreeItem &item = *( ( dmopmBrowseTreeItem* )pTreeBrowserDirs->getItemData( i ) );
		iter = std::find_if( expandedEntries.begin(), expandedEntries.end(), fEqualString( item.GetPath() ) );
		
		if( iter == expandedEntries.end() ){
			pTreeBrowserDirs->expandTree( treeItem );
			
		}else{
			pTreeBrowserDirs->collapseTree( treeItem );
		}
	}
	*/
	UpdateBrowserFileList();
}

void dmopmPanelEntries::UpdateBrowserFileList(){
	dmopmBrowseTreeItem *curItem = ( dmopmBrowseTreeItem* )pTreeBrowserDirs->getCurrentItem();
	
	pListBrowserFiles->clearItems();
	
	if( curItem && pWindowMain->GetPackage() ){
		const dmopmPackage &package = *pWindowMain->GetPackage();
		const int count = package.GetEntryCount();
		dmopmPackageEntry *entry;
		FXFileAssoc *fileAssoc;
		FXString text;
		int i;
		
		if( curItem->GetUnknown() ){
			for( i=0; i<count; i++ ){
				entry = package.GetEntryAt( i );
				
				if( entry->GetPath().length() == 0 ){
					text.format( "file_%u.dat%s\t%u\t%u\t%u", entry->GetID(), entry->GetImportPath().empty()?"":" (*)",
						entry->GetSize(), entry->GetID(), entry->GetOffset() );
					pListBrowserFiles->appendItem( text, pIconDocBig, pIconDocSmall, entry );
				}
			}
			
		}else{
			const FXString &pathDir = curItem->GetPath();
			
			for( i=0; i<count; i++ ){
				entry = package.GetEntryAt( i );
				
				if( entry->GetPath().length() > 0 && comparecase( entry->GetPathDir(), pathDir ) == 0 ){
					fileAssoc = pFileDict->findFileBinding( entry->GetPathName().text() );
					
					text.format( "%s%s\t%u\t%u\t%u", entry->GetPathName().text(), entry->GetImportPath().empty()?"":" (*)",
						entry->GetSize(), entry->GetID(), entry->GetOffset() );
					
					if( fileAssoc ){
						pListBrowserFiles->appendItem( text, fileAssoc->bigicon, fileAssoc->miniicon, entry );
						
					}else{
						pListBrowserFiles->appendItem( text, pIconDocBig, pIconDocSmall, entry );
					}
				}
			}
		}
	}
	
	pListBrowserFiles->sortItems();
}



dmopmBrowseTreeItem *dmopmPanelEntries::GetItemForPath( const FXString &path ){
	const FXString directory = FXPath::directory( path );
	const FXString name = FXPath::name( path );
	dmopmBrowseTreeItem *parentItem = NULL;
	dmopmBrowseTreeItem *item = NULL;
	
	if( directory.empty() || FXPath::isTopDirectory( directory ) ){
		parentItem = pRootItem;
		
	}else{
		parentItem = GetItemForPath( directory );
	}
	
	item = ( dmopmBrowseTreeItem* )parentItem->getFirst();
	while( item && comparecase( item->GetName(), name ) != 0 ){
		item = ( dmopmBrowseTreeItem* )item->getNext();
	}
	
	if( ! item ){
		item = new dmopmBrowseTreeItem( path, FXPath::name( path ), pIconFolderOpenSmall, pIconFolderSmall );
		pTreeBrowserDirs->appendItem( parentItem, item );
		pTreeBrowserDirs->sortChildItems( parentItem );
	}
	
	return item;
}



// Events
///////////

long dmopmPanelEntries::onListEntriesHeaderClicked( FXObject *sender, FXSelector selector, void *data ){
	const int index = ( intptr_t )data;
	
	if( index == 0 ){ // sort by number
		if( pListEntries->getSortFunc() == fSortListEntryByCol1Asc ){
			pListEntries->setSortFunc( fSortListEntryByCol1Desc );
			
		}else{
			pListEntries->setSortFunc( fSortListEntryByCol1Asc );
		}
		
	}else if( index == 1 ){ // sort by ID
		if( pListEntries->getSortFunc() == fSortListEntryByIDAsc ){
			pListEntries->setSortFunc( fSortListEntryByIDDesc );
			
		}else{
			pListEntries->setSortFunc( fSortListEntryByIDAsc );
		}
		
	}else if( index == 2 ){ // sort by offset
		if( pListEntries->getSortFunc() == fSortListEntryByOffsetAsc ){
			pListEntries->setSortFunc( fSortListEntryByOffsetDesc );
			
		}else{
			pListEntries->setSortFunc( fSortListEntryByOffsetAsc );
		}
		
	}else if( index == 3 ){ // sort by size
		if( pListEntries->getSortFunc() == fSortListEntryBySizeAsc ){
			pListEntries->setSortFunc( fSortListEntryBySizeDesc );
			
		}else{
			pListEntries->setSortFunc( fSortListEntryBySizeAsc );
		}
		
	}else{ // sort by path
		if( pListEntries->getSortFunc() == fSortListEntryByPathAsc ){
			pListEntries->setSortFunc( fSortListEntryByPathDesc );
			
		}else{
			pListEntries->setSortFunc( fSortListEntryByPathAsc );
		}
	}
	
	pListEntries->sortItems();
	
	return 1;
}

long dmopmPanelEntries::onListEntriesRMDown( FXObject *sender, FXSelector selector, void *data ){
	if( pWindowMain->GetPackage() ){
		const FXEvent &event = *( ( FXEvent* )data );
		int selectionCount = 0;
		int i, count;
		
		count = pListEntries->getNumItems();
		for( i=0; i<count; i++ ){
			if( pListEntries->isItemSelected( i ) ){
				selectionCount++;
			}
		}
		if( selectionCount == 0 ){
			int selection = pListEntries->getItemAt( event.win_x, event.win_y );
			
			if( selection != -1 ){
				pListEntries->setCurrentItem( selection );
				pListEntries->selectItem( selection );
				selectionCount = 1;
			}
		}
		
		if( selectionCount > 0 ){
			FXMenuPane *popup = NULL;
			FXMenuCommand *command;
			
			popup = new FXMenuPane( this );
			
			command = new FXMenuCommand( popup, "Edit...", NULL, this, ID_PU_FILE_PROPERTIES );
			if( selectionCount > 1 ){
				command->disable();
			}
			
			new FXMenuCommand( popup, "Export...", NULL, this, ID_PU_EXPORT_FILE );
			
			command = new FXMenuCommand( popup, "Import...", NULL, this, ID_PU_IMPORT_FILE );
			if( selectionCount > 1 ){
				command->disable();
			}
			
			new FXMenuCommand( popup, "Clear Import...", NULL, this, ID_PU_CLEAR_IMPORT_FILE );
			
			popup->create();
			
			popup->popup( NULL, event.root_x, event.root_y );
			getApp()->runModalWhileShown( popup );
			
			delete popup;
		}
	}
	
	return 1;
}

long dmopmPanelEntries::onListEntriesRMUp( FXObject *sender, FXSelector selector, void *data ){
	return 1;
}

long dmopmPanelEntries::onTreeBrowserDirChanged( FXObject *sender, FXSelector selector, void *data ){
	UpdateBrowserFileList();
	return 1;
}

long dmopmPanelEntries::onTreeBrowserDirRMDown( FXObject *sender, FXSelector selector, void *data ){
	if( pWindowMain->GetPackage() ){
		const FXEvent &event = *( ( FXEvent* )data );
		dmopmBrowseTreeItem *item = ( dmopmBrowseTreeItem* )pTreeBrowserDirs->getItemAt( event.win_x, event.win_y );
		
		if( item ){
			pTreeBrowserDirs->makeItemVisible( item );
			pTreeBrowserDirs->setCurrentItem( item );
			UpdateBrowserFileList();
			
			FXMenuPane *popup = NULL;
			
			popup = new FXMenuPane( this );
			
			new FXMenuCommand( popup, "Export...", NULL, this, ID_PU_EXPORT_DIRECTORY );
			
			popup->create();
			
			popup->popup( NULL, event.root_x, event.root_y );
			getApp()->runModalWhileShown( popup );
			
			delete popup;
		}
	}
	
	return 1;
}

long dmopmPanelEntries::onTreeBrowserDirRMUp( FXObject *sender, FXSelector selector, void *data ){
	return 1;
}

long dmopmPanelEntries::onListBrowserFilesHeaderClicked( FXObject *sender, FXSelector selector, void *data ){
	const int index = ( intptr_t )data;
	
	if( index == 0 ){ // sort by name
		if( pListBrowserFiles->getSortFunc() == fSortListEntryByNameAsc ){
			pListBrowserFiles->setSortFunc( fSortListEntryByNameDesc );
			
		}else{
			pListBrowserFiles->setSortFunc( fSortListEntryByNameAsc );
		}
		
	}else if( index == 1 ){ // sort by size
		if( pListBrowserFiles->getSortFunc() == fSortListEntryBySizeAsc ){
			pListBrowserFiles->setSortFunc( fSortListEntryBySizeDesc );
			
		}else{
			pListBrowserFiles->setSortFunc( fSortListEntryBySizeAsc );
		}
		
	}else if( index == 2 ){ // sort by ID
		if( pListBrowserFiles->getSortFunc() == fSortListEntryByIDAsc ){
			pListBrowserFiles->setSortFunc( fSortListEntryByIDDesc );
			
		}else{
			pListBrowserFiles->setSortFunc( fSortListEntryByIDAsc );
		}
		
	}else{ // sort by offset
		if( pListBrowserFiles->getSortFunc() == fSortListEntryByOffsetAsc ){
			pListBrowserFiles->setSortFunc( fSortListEntryByOffsetDesc );
			
		}else{
			pListBrowserFiles->setSortFunc( fSortListEntryByOffsetAsc );
		}
	}
	
	pListBrowserFiles->sortItems();
	
	return 1;
}

long dmopmPanelEntries::onListBrowserRMDown( FXObject *sender, FXSelector selector, void *data ){
	if( pWindowMain->GetPackage() ){
		const FXEvent &event = *( ( FXEvent* )data );
		int selectionCount = 0;
		int i, count;
		
		count = pListBrowserFiles->getNumItems();
		for( i=0; i<count; i++ ){
			if( pListBrowserFiles->isItemSelected( i ) ){
				selectionCount++;
			}
		}
		if( selectionCount == 0 ){
			int selection = pListBrowserFiles->getItemAt( event.win_x, event.win_y );
			
			if( selection != -1 ){
				pListBrowserFiles->setCurrentItem( selection );
				pListBrowserFiles->selectItem( selection );
				selectionCount = 1;
			}
		}
		
		if( selectionCount > 0 ){
			FXMenuPane *popup = NULL;
			FXMenuCommand *command;
			
			popup = new FXMenuPane( this );
			
			command = new FXMenuCommand( popup, "Edit...", NULL, this, ID_PU_FILE_PROPERTIES );
			if( selectionCount > 1 ){
				command->disable();
			}
			
			new FXMenuCommand( popup, "Export...", NULL, this, ID_PU_EXPORT_FILE );
			
			command = new FXMenuCommand( popup, "Import...", NULL, this, ID_PU_IMPORT_FILE );
			if( selectionCount > 1 ){
				command->disable();
			}
			
			new FXMenuCommand( popup, "Clear Import...", NULL, this, ID_PU_CLEAR_IMPORT_FILE );
			
			popup->create();
			
			popup->popup( NULL, event.root_x, event.root_y );
			getApp()->runModalWhileShown( popup );
			
			delete popup;
		}
	}
	
	return 1;
}

long dmopmPanelEntries::onListBrowserRMUp( FXObject *sender, FXSelector selector, void *data ){
	return 1;
}

long dmopmPanelEntries::onPUFileProperties( FXObject *sender, FXSelector selector, void *data ){
	dmopmPackageEntry *entry = NULL;
		int i, count;
	
	if( pTabPanels->getCurrent() == 0 ){
		count = pListEntries->getNumItems();
		for( i=0; i<count; i++ ){
			if( pListEntries->isItemSelected( i ) ){
				if( entry ){
					entry = NULL;
					break;
				}
				entry = ( dmopmPackageEntry* )pListEntries->getItemData( i );
			}
		}
		
	}else{
		count = pListBrowserFiles->getNumItems();
		for( i=0; i<count; i++ ){
			if( pListBrowserFiles->isItemSelected( i ) ){
				if( entry ){
					entry = NULL;
					break;
				}
				entry = ( dmopmPackageEntry* )pListBrowserFiles->getItemData( i );
			}
		}
	}
	
	if( pWindowMain->GetPackage() && entry ){
		dmopmDialogEditEntry dialog( this, pWindowMain, entry );
		
		if( dialog.execute() == TRUE ){
			pWindowMain->GetPackage()->SetChanged( true );
			UpdatePackage();
		}
	}
	
	return 1;
}

long dmopmPanelEntries::onPUExportFile( FXObject *sender, FXSelector selector, void *data ){
	if( pWindowMain->GetPackage() ){
		FXDirDialog dialog( this, "Select Export Directory" );
		
		dialog.setDirectory( pPathExport );
		
		if( dialog.execute( PLACEMENT_OWNER ) == TRUE ){
			dmopmArchiveFile archive( pWindowMain->GetConfiguration(), pWindowMain->GetPackage() );
			FXFileStream *stream = NULL;
			int exportedFileCount = 0;
			FXString entryPath;
			FXString filename;
			FXString data;
			int i, count;
			
			pPathExport = dialog.getDirectory();
			
			if( pTabPanels->getCurrent() == 0 ){
				count = pListEntries->getNumItems();
				for( i=0; i<count; i++ ){
					if( pListEntries->isItemSelected( i ) ){
						const dmopmPackageEntry &entry = *( ( dmopmPackageEntry* )pListEntries->getItemData( i ) );
						
						entryPath = entry.GetPath();
						
						if( entryPath.empty() ){
							entryPath.format( "file_%u.dat", entry.GetID() );
							filename = FXPath::absolute( pPathExport, entryPath );
							
						}else{
							filename = FXPath::absolute( pPathExport, entry.GetPath() );
						}
						
						try{
							archive.ReadFileData( entry, data );
							
							archive.MkDir( FXPath::directory( filename ) );
							
							printf( "writing entry %s to file '%s'.\n", entryPath.text(), filename.text() );
							stream = new FXFileStream;
							if( ! stream ) THROW( dueOutOfMemory );
							stream->open( filename, FXStreamSave );
							
							stream->save( data.text(), data.length());
							
							delete stream;
							
						}catch( duException e ){
							if( stream ){
								delete stream;
							}
							
							pWindowMain->DisplayException( e );
							return 1;
						}
						
						exportedFileCount++;
					}
				}
				
			}else{
				count = pListBrowserFiles->getNumItems();
				for( i=0; i<count; i++ ){
					if( pListBrowserFiles->isItemSelected( i ) ){
						const dmopmPackageEntry &entry = *( ( dmopmPackageEntry* )pListBrowserFiles->getItemData( i ) );
						
						entryPath = entry.GetPath();
						
						if( entryPath.empty() ){
							entryPath.format( "file_%u.dat", entry.GetID() );
							filename = FXPath::absolute( pPathExport, entryPath );
							
						}else{
							filename = FXPath::absolute( pPathExport, entry.GetPathName() );
						}
						
						try{
							archive.ReadFileData( entry, data );
							
							printf( "writing entry %s to file '%s'.\n", entryPath.text(), filename.text() );
							stream = new FXFileStream;
							if( ! stream ) THROW( dueOutOfMemory );
							stream->open( filename, FXStreamSave );
							
							stream->save( data.text(), data.length());
							
							delete stream;
							
						}catch( duException e ){
							if( stream ){
								delete stream;
							}
							
							pWindowMain->DisplayException( e );
							return 1;
						}
						
						exportedFileCount++;
					}
				}
			}
			
			FXMessageBox::information( this, MBOX_OK, "Export Files", "Exported %i files successfully.", exportedFileCount );
		}
	}
	
	return 1;
}

long dmopmPanelEntries::onPUExportDirectory( FXObject *sender, FXSelector selector, void *data ){
	dmopmBrowseTreeItem *item = ( dmopmBrowseTreeItem* )pTreeBrowserDirs->getCurrentItem();
	
	if( pWindowMain->GetPackage() && item ){
		FXDirDialog dialog( this, "Select Export Directory" );
		
		dialog.setDirectory( pPathExport );
		
		if( dialog.execute( PLACEMENT_OWNER ) == TRUE ){
			dmopmArchiveFile archive( pWindowMain->GetConfiguration(), pWindowMain->GetPackage() );
			const dmopmPackage &package = *pWindowMain->GetPackage();
			const int count = package.GetEntryCount();
			FXFileStream *stream = NULL;
			int exportedFileCount = 0;
			FXString requiredDir;
			FXString entryPath;
			FXString filename;
			FXString data;
			int i;
			
			pPathExport = dialog.getDirectory();
			
			if( item->GetUnknown() ){
				for( i=0; i<count; i++ ){
					const dmopmPackageEntry &entry = *package.GetEntryAt( i );
					
					if( entry.GetPath().length() == 0 ){
						entryPath.format( "file_%u.dat", entry.GetID() );
						filename = FXPath::absolute( pPathExport, entryPath );
						
						try{
							archive.ReadFileData( entry, data );
							
							printf( "writing entry %s to file '%s'.\n", entryPath.text(), filename.text() );
							stream = new FXFileStream;
							if( ! stream ) THROW( dueOutOfMemory );
							stream->open( filename, FXStreamSave );
							
							stream->save( data.text(), data.length());
							
							delete stream;
							
						}catch( duException e ){
							if( stream ){
								delete stream;
							}
							
							pWindowMain->DisplayException( e );
							return 1;
						}
						
						exportedFileCount++;
					}
				}
				
			}else{
				const FXString &pathDir = item->GetPath();
				const FXString exportDir = FXPath::absolute( pPathExport, item->GetName() );
				FXString testPath;
				bool matches;
				
				for( i=0; i<count; i++ ){
					const dmopmPackageEntry &entry = *package.GetEntryAt( i );
					
					matches = false;
					
					if( ! entry.GetPath().empty() ){
						testPath = entry.GetPathDir();
						while( testPath.length() >= pathDir.length() ){
							if( comparecase( testPath, pathDir ) == 0 ){
								matches = true;
								break;
							}
							testPath = FXPath::directory( testPath );
						}
					}
					
					if( matches ){
						entryPath = entry.GetPath();
						requiredDir = FXPath::absolute( exportDir, FXPath::relative( pathDir, entry.GetPathDir() ) );
						filename = FXPath::absolute( requiredDir, entry.GetPathName() );
						
						try{
							archive.ReadFileData( entry, data );
							
							archive.MkDir( requiredDir );
							
							printf( "writing entry %s to file '%s'.\n", entryPath.text(), filename.text() );
							stream = new FXFileStream;
							if( ! stream ) THROW( dueOutOfMemory );
							
							if( ! stream->open( filename, FXStreamSave ) ){
								THROW( dueWriteFile );
							}
							
							stream->save( data.text(), data.length());
							
							delete stream;
							
						}catch( duException e ){
							if( stream ){
								delete stream;
							}
							
							pWindowMain->DisplayException( e );
							return 1;
						}
						
						exportedFileCount++;
					}
				}
			}
			
			FXMessageBox::information( this, MBOX_OK, "Export Files", "Exported %i files successfully.", exportedFileCount );
		}
	}
	
	return 1;
}

long dmopmPanelEntries::onPUImportFile( FXObject *sender, FXSelector selector, void *data ){
	dmopmPackageEntry *entry = NULL;
	
	if( pWindowMain->GetPackage() ){
		int i, count;
		
		if( pTabPanels->getCurrent() == 0 ){
			count = pListEntries->getNumItems();
			for( i=0; i<count; i++ ){
				if( pListEntries->isItemSelected( i ) ){
					if( entry ){
						entry = NULL;
						break;
					}
					
					entry = ( dmopmPackageEntry* )pListEntries->getItemData( i );
				}
			}
			
		}else{
			count = pListBrowserFiles->getNumItems();
			for( i=0; i<count; i++ ){
				if( pListBrowserFiles->isItemSelected( i ) ){
					if( entry ){
						entry = NULL;
						break;
					}
					
					entry = ( dmopmPackageEntry* )pListBrowserFiles->getItemData( i );
				}
			}
		}
	}
	
	if( entry ){
		FXFileDialog dialog( this, "Select Import File" );
		
		dialog.setFilename( pPathImport );
		
		if( dialog.execute( PLACEMENT_OWNER ) == TRUE ){
			pPathImport = dialog.getFilename();
			
			entry->SetImportPath( pPathImport );
			
			UpdatePackage();
		}
	}
	
	return 1;
}

long dmopmPanelEntries::onPUClearImportFile( FXObject *sender, FXSelector selector, void *data ){
	if( pWindowMain->GetPackage() ){
		dmopmPackageEntry *entry = NULL;
		int i, count;
		
		if( pTabPanels->getCurrent() == 0 ){
			count = pListEntries->getNumItems();
			for( i=0; i<count; i++ ){
				if( pListEntries->isItemSelected( i ) ){
					entry = ( dmopmPackageEntry* )pListEntries->getItemData( i );
					
					if( entry ){
						entry->SetImportPath( FXString() );
					}
				}
			}
			
		}else{
			count = pListBrowserFiles->getNumItems();
			for( i=0; i<count; i++ ){
				if( pListBrowserFiles->isItemSelected( i ) ){
					entry = ( dmopmPackageEntry* )pListBrowserFiles->getItemData( i );
					
					if( entry ){
						entry->SetImportPath( FXString() );
					}
				}
			}
		}
		
		UpdatePackage();
	}
	
	return 1;
}
