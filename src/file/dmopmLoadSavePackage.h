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

#ifndef _DMOPMLOADSAVEPACKAGE_H_
#define _DMOPMLOADSAVEPACKAGE_H_

#include "../foxtoolkit.h"

class dmopmPackage;



/**
 * @brief Load/Save Package.
 * @author Plüss Roland
 * @version 1.0
 * @date 2011
 */
class dmopmLoadSavePackage{
public:
	/** @name Constructors and Destructors */
	/*@{*/
	/** Creates a new load/save package. */
	dmopmLoadSavePackage();
	/** Cleans up the load/save package. */
	~dmopmLoadSavePackage();
	/*@}*/
	
	/** @name Management */
	/*@{*/
	/** Load package from file. */
	void LoadFromFile( const FXString &filename, dmopmPackage &package );
	void LoadFromFileV1_0( FXFileStream &stream, dmopmPackage &package );
	void LoadFromFileV1_1( FXFileStream &stream, dmopmPackage &package );
	void LoadFromFileV1_2( FXFileStream &stream, dmopmPackage &package );
	/** Save package to file. */
	void SaveToFile( const FXString &filename, const dmopmPackage &package );
	/*@}*/
};

#endif
