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

#include "dmopmLoadSavePackage.h"
#include "../package/dmopmPackage.h"
#include "../package/dmopmPackageEntry.h"
#include "../exceptions.h"



// Class dmopmLoadSavePackage
///////////////////////////////

// Constructor, destructor
////////////////////////////

dmopmLoadSavePackage::dmopmLoadSavePackage(){
}

dmopmLoadSavePackage::~dmopmLoadSavePackage(){
}



// Management
///////////////

void dmopmLoadSavePackage::LoadFromFile( const FXString &filename, dmopmPackage &package ){
	FXchar fileSignature[] = "DMOPackage======"; // 16 chars
	FXchar checkFileSignature[ 16 ];
	FXshort versionMajor, versionMinor;
	FXFileStream *stream = NULL;
	
	try{
		// open the file
		printf( "reading package from '%s'...\n", filename.text() );
		stream = new FXFileStream;
		if( ! stream ) THROW( dueOutOfMemory );
		stream->open( filename, FXStreamLoad );
		
		// magic string
		stream->load( &checkFileSignature[ 0 ], 16 );
		if( compare( &checkFileSignature[ 0 ], &fileSignature[ 0 ], 16 ) != 0 ) THROW( dueInvalidFileFormat );
		
		// version
		*stream >> versionMajor;
		*stream >> versionMinor;
		
		if( versionMajor == 1 and versionMinor == 0 ){
			LoadFromFileV1_0( *stream, package );
			
		}else if( versionMajor == 1 and versionMinor == 1 ){
			LoadFromFileV1_1( *stream, package );
			
		}else if( versionMajor == 1 and versionMinor == 2 ){
			LoadFromFileV1_2( *stream, package );
			
		}else{
			printf( "unrecognized version %i.%i\n", versionMajor, versionMinor );
			THROW( dueInvalidFileFormat );
		}
		
		// close the file
		printf( "closing file.\n" );
		delete stream;
		
	}catch( duException ){
		if( stream ){
			delete stream;
		}
		throw;
	}
}

void dmopmLoadSavePackage::LoadFromFileV1_0( FXFileStream &stream, dmopmPackage &package ){
	dmopmPackageEntry *entry = NULL;
	FXint e, entryCount;
	FXshort valueShort;
	FXuint valueUInt;
	FXString string;
	FXint valueInt;
	
	package.SetGameType( dmopmPackage::egtKorean );
	
	// short len(pathPF)
	// char[] pathPF
	stream >> valueShort;
	string.assign( ' ', valueShort );
	stream.load( ( FXchar* )string.text(), valueShort );
	if( PATHSEP != '\\' ){
		string.substitute( '\\', PATHSEP );
	}
	package.SetPathPF( string );
	
	// short len(pathHF)
	// char[] pathHF
	stream >> valueShort;
	string.assign( ' ', valueShort );
	stream.load( ( FXchar* )string.text(), valueShort );
	if( PATHSEP != '\\' ){
		string.substitute( '\\', PATHSEP );
	}
	package.SetPathHF( string );
	
	// short len(contentPathPrefix)
	// char[] contentPathPrefix
	stream >> valueShort;
	string.assign( ' ', valueShort );
	stream.load( ( FXchar* )string.text(), valueShort );
	if( PATHSEP != '\\' ){
		string.substitute( '\\', PATHSEP );
	}
	package.SetContentPathPrefix( string );
	
	// int updateVersion
	stream >> valueInt;
	package.SetVersion( valueInt );
	
	// int len( entries )
	stream >> entryCount;
	for( e=0; e<entryCount; e++ ){
		entry = new dmopmPackageEntry;
		if( ! entry ){
			THROW( dueOutOfMemory );
		}
		
		// uint id
		stream >> valueUInt;
		entry->SetID( valueUInt );
		
		// uint offset
		stream >> valueUInt;
		entry->SetOffset( valueUInt );
		
		// uint size
		stream >> valueUInt;
		entry->SetSize( valueUInt );
		
		// short len(path)
		// char[] path
		stream >> valueShort;
		string.assign( ' ', valueShort );
		stream.load( ( FXchar* )string.text(), valueShort );
		if( PATHSEP != '\\' ){
			string.substitute( '\\', PATHSEP );
		}
		entry->SetPath( string );
		
		package.AddEntry( entry );
		entry = NULL;
	}
}

