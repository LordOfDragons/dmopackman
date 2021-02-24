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

#ifndef _DMOPMPACKAGE_H_
#define _DMOPMPACKAGE_H_

#include <vector>
#include "../foxtoolkit.h"

class dmopmPackageEntry;



/**
 * @brief Package.
 * @author Plüss Roland
 * @version 1.0
 * @date 2011
 */
class dmopmPackage{
public:
	/** Game type definition. */
	enum eGameTypes{
		/** Korean version, KDMO. */
		egtKorean,
		/** English version, GDMO. */
		egtEnglish
	};
	
private:
	std::vector<dmopmPackageEntry*> pEntries;
	FXString pPathPF;
	FXString pPathHF;
	FXString pContentPathPrefix;
	int pVersion;
	int pGameType;
	
	FXString pFilename;
	bool pSaved;
	bool pChanged;
	
public:
	/** @name Constructors and Destructors */
	/*@{*/
	/** Creates a new package. */
	dmopmPackage();
	/** Cleans up the package. */
	~dmopmPackage();
	/*@}*/
	
	/** @name Management */
	/*@{*/
	/** Retrieves the filename. */
	inline const FXString &GetFilename() const{ return pFilename; }
	/** Sets the path to the filename. */
	void SetFilename( const FXString &filename );
	/** Determines if the file has been saved. */
	inline bool GetSaved() const{ return pSaved; }
	/** Sets if the file has been saved. */
	void SetSaved( bool saved );
	/** Determines if the file has changed. */
	inline bool GetChanged() const{ return pChanged; }
	/** Sets if the file has changed. */
	void SetChanged( bool changed );
	
	/** Retrieves the path to the PF file. */
	inline const FXString &GetPathPF() const{ return pPathPF; }
	/** Sets the path to the PF file. */
	void SetPathPF( const FXString &path );
	/** Retrieves the path to the HF file. */
	inline const FXString &GetPathHF() const{ return pPathHF; }
	/** Sets the path to the HF file. */
	void SetPathHF( const FXString &path );
	/** Retrieves the prefix for content path. */
	inline const FXString &GetContentPathPrefix() const{ return pContentPathPrefix; }
	/** Sets the prefix for content path. */
	void SetContentPathPrefix( const FXString &prefix );
	/** Retrieves the version this package is updated to. */
	inline int GetVersion() const{ return pVersion; }
	/** Sets the version this package is updated to. */
	void SetVersion( int version );
	/** Retrieves the game type this package is designed for. */
	inline int GetGameType() const{ return pGameType; }
	/** Sets the game type this package is designed for. */
	void SetGameType( int gameType );
	
	/** Retrieves the number of entries. */
	int GetEntryCount() const;
	/** Retrieves the entry at the given position. */
	dmopmPackageEntry *GetEntryAt( int index ) const;
	/** Retrieves the entry with the given ID or NULL if not found. */
	dmopmPackageEntry *GetEntryWithID( unsigned int id ) const;
	/** Adds an entry. */
	void AddEntry( dmopmPackageEntry *entry );
	/** Removes an entry. */
	void RemoveEntry( dmopmPackageEntry *entry );
	/** Removes all entries. */
	void RemoveAllEntries();
	/*@}*/
};

#endif
