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

#ifndef _DMOPMZIPREADER_H_
#define _DMOPMZIPREADER_H_

#include <vector>
#include "../foxtoolkit.h"

#include <unzip.h>

class dmopmPackageEntry;



/**
 * @brief Zip Reader.
 * Reads a zip file storing the content as a list of package entries.
 * @author Plüss Roland
 * @version 1.0
 * @date 2011
 */
class dmopmZipReader{
private:
	std::vector<dmopmPackageEntry*> pEntries;
	
	FXString pStreamName;
	FXStream *pStream;
	unzFile pZipFile;
	int pZipFileEntryCount;
	int pZipFileCurEntry;
	bool pReadData;
	
public:
	/** @name Constructors and Destructors */
	/*@{*/
	/** Creates a new zip reader. */
	dmopmZipReader();
	/** Cleans up the zip reader. */
	~dmopmZipReader();
	/*@}*/
	
	/** @name Management */
	/*@{*/
	/** Open zip file from stream. */
	void Open( FXStream *stream, const FXString &name );
	/** Read the next entry from the stream. Returns true if an entry is read or false if there are no more entries. */
	bool ReadNextEntry();
	/** Close the zip file and stream. */
	void Close();
	/** Sets if data is read too. */
	void SetReadData( bool readData );
	
	/** Retrieves the number of entries. */
	int GetEntryCount() const;
	/** Retrieves the entry at the given position. */
	dmopmPackageEntry *GetEntryAt( int index ) const;
	/** Adds an entry. */
	void AddEntry( dmopmPackageEntry *entry );
	/** Removes an entry. */
	void RemoveEntry( dmopmPackageEntry *entry );
	/** Removes all entries. */
	void RemoveAllEntries();
	/*@}*/
};

#endif