void dmopmLoadSavePackage::LoadFromFileV1_1( FXFileStream &stream, dmopmPackage &package ){
	dmopmPackageEntry *entry = NULL;
	FXint e, entryCount;
	FXshort valueShort;
	FXuint valueUInt;
	FXString string;
	FXint valueInt;
	
	// short len(pathPF)
	// char[] pathPF
	stream >> valueShort;
	string.assign( ' ', valueShort );
	stream.load( ( FXchar* )string.text(), valueShort );
	if( PATHSEP != '\\' ){
		string.substitute( '\\', PATHSEP );
	}
	package.SetPathPF( string );
	
	// short len(pathHF)
	// char[] pathHF
	stream >> valueShort;
	string.assign( ' ', valueShort );
	stream.load( ( FXchar* )string.text(), valueShort );
	if( PATHSEP != '\\' ){
		string.substitute( '\\', PATHSEP );
	}
	package.SetPathHF( string );
	
	// short len(contentPathPrefix)
	// char[] contentPathPrefix
	stream >> valueShort;
	string.assign( ' ', valueShort );
	stream.load( ( FXchar* )string.text(), valueShort );
	if( PATHSEP != '\\' ){
		string.substitute( '\\', PATHSEP );
	}
	package.SetContentPathPrefix( string );
	
	// int updateVersion
	stream >> valueInt;
	package.SetVersion( valueInt );
	
	// int gameType
	stream >> valueInt;
	package.SetGameType( valueInt );
	
	// int len( entries )
	stream >> entryCount;
	for( e=0; e<entryCount; e++ ){
		entry = new dmopmPackageEntry;
		if( ! entry ){
			THROW( dueOutOfMemory );
		}
		
		// uint id
		stream >> valueUInt;
		entry->SetID( valueUInt );
		
		// uint offset
		stream >> valueUInt;
		entry->SetOffset( valueUInt );
		
		// uint size
		stream >> valueUInt;
		entry->SetSize( valueUInt );
		
		// short len(path)
		// char[] path
		stream >> valueShort;
		string.assign( ' ', valueShort );
		stream.load( ( FXchar* )string.text(), valueShort );
		if( PATHSEP != '\\' ){
			string.substitute( '\\', PATHSEP );
		}
		entry->SetPath( string );
		
		package.AddEntry( entry );
		entry = NULL;
	}
}

void dmopmLoadSavePackage::LoadFromFileV1_2( FXFileStream &stream, dmopmPackage &package ){
	dmopmPackageEntry *entry = NULL;
	FXint e, entryCount;
	FXshort valueShort;
	FXuint valueUInt;
	FXString string;
	FXint valueInt;
	
	// short len(pathPF)
	// char[] pathPF
	stream >> valueShort;
	string.assign( ' ', valueShort );
	stream.load( ( FXchar* )string.text(), valueShort );
	if( PATHSEP != '\\' ){
		string.substitute( '\\', PATHSEP );
	}
	package.SetPathPF( string );
	
	// short len(pathHF)
	// char[] pathHF
	stream >> valueShort;
	string.assign( ' ', valueShort );
	stream.load( ( FXchar* )string.text(), valueShort );
	if( PATHSEP != '\\' ){
		string.substitute( '\\', PATHSEP );
	}
	package.SetPathHF( string );
	
	// short len(contentPathPrefix)
	// char[] contentPathPrefix
	stream >> valueShort;
	string.assign( ' ', valueShort );
	stream.load( ( FXchar* )string.text(), valueShort );
	if( PATHSEP != '\\' ){
		string.substitute( '\\', PATHSEP );
	}
	package.SetContentPathPrefix( string );
	
	// int updateVersion
	stream >> valueInt;
	package.SetVersion( valueInt );
	
	// int gameType
	stream >> valueInt;
	package.SetGameType( valueInt );
	
	// int len( entries )
	stream >> entryCount;
	for( e=0; e<entryCount; e++ ){
		entry = new dmopmPackageEntry;
		if( ! entry ){
			THROW( dueOutOfMemory );
		}
		
		// uint id
		stream >> valueUInt;
		entry->SetID( valueUInt );
		
		// uint offset
		stream >> valueUInt;
		entry->SetOffset( valueUInt );
		
		// uint size
		stream >> valueUInt;
		entry->SetSize( valueUInt );
		
		// uint extra
		stream >> valueUInt;
		entry->SetExtra( valueUInt );
		
		// short len(path)
		// char[] path
		stream >> valueShort;
		string.assign( ' ', valueShort );
		stream.load( ( FXchar* )string.text(), valueShort );
		if( PATHSEP != '\\' ){
			string.substitute( '\\', PATHSEP );
		}
		entry->SetPath( string );
		
		package.AddEntry( entry );
		entry = NULL;
	}
}



