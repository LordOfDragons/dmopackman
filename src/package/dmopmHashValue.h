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

#ifndef _DMOPMHASHVALUE_H_
#define _DMOPMHASHVALUE_H_

#include "../foxtoolkit.h"

class SHA1;



/**
 * @brief Hash Value.
 * Stores a hash value.
 * @author Plüss Roland
 * @version 1.0
 * @date 2011
 */
class dmopmHashValue{
private:
	unsigned int pValues[ 5 ];
	
public:
	/** @name Constructors and Destructors */
	/*@{*/
	/** Creates a new hash value. */
	dmopmHashValue();
	/** Cleans up the hash value. */
	~dmopmHashValue();
	/*@}*/
	
	/** @name Management */
	/*@{*/
	/** Retrieves the hash value as string. */
	void GetAsString( FXString &string ) const;
	/** Sets the hash value from an SHA1 instance. */
	void SetValue( SHA1 &sha1 );
	/** Sets the hash value. */
	dmopmHashValue &operator=( const dmopmHashValue &hashValue );
	/** Determines if two hash values are equal. */
	bool operator==( const dmopmHashValue &hashValue ) const;
	/*@}*/
};

#endif
