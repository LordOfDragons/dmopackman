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

#include "dmopmArchiveFile.h"
#include "../config/dmopmConfig.h"
#include "../package/dmopmPackage.h"
#include "../package/dmopmPackageEntry.h"
#include "../package/dmopmZipReader.h"
#include "../exceptions.h"



// Class dmopmArchiveFile
///////////////////////////

// Constructor, destructor
////////////////////////////

dmopmArchiveFile::dmopmArchiveFile( dmopmConfig *config, dmopmPackage *package ){
	if( ! config || ! package ){
		THROW( dueInvalidParam );
	}
	
	pConfig = config;
	pPackage = package;
}

dmopmArchiveFile::~dmopmArchiveFile(){
}



// Management
///////////////

void dmopmArchiveFile::ReadFileMap( dmopmPackage &package ){
	const FXString &pathDMO = pConfig->GetPathDMO();
	const FXString &pathFile = pPackage->GetPathHF();
	const FXString pathAbsolute( FXPath::absolute( pathDMO, pathFile ) );
	dmopmPackageEntry *entry = NULL;
	FXFileStream *stream = NULL;
	unsigned int entrySize;
	unsigned int entryID;
	unsigned int entryOffset;
	unsigned int entryExtra;
	int e, entryCount;
	FXuint valueUInt;
	
	try{
		package.RemoveAllEntries();
		
		// open the file
		printf( "reading file map '%s'...\n", pathAbsolute.text() );
		stream = new FXFileStream;
		if( ! stream ) THROW( dueOutOfMemory );
		if( ! stream->open( pathAbsolute, FXStreamLoad ) ) THROW( dueFileNotFound );
		
		// first value is always 16
		*stream >> valueUInt;
		if( valueUInt != 16 ) THROW( dueInvalidFileFormat );
		
		// second value is the number of entries
		*stream >> valueUInt;
		entryCount = ( int )valueUInt;
		
		// read maps
		for( e=0; e<entryCount; e++ ){
			// always 1
			*stream >> valueUInt;
			if( valueUInt != 1 ){
				THROW( dueInvalidFileFormat );
			}
			
			// file size
			*stream >> valueUInt;
			entrySize = ( unsigned int )valueUInt;
			
			// file size again
			*stream >> valueUInt;
			// this SHOULD be equal to entrySize but digitalic fucks this up from time to time U_U
			//if( valueUInt != entrySize ){
			//	THROW( dueInvalidFileFormat );
			//}
			
			// file identifier
			*stream >> valueUInt;
			entryID = ( unsigned int )valueUInt;
			
			// file offset
			*stream >> valueUInt;
			entryOffset = ( unsigned int )valueUInt;
			
			// always 0 or 1. in the old version this had been always 0. now there seems to be also
			// another value in there.
			*stream >> valueUInt;
			entryExtra = ( unsigned int )valueUInt;
			
			// add entry
			entry = new dmopmPackageEntry;
			entry->SetID( entryID );
			entry->SetOffset( entryOffset );
			entry->SetSize( entrySize );
			entry->SetExtra( entryExtra );
			package.AddEntry( entry );
			entry = NULL;
		}
		
		// close the file
		printf( "closing file.\n" );
		delete stream;
		
	}catch( duException e ){
		if( entry ){
			delete entry;
		}
		if( stream ){
			delete stream;
		}
		throw;
	}
}

void dmopmArchiveFile::ReadFileData( const dmopmPackageEntry &entry, FXString &data ){
	const FXString &pathDMO = pConfig->GetPathDMO();
	const FXString &pathFile = pPackage->GetPathPF();
	const FXString pathAbsolute( FXPath::absolute( pathDMO, pathFile ) );
	FXFileStream *stream = NULL;
	
	try{
		// open the archive file
		stream = new FXFileStream;
		if( ! stream ) THROW( dueOutOfMemory );
		if( ! stream->open( pathAbsolute, FXStreamLoad ) ) THROW( dueFileNotFound );
		
		stream->position( entry.GetOffset() );
		
		// read file data
		if( entry.GetSize() > 0 ){
			data.assign( ' ', entry.GetSize() );
			stream->load( ( FXchar* )data.text(), entry.GetSize() );
			
		}else{
			data = "";
		}
		
		// close the file
		delete stream;
		
	}catch( duException e ){
		if( stream ) delete stream;
		throw;
	}
}

void dmopmArchiveFile::MkDir( const FXString &path ) const{
	FXDir dir;
	
	if( dir.open( path.text() ) ){
		dir.close();
		
	}else{
		FXString parentDir = FXPath::directory( path );
		
		if( parentDir.empty() ){
			THROW( dueFileNotFound );
		}
		
		MkDir( parentDir );
		
		if( ! FXDir::create( path ) ){
			THROW( dueWriteFile );
		}
	}
}

void dmopmArchiveFile::UpdateEntriesFromArchive(){
}
