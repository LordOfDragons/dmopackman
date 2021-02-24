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

#ifndef _DMOPMARCHIVEFILE_H_
#define _DMOPMARCHIVEFILE_H_

#include "../foxtoolkit.h"

class dmopmPackage;
class dmopmPackageEntry;
class dmopmConfig;



/**
 * @brief Archive File.
 * Supports reading and writing archive files.
 * @author Plüss Roland
 * @version 1.0
 * @date 2011
 */
class dmopmArchiveFile{
private:
	dmopmPackage *pPackage;
	dmopmConfig *pConfig;
	
public:
	/** @name Constructors and Destructors */
	/*@{*/
	/** Creates a new archive file. */
	dmopmArchiveFile( dmopmConfig *config, dmopmPackage *package );
	/** Cleans up the archive file. */
	~dmopmArchiveFile();
	/*@}*/
	
	/** @name Management */
	/*@{*/
	/** Read file map from archive into package replacing all entries. */
	void ReadFileMap( dmopmPackage &package );
	/** Read a file data from the archive. */
	void ReadFileData( const dmopmPackageEntry &entry, FXString &data );
	
	/** Create directory and all its parents if not existing. */
	void MkDir( const FXString &path ) const;
	
	/** Updates the entries in the package from the archive file. */
	void UpdateEntriesFromArchive();
	/*@}*/
};

#endif
