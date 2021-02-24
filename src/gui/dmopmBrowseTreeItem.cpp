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

#include "dmopmBrowseTreeItem.h"
#include "../package/dmopmPackageEntry.h"
#include "../exceptions.h"



// Class dmopmBrowseTreeItem
//////////////////////////////

FXIMPLEMENT( dmopmBrowseTreeItem, FXTreeItem, NULL, 0 )

// Constructor, destructor
////////////////////////////

dmopmBrowseTreeItem::dmopmBrowseTreeItem(){ }

dmopmBrowseTreeItem::dmopmBrowseTreeItem( const FXString &text, FXIcon *iconOpen, FXIcon *iconClosed ) :
FXTreeItem( text, iconOpen, iconClosed, NULL ){
	pUnknown = true;
}

dmopmBrowseTreeItem::dmopmBrowseTreeItem( const FXString &path, const FXString &text, FXIcon *iconOpen, FXIcon *iconClosed ) :
FXTreeItem( text, iconOpen, iconClosed, NULL ){
	pPath = path;
	pName = FXPath::name( path );
	pUnknown = false;
}

dmopmBrowseTreeItem::~dmopmBrowseTreeItem(){
}



// Management
///////////////

