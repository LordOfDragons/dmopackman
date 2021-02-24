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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dmopmDownloader.h"
#include "../exceptions.h"



// Pre-Definition
///////////////////

size_t cbWriteDataSync( void *buffer, size_t size, size_t nmemb, void *userdata );
size_t cbHeaderFunctionSync( void *ptr, size_t size, size_t nmemb, void *userdata );
size_t cbWriteDataAsync( void *buffer, size_t size, size_t nmemb, void *userdata );
size_t cbHeaderFunctionAsync( void *ptr, size_t size, size_t nmemb, void *userdata );



// Asynchronous Download Thread
/////////////////////////////////

class cThreadDownloadAsync : public FXThread{
public:
	enum eStatus{
		esDownloading,
		esSuccessful,
		esFailed
	};
	
private:
	CURL *pCurl;
	FXString pData;
	FXString pURL;
	int pTotalDataSize;
	int pReceivedDataSize;
	int pStatus;
	bool pCancelDownload;
	FXMutex pMutex;
	
public:
	cThreadDownloadAsync(){
		pCurl = NULL;
		pTotalDataSize = 0;
		pReceivedDataSize = 0;
		pStatus = esSuccessful;
		pCancelDownload = false;
		
		pCurl = curl_easy_init();
		curl_easy_setopt( pCurl, CURLOPT_WRITEFUNCTION, cbWriteDataAsync );
		curl_easy_setopt( pCurl, CURLOPT_WRITEDATA, this );
		curl_easy_setopt( pCurl, CURLOPT_HEADERFUNCTION, cbHeaderFunctionAsync );
		curl_easy_setopt( pCurl, CURLOPT_HEADERDATA, this );
	}
	
	virtual ~cThreadDownloadAsync(){
		if( pCurl ){
			curl_easy_cleanup( pCurl );
		}
	}
	
	inline int GetTotalDataSize() const{
		return pTotalDataSize;
	}
	void SetTotalDataSize( int size ){
		pTotalDataSize = size;
	}
	inline int GetReceivedDataSize() const{
		return pReceivedDataSize;
	}
	void SetReceivedDataSize( int size ){
		pReceivedDataSize = size;
	}
	inline FXString &GetData(){
		return pData;
	}
	inline int GetStatus() const{
		return pStatus;
	}
	inline bool GetCancelDownload() const{
		return pCancelDownload;
	}
	void SetCancelDownload(){
		pCancelDownload = true;
	}
	
	void Reset(){
		pData = "";
		pURL = "";
		pTotalDataSize = 0;
		pReceivedDataSize = 0;
		pStatus = esSuccessful;
	}
	void SetURL( const FXString &url ){
		pURL = url;
	}
	
	void LockMutex(){
		pMutex.lock();
	}
	void UnlockMutex(){
		pMutex.unlock();
	}
	
	virtual FXint run(){
		LockMutex();
		pCancelDownload = false;
		pStatus = esDownloading;
		curl_easy_setopt( pCurl, CURLOPT_URL, pURL.text() );
		UnlockMutex();
		
		if( curl_easy_perform( pCurl ) == 0 ){
			LockMutex();
			pStatus = esSuccessful;
			UnlockMutex();
			
		}else{
			LockMutex();
			pStatus = esFailed;
			UnlockMutex();
		}
		
		return 0;
	}
};



// Callbacks
//////////////

size_t cbWriteDataSync( void *buffer, size_t size, size_t nmemb, void *userdata ){
	FXString *data = ( FXString* )userdata;
	
	if( data ){
		data->append( ( FXchar* )buffer, size * nmemb );
		return size * nmemb;
		
	}else{
		return 0;
	}
}

size_t cbHeaderFunctionSync( void *ptr, size_t size, size_t nmemb, void *userdata ){
	const int length = ( int )( size * nmemb );
	const char *header = ( const char * )ptr;
	
	if( strncmp( header, "Content-Length: ", 16 ) == 0 ){
		//char numBuffer[ 30 ];
		int i, bufPos = 0;
		int character;
		
		for( i=16; i<length; i++ ){
			character = header[ i ];
			
			if( isspace( character ) ){
				break;
				
			}else{
				if( bufPos == 30 ){
					return 0;
				}
				//numBuffer[ bufPos++ ] = ( char )character;
			}
		}
		
		//numBuffer[ bufPos ] = '\0';
		
		//printf( "Header: content length = %i bytes\n", atoi( ( char* )&numBuffer ) );
	}
	
	return size * nmemb;
}

size_t cbWriteDataAsync( void *buffer, size_t size, size_t nmemb, void *userdata ){
	cThreadDownloadAsync &thread = *( ( cThreadDownloadAsync* )userdata );
	size_t result = 0;
	
	thread.LockMutex();
	if( ! thread.GetCancelDownload() ){
		thread.GetData().append( ( FXchar* )buffer, size * nmemb );
		thread.SetReceivedDataSize( thread.GetData().length() );
		result = size * nmemb;
	}
	thread.UnlockMutex();
	
	return result;
}

