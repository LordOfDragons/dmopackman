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
#include <inttypes.h>

#include "dmopmHashValue.h"
#include "../exceptions.h"
#include "sha1.h"



// Class dmopmHashValue
/////////////////////////

// Constructor, destructor
////////////////////////////

dmopmHashValue::dmopmHashValue(){
	pValues[ 0 ] = 0;
	pValues[ 1 ] = 0;
	pValues[ 2 ] = 0;
	pValues[ 3 ] = 0;
	pValues[ 4 ] = 0;
}

dmopmHashValue::~dmopmHashValue(){
}



// Management
///////////////

void dmopmHashValue::GetAsString( FXString &string ) const{
	string.format( "0x%08X0x%08X0x%08X0x%08X0x%08X", pValues[ 0 ], pValues[ 1 ], pValues[ 2 ], pValues[ 3 ], pValues[ 4 ] );
}

void dmopmHashValue::SetValue( SHA1 &sha1 ){
	sha1.Result( ( unsigned int * )&pValues );
}

dmopmHashValue &dmopmHashValue::operator=( const dmopmHashValue &hashValue ){
	pValues[ 0 ] = hashValue.pValues[ 0 ];
	pValues[ 1 ] = hashValue.pValues[ 1 ];
	pValues[ 2 ] = hashValue.pValues[ 2 ];
	pValues[ 3 ] = hashValue.pValues[ 3 ];
	pValues[ 4 ] = hashValue.pValues[ 4 ];
	return *this;
}

bool dmopmHashValue::operator==( const dmopmHashValue &hashValue ) const{
	return pValues[ 0 ] == hashValue.pValues[ 0 ]
		&& pValues[ 1 ] == hashValue.pValues[ 1 ]
		&& pValues[ 2 ] == hashValue.pValues[ 2 ]
		&& pValues[ 3 ] == hashValue.pValues[ 3 ]
		&& pValues[ 4 ] == hashValue.pValues[ 4 ];
}
