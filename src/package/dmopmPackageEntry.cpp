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
#include <algorithm>
#include <functional>

#include "dmopmPackageEntry.h"
#include "../exceptions.h"



// Class dmopmPackageEntry
////////////////////////////

// Constructor, destructor
////////////////////////////

dmopmPackageEntry::dmopmPackageEntry(){
	pID = 0;
	pOffset = 0;
	pSize = 0;
	pExtra = 0;
}

dmopmPackageEntry::~dmopmPackageEntry(){
}



// Management
///////////////

void dmopmPackageEntry::SetID( unsigned int id ){
	pID = id;
}

void dmopmPackageEntry::SetOffset( unsigned int offset ){
	pOffset = offset;
}

void dmopmPackageEntry::SetSize( unsigned int size ){
	pSize = size;
}

void dmopmPackageEntry::SetExtra( unsigned int extra ){
	pExtra = extra;
}

void dmopmPackageEntry::SetPath( const FXString &path ){
	pPath = path;
	pPathDir = FXPath::directory( path );
	pPathName = FXPath::name( path );
}

void dmopmPackageEntry::SetHashValue( const dmopmHashValue &hashValue ){
	pHashValue = hashValue;
}

void dmopmPackageEntry::SetImportPath( const FXString &path ){
	pImportPath = path;
}
