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

#include "dmopmMemoryStream.h"
#include "../exceptions.h"



// Class dmopmMemoryStream
////////////////////////////

// Constructor, destructor
////////////////////////////

dmopmMemoryStream::dmopmMemoryStream( const FXObject *cont ) : FXMemoryStream( cont ){
}

dmopmMemoryStream::~dmopmMemoryStream(){
}



// Management
///////////////

bool dmopmMemoryStream::position( FXlong offset, FXWhence whence ){
	// FXMemoryStream::position(FXlong,FXWhence) has a nasty bug included. At various
	// places checks are made in the form "if(something >= endptr) setError". This is
	// incorrect as it should be ">" instead of ">=". this especially makes seeking
	// at the end of memory stream fail
	
	if( dir == FXStreamDead ){
		fxerror( "FXMemoryStream::position: stream is not open.\n" );
	}
	
	if( code == FXStreamOK ){
		if( whence == FXFromCurrent ){
			offset = offset + pos;
			
		}else if( whence == FXFromEnd ){
			offset = offset + endptr - begptr;
		}
		
		if( dir == FXStreamSave ){
			if( begptr + offset > endptr ){
				if( ! owns ){
					setError( FXStreamFull );
					return false;
				}
				setSpace( offset );
				if( begptr + offset > endptr ){
					return false;
				}
			}
			wrptr = begptr + offset;
			
		}else{
			if( begptr + offset > endptr ){
				setError( FXStreamEnd );
				return false;
			}
			rdptr = begptr + offset;
		}
		
		pos = offset;
		
		return true;
	}
	
	return false;
}