void dmopmLoadSavePackage::SaveToFile( const FXString &filename, const dmopmPackage &package ){
	FXchar fileSignature[] = "DMOPackage======"; // 16 chars
	FXFileStream *stream = NULL;
	FXint e, entryCount;
	FXshort valueShort;
	FXuint valueUInt;
	FXString string;
	FXint valueInt;
	
	try{
		// open the file
		printf( "writing package to '%s'...\n", filename.text() );
		stream = new FXFileStream;
		if( ! stream ) THROW( dueOutOfMemory );
		stream->open( filename, FXStreamSave );
		
		// magic string
		stream->save( &fileSignature[ 0 ], 16 );
		
		// version just for the case we change the format later on
		valueShort = 1;
		*stream << valueShort;
		valueShort = 2;
		*stream << valueShort;
		
		// short len(pathPF)
		// char[] pathPF
		string = package.GetPathPF();
		if( PATHSEP != '\\' ){
			string.substitute( PATHSEP, '\\' );
		}
		valueShort = string.length();
		*stream << valueShort;
		stream->save( string.text(), valueShort );
		
		// short len(pathHF)
		// char[] pathHF
		string = package.GetPathHF();
		if( PATHSEP != '\\' ){
			string.substitute( PATHSEP, '\\' );
		}
		valueShort = string.length();
		*stream << valueShort;
		stream->save( string.text(), valueShort );
		
		// short len(contentPathPrefix)
		// char[] contentPathPrefix
		string = package.GetContentPathPrefix();
		if( PATHSEP != '\\' ){
			string.substitute( PATHSEP, '\\' );
		}
		valueShort = string.length();
		*stream << valueShort;
		stream->save( string.text(), valueShort );
		
		// int updateVersion
		valueInt = package.GetVersion();
		*stream << valueInt;
		
		// int gameType
		valueInt = package.GetGameType();
		*stream << valueInt;
		
		// int len(entries)
		entryCount = package.GetEntryCount();
		*stream << entryCount;
		for( e=0; e<entryCount; e++ ){
			const dmopmPackageEntry &entry = *package.GetEntryAt( e );
			
			// uint id
			valueUInt = entry.GetID();
			*stream << valueUInt;
			
			// uint offset
			valueUInt = entry.GetOffset();
			*stream << valueUInt;
			
			// uint size
			valueUInt = entry.GetSize();
			*stream << valueUInt;
			
			// uint extra
			valueUInt = entry.GetExtra();
			*stream << valueUInt;
			
			// short len(path)
			// char[] path
			string = entry.GetPath();
			if( PATHSEP != '\\' ){
				string.substitute( PATHSEP, '\\' );
			}
			valueShort = string.length();
			*stream << valueShort;
			stream->save( string.text(), valueShort );
		}
		
		// close the file
		printf( "closing file.\n" );
		delete stream;
		
	}catch( duException ){
		if( stream ){
			delete stream;
		}
		throw;
	}
}
