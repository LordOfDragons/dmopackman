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

#ifndef _DMOPMMEMORYSTREAM_H_
#define _DMOPMMEMORYSTREAM_H_

#include "../foxtoolkit.h"



/**
 * @brief Fixed FXMemoryStream.
 * FXMemoryStream has a broken seeking function. This class replaces the broken
 * seeking function with a correct one.
 * @author Plüss Roland
 * @version 1.0
 * @date 2011
 */
class dmopmMemoryStream : public FXMemoryStream{
public:
	/** @name Constructors and Destructors */
	/*@{*/
	/** Creates a new fixed stream. */
	dmopmMemoryStream( const FXObject *cont = NULL );
	/** Cleans up the fixed stream. */
	virtual ~dmopmMemoryStream();
	/*@}*/
	
	/** @name Management */
	/*@{*/
	/** Fixed seeking function. */
	virtual bool position( FXlong offset, FXWhence whence = FXFromStart );
	/*@}*/
};

#endif
