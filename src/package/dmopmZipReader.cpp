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

#include "dmopmZipReader.h"
#include "dmopmPackageEntry.h"
#include "../exceptions.h"
#include <sha1.h>



// Functors
/////////////

struct fEntryEqualID{
	unsigned int pID;
	
	fEntryEqualID( unsigned int id ){
		pID = id;
	}
	
	bool operator()( dmopmPackageEntry *entry ){
		return entry->GetID() == pID;
	}
};



// Callbacks
//////////////

voidpf ZCALLBACK minizip_open_file_func( voidpf opaque, const char *filename, int mode ){
	return opaque;
}

uLong ZCALLBACK minizip_read_file_func( voidpf opaque, voidpf stream, void *buf, uLong size ){
	FXStream &fxstream = *( ( FXStream* )opaque );
	const FXlong position = fxstream.position();
	
	fxstream.load( ( FXchar* )buf, size );
	
	return ( uLong )( fxstream.position() - position );
}

uLong ZCALLBACK minizip_write_file_func( voidpf opaque, voidpf stream, const void *buf, uLong size ){
	// not implemented
	return 0;
}

long ZCALLBACK minizip_tell_file_func( voidpf opaque, voidpf stream ){
	FXStream &fxstream = *( ( FXStream* )opaque );
	
	return ( long )fxstream.position();
}

long ZCALLBACK minizip_seek_file_func( voidpf opaque, voidpf stream, uLong offset, int origin ){
	FXStream &fxstream = *( ( FXStream* )opaque );
	
	if( origin == ZLIB_FILEFUNC_SEEK_CUR ){
		return fxstream.position( offset, FXFromCurrent ) ? 0 : -1;
		
	}else if( origin == ZLIB_FILEFUNC_SEEK_END ){
		return fxstream.position( offset, FXFromEnd ) ? 0 : -1;
		
	}else if( origin == ZLIB_FILEFUNC_SEEK_SET ){
		return fxstream.position( offset, FXFromStart ) ? 0 : -1;
		
	}else{
		return -1;
	}
}

int ZCALLBACK minizip_close_file_func( voidpf opaque, voidpf stream ){
	FXStream &fxstream = *( ( FXStream* )opaque );
	
	fxstream.close();
	
	return 0;
}

int ZCALLBACK minizip_error_file_func( voidpf opaque,voidpf stream ){
	// not implemented
	return -1;
}



// Class dmopmZipReader
/////////////////////////

// Constructor, destructor
////////////////////////////

dmopmZipReader::dmopmZipReader(){
	pStream = NULL;
	pZipFile = NULL;
	pZipFileCurEntry = 0;
	pZipFileEntryCount = 0;
	pReadData = false;
}

dmopmZipReader::~dmopmZipReader(){
	Close();
	RemoveAllEntries();
}



// Management
///////////////

void dmopmZipReader::SetReadData( bool readData ){
	pReadData = readData;
}

void dmopmZipReader::Open( FXStream *stream, const FXString &name ){
	if( ! stream || pStream || pZipFile ){
		THROW( dueInvalidParam );
	}
	
	zlib_filefunc_def ffunc;
	unz_global_info gi;
	FXString comment;
	
	pStream = stream;
	pStreamName = name;
	
	try{
		ffunc.zopen_file = minizip_open_file_func;
		ffunc.zread_file = minizip_read_file_func;
		ffunc.zwrite_file = minizip_write_file_func;
		ffunc.ztell_file = minizip_tell_file_func;
		ffunc.zseek_file = minizip_seek_file_func;
		ffunc.zclose_file = minizip_close_file_func;
		ffunc.zerror_file = minizip_error_file_func;
		ffunc.opaque = stream;
		
		pZipFile = unzOpen2( pStreamName.text(), &ffunc );
		if( ! pZipFile ){
			THROW( dueReadFile );
		}
		
		if( unzGetGlobalInfo( pZipFile, &gi ) != UNZ_OK ){
			THROW( dueReadFile );
		}
		
		pZipFileEntryCount = ( int )gi.number_entry;
		
		comment.assign( ' ', gi.size_comment );
		if( unzGetGlobalComment( pZipFile, ( char* )comment.text(), gi.size_comment ) != UNZ_OK ){
			THROW( dueReadFile );
		}
		//printf( "zip file comment: '%s'\n", comment.text() );
		
	}catch( duException ){
		if( pZipFile ){
			unzClose( pZipFile );
			pZipFile = NULL;
		}
		pStream = NULL;
		throw;
	}
}

