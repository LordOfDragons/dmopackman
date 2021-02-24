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

#include "dmopmCRC.h"
#include "../exceptions.h"



// Definitions
////////////////

#define CRC32POLY 0x04C11DB7



// Class dmopmCRC
///////////////////

// Constructor, destructor
////////////////////////////

dmopmCRC::dmopmCRC(){
	pCRC = 0;
}

dmopmCRC::~dmopmCRC(){
}



// Management
///////////////

void dmopmCRC::SetCRC( unsigned int crc ){
	pCRC = crc;
}

void dmopmCRC::Add( const char *data, int length ){
	int i, j, bits;
	
	for( i=0; i<length; i++ ){
		bits = data[ i ];
		
		for( j=0; j<8; j++ ){
			if( ( ( pCRC & 0x80000000 ) ? 1 : 0 ) != ( bits & 0x1 ) ){
				pCRC = ( pCRC << 1 ) ^ CRC32POLY;
				
			}else{
				pCRC <<= 1;
			}
			
			bits >>= 1;
		}
	}
}