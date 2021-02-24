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

#ifndef _DMOPMBROWSEITEM_H_
#define _DMOPMBROWSEITEM_H_

#include "../foxtoolkit.h"

class dmopmPackageEntry;



/**
 * @brief Browse Tree Item.
 * @author Plüss Roland
 * @version 1.0
 * @date 2011
 */
class dmopmBrowseTreeItem : public FXTreeItem{
	FXDECLARE( dmopmBrowseTreeItem )
protected:
	dmopmBrowseTreeItem();
	
private:
	FXString pPath;
	FXString pName;
	bool pUnknown;
	
public:
	/** @name Constructors and Destructors */
	/*@{*/
	/** Creates a new item. */
	dmopmBrowseTreeItem( const FXString &text, FXIcon *iconOpen, FXIcon *iconClosed );
	/** Creates a new item. */
	dmopmBrowseTreeItem( const FXString &path, const FXString &text, FXIcon *iconOpen, FXIcon *iconClosed );
	/** Cleans up the item. */
	virtual ~dmopmBrowseTreeItem();
	/*@}*/
	
	/** @name Management */
	/*@{*/
	/** Retrieves the path. */
	inline const FXString &GetPath() const{  return pPath; }
	/** Retrieves the name. */
	inline const FXString &GetName() const{ return pName; }
	/** Determines if this node contains unknown entries. */
	inline bool GetUnknown() const{ return pUnknown; }
	/*@}*/
};

#endif