bool dmopmZipReader::ReadNextEntry(){
	if( ! pStream || ! pZipFile ){
		THROW( dueInvalidParam );
	}
	
	if( pZipFileCurEntry == pZipFileEntryCount ){
		return false;
	}
	
	dmopmPackageEntry *entry = NULL;
	dmopmHashValue hashValue;
	FXString fileDataBuffer;
	FXString extraField;
	FXString filename;
	unz_file_info fi;
	FXString comment;
	int readBytes;
	FXString data;
	SHA1 sha1;
	
	fileDataBuffer.assign( ' ', 1024 );
	
	if( unzGetCurrentFileInfo( pZipFile, &fi, NULL, 0, NULL, 0, NULL, 0 ) != UNZ_OK ){
		THROW( dueReadFile );
	}
	
	filename.assign( ' ', ( int )fi.size_filename );
	extraField.assign( ' ', ( int )fi.size_file_extra );
	comment.assign( ' ', ( int )fi.size_file_comment );
	
	if( unzGetCurrentFileInfo( pZipFile, &fi, ( char* )filename.text(), fi.size_filename, NULL,
	0, NULL, 0 ) != UNZ_OK ){
//	if( unzGetCurrentFileInfo( pZipFile, &fi, ( char* )filename.text(), fi.size_filename, ( char* )extraField.text(),
//	fi.size_file_extra, ( char* )comment.text(), fi.size_file_comment ) != UNZ_OK ){
		THROW( dueReadFile );
	}
	
	if( fi.uncompressed_size > 0 ){
		if( unzOpenCurrentFile( pZipFile ) != UNZ_OK ){
			THROW( dueReadFile );
		}
		
		if( pReadData ){
			data = "";
		}
		sha1.Reset();
		while( true ){
			readBytes = unzReadCurrentFile( pZipFile, ( char* )fileDataBuffer.text(), 1024 );
			
			if( readBytes > 0 ){
				if( pReadData ){
					data.append( fileDataBuffer.text(), readBytes );
				}
				sha1.Input( fileDataBuffer.text(), readBytes );
				
			}else if( readBytes < 0 ){
				THROW( dueReadFile );
				
			}else{ // 0 = EOF
				break;
			}
		}
		hashValue.SetValue( sha1 );
		
		if( unzCloseCurrentFile( pZipFile ) != UNZ_OK ){
			THROW( dueReadFile );
		}
		
		try{
			entry = new dmopmPackageEntry;
			
			entry->SetID( 0 );
			entry->SetOffset( 0 );
			entry->SetSize( ( unsigned int )fi.uncompressed_size );
			entry->SetExtra( 0 );
			entry->SetPath( filename );
			entry->SetHashValue( hashValue );
			if( pReadData ){
				entry->GetData() = data;
			}
			AddEntry( entry );
			
		}catch( duException ){
			if( entry ){
				delete entry;
			}
			throw;
		}
		
		FXString hashString;
		hashValue.GetAsString( hashString );
		//printf( "entry %i: filename='%s' filesize=%li hash=%s extra='%s' comment='%s'\n", pZipFileCurEntry,
		//	filename.text(), fi.uncompressed_size, hashString.text(), extraField.text(), comment.text() );
		
	}else{
		//printf( "entry %i: filename='%s' filesize=%li hash=? extra='%s' comment='%s'\n", pZipFileCurEntry,
		//	filename.text(), fi.uncompressed_size, extraField.text(), comment.text() );
	}
	
	pZipFileCurEntry++;
	if( pZipFileCurEntry < pZipFileEntryCount ){
		if( unzGoToNextFile( pZipFile ) != UNZ_OK ){
			THROW( dueReadFile );
		}
	}
	return pZipFileCurEntry < pZipFileEntryCount;
}

void dmopmZipReader::Close(){
	if( pZipFile ){
		unzClose( pZipFile );
		pZipFile = NULL;
	}
	if( pStream ){
		delete pStream;
		pStream = NULL;
	}
}



int dmopmZipReader::GetEntryCount() const{
	return pEntries.size();
}

dmopmPackageEntry *dmopmZipReader::GetEntryAt( int index ) const{
	if( index < 0 || index >= ( int )pEntries.size() ){
		THROW( dueInvalidParam );
	}
	
	return pEntries.at( index );
}

void dmopmZipReader::AddEntry( dmopmPackageEntry *entry ){
	if( ! entry ){
		THROW( dueInvalidParam );
	}
	
	pEntries.push_back( entry );
}

void dmopmZipReader::RemoveEntry( dmopmPackageEntry *entry ){
	if( ! entry ){
		THROW( dueInvalidParam );
	}
	
	std::vector<dmopmPackageEntry*>::iterator iter = std::find( pEntries.begin(), pEntries.end(), entry );
	
	if( iter == pEntries.end() ){
		THROW( dueInvalidParam );
	}
	pEntries.erase( iter );
	delete entry;
}

void dmopmZipReader::RemoveAllEntries(){
	while( ! pEntries.empty() ){
		delete pEntries.back();
		pEntries.pop_back();
	}
}
