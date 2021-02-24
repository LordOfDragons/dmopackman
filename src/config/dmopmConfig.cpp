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

#include "dmopmConfig.h"
#include "../exceptions.h"
#include "../package/dmopmPackage.h"



// Class dmopmConfig
//////////////////////

// Constructor, destructor
////////////////////////////

dmopmConfig::dmopmConfig(){
	pVerbose = false;
	pGameType = dmopmPackage::egtEnglish;
}

dmopmConfig::~dmopmConfig(){
}



// Management
///////////////

void dmopmConfig::SetPathDMO( const FXString &path ){
	pPathDMO = path;
}

void dmopmConfig::SetVerbose( bool verbose ){
	pVerbose = verbose;
}

void dmopmConfig::SetGameType( int gameType ){
	pGameType = gameType;
}