size_t cbHeaderFunctionAsync( void *ptr, size_t size, size_t nmemb, void *userdata ){
	const int length = ( int )( size * nmemb );
	const char *header = ( const char * )ptr;
	
	if( strncmp( header, "Content-Length: ", 16 ) == 0 ){
		cThreadDownloadAsync &thread = *( ( cThreadDownloadAsync* )userdata );
		char numBuffer[ 30 ];
		int i, bufPos = 0;
		int character;
		
		for( i=16; i<length; i++ ){
			character = header[ i ];
			
			if( isspace( character ) ){
				break;
				
			}else{
				if( bufPos == 30 ){
					return 0;
				}
				numBuffer[ bufPos++ ] = ( char )character;
			}
		}
		
		numBuffer[ bufPos ] = '\0';
		
		thread.LockMutex();
		thread.SetTotalDataSize( atoi( ( char* )&numBuffer ) );
		thread.UnlockMutex();
	}
	
	return size * nmemb;
}



// Class dmopmDownloader
//////////////////////////

// Constructor, destructor
////////////////////////////

dmopmDownloader::dmopmDownloader(){
	pCurlEasy = NULL;
	pThread = NULL;
	pDoesAsyncDownload = false;
	
	curl_global_init( CURL_GLOBAL_ALL );
	
	pCurlEasy = curl_easy_init();
	curl_easy_setopt( pCurlEasy, CURLOPT_WRITEFUNCTION, cbWriteDataSync );
	curl_easy_setopt( pCurlEasy, CURLOPT_WRITEDATA, NULL );
	curl_easy_setopt( pCurlEasy, CURLOPT_HEADERFUNCTION, cbHeaderFunctionSync );
	curl_easy_setopt( pCurlEasy, CURLOPT_HEADERDATA, NULL );
	
	pThread = new cThreadDownloadAsync;
}

dmopmDownloader::~dmopmDownloader(){
	StopGetFileAsync();
	if( pThread ){
		delete pThread;
	}
	
	if( pCurlEasy ){
		curl_easy_cleanup( pCurlEasy );
	}
	
	curl_global_cleanup();
}



// Management
///////////////

void dmopmDownloader::GetFileSync( const FXString &url, FXString &data ){
	curl_easy_setopt( pCurlEasy, CURLOPT_URL, url.text() );
	curl_easy_setopt( pCurlEasy, CURLOPT_WRITEDATA, &data );
	if( curl_easy_perform( pCurlEasy ) != 0 ){
		curl_easy_setopt( pCurlEasy, CURLOPT_WRITEDATA, NULL );
		THROW( dueInvalidAction );
	}
	curl_easy_setopt( pCurlEasy, CURLOPT_WRITEDATA, NULL );
}



void dmopmDownloader::UpdateAsyncDownload(){
	if( pDoesAsyncDownload ){
		int status = cThreadDownloadAsync::esDownloading;
		//int receivedBytes = 0;
		int totalBytes = 0;
		
		pThread->LockMutex();
		status = pThread->GetStatus();
		totalBytes = pThread->GetTotalDataSize();
		//receivedBytes = pThread->GetReceivedDataSize();
		pThread->UnlockMutex();
		
		if( status == cThreadDownloadAsync::esDownloading ){
			if( totalBytes > 0 ){
				//printf( "received %i from %i (%.0f%%)\n", receivedBytes, totalBytes, 100.0f * ( ( float )receivedBytes / ( float )totalBytes ) );
				
			}else{
				//printf( "received %i from ? (?%%)\n", receivedBytes );
			}
			
		}else{
			StopGetFileAsync();
		}
	}
}

void dmopmDownloader::GetFileAsync( const FXString &url ){
	if( pDoesAsyncDownload ){
		THROW( dueInvalidParam );
	}
	
	pThread->Reset();
	pThread->SetURL( url );
	pThread->start();
	pDoesAsyncDownload = true;
}

void dmopmDownloader::StopGetFileAsync(){
	if( pDoesAsyncDownload ){
		int status;
		
		pThread->LockMutex();
		status = pThread->GetStatus();
		pThread->SetCancelDownload();
		pThread->UnlockMutex();
		
		if( status == cThreadDownloadAsync::esDownloading ){
			pThread->join();
		}
		
		pDoesAsyncDownload = false;
	}
}

const FX::FXString& dmopmDownloader::GetDataAsync(){
	if( pDoesAsyncDownload ){
		THROW( dueInvalidParam );
	}
	
	return pThread->GetData();
}

void dmopmDownloader::GetProgressAsync( FXuint &progress, FXuint &total ){
	if( pDoesAsyncDownload ){
		pThread->LockMutex();
		progress = pThread->GetReceivedDataSize();
		total = pThread->GetTotalDataSize();
		pThread->UnlockMutex();
		
	}else{
		progress = 0;
		total = 1;
	}
}
