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

#ifndef _DMOPMPACKAGEENTRY_H_
#define _DMOPMPACKAGEENTRY_H_

#include <vector>
#include "../foxtoolkit.h"
#include "dmopmHashValue.h"



/**
 * @brief Package Entry.
 * @author Plüss Roland
 * @version 1.0
 * @date 2011
 */
class dmopmPackageEntry{
private:
	unsigned int pID;
	unsigned int pOffset;
	unsigned int pSize;
	unsigned int pExtra;
	FXString pPath;
	FXString pPathDir;
	FXString pPathName;
	dmopmHashValue pHashValue;
	FXString pImportPath;
	FXString pData;
	
public:
	/** @name Constructors and Destructors */
	/*@{*/
	/** Creates a new package entry. */
	dmopmPackageEntry();
	/** Cleans up the package entry. */
	~dmopmPackageEntry();
	/*@}*/
	
	/** @name Management */
	/*@{*/
	/** Retrieves the ID. */
	inline unsigned int GetID() const{ return pID; }
	/** Sets the ID. */
	void SetID( unsigned int id );
	/** Retrieves the offset. */
	inline unsigned int GetOffset() const{ return pOffset; }
	/** Sets the offset. */
	void SetOffset( unsigned int offset );
	/** Retrieves the size. */
	inline unsigned int GetSize() const{ return pSize; }
	/** Sets the size. */
	void SetSize( unsigned int size );
	/** Retrieves the extra data. */
	inline unsigned int GetExtra() const{ return pExtra; }
	/** Sets the extra data. */
	void SetExtra( unsigned int extra );
	/** Retrieves the path. */
	inline const FXString &GetPath() const{ return pPath; }
	/** Sets the path. */
	void SetPath( const FXString &path );
	/** Retrieves the path directory part. */
	inline const FXString &GetPathDir() const{ return pPathDir; }
	/** Retrieves the path name part. */
	inline const FXString &GetPathName() const{ return pPathName; }
	/** Retrieves the hash of the content. */
	inline const dmopmHashValue &GetHashValue() const{ return pHashValue; }
	/** Sets the hash value of the content. */
	void SetHashValue( const dmopmHashValue &hashValue );
	/** Retrieves the import path or an empty string to not change. */
	inline const FXString &GetImportPath() const{ return pImportPath; }
	/** Sets the import path or an empty string to not change. */
	void SetImportPath( const FXString &path );
	/** Retrieves the entry data. */
	inline FXString &GetData(){ return pData; }
	inline const FXString &GetData() const{ return pData; }
	/*@}*/
};

#endif
