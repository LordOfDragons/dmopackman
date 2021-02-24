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

#ifndef _DMOPMDOWNLOADER_H_
#define _DMOPMDOWNLOADER_H_

#include "../foxtoolkit.h"

#include <curl/curl.h>

class cThreadDownloadAsync;



/**
 * @brief Downloader.
 * @author Plüss Roland
 * @version 1.0
 * @date 2011
 */
class dmopmDownloader{
private:
	CURL *pCurlEasy;
	cThreadDownloadAsync *pThread;
	bool pDoesAsyncDownload;
	
public:
	/** @name Constructors and Destructors */
	/*@{*/
	/** Creates a new downloader. */
	dmopmDownloader();
	/** Cleans up the downloader. */
	~dmopmDownloader();
	/*@}*/
	
	/** @name Management */
	/*@{*/
	/** Retrieves a file synchronously. */
	void GetFileSync( const FXString &url, FXString &data );
	
	/** Determines if an asynchronous download is in progress. */
	inline bool GetDoesAsyncDownload() const{ return pDoesAsyncDownload; }
	/** Check asynchronous download for progress if one is running. */
	void UpdateAsyncDownload();
	/** Retrieves a file asynchronously. Fails if such a download is running already. */
	void GetFileAsync( const FXString &url );
	/** Stop asynchronous download if one is running. */
	void StopGetFileAsync();
	/** Retrieves the asynchronously downloaded data. Fails if an asynchronous download is running. */
	const FXString &GetDataAsync();
	/** Retrieves the progress of the asynchronous download if any. */
	void GetProgressAsync( FXuint &progress, FXuint &total );
	/*@}*/
};

#endif
