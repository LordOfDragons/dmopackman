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

#include "dmopmPackage.h"
#include "dmopmPackageEntry.h"
#include "../exceptions.h"



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



// Class dmopmPackage
///////////////////////

// Constructor, destructor
////////////////////////////

dmopmPackage::dmopmPackage(){
	pFilename = "Package.dmop";
	pSaved = false;
	pChanged = false;
	pPathPF = "pack.pf";
	pPathHF = "pack.hf";
	pContentPathPrefix = "Data/";
	pVersion = 1;
	pGameType = egtEnglish;
}

dmopmPackage::~dmopmPackage(){
	RemoveAllEntries();
}



// Management
///////////////

void dmopmPackage::SetFilename( const FXString &filename ){
	pFilename = filename;
}

void dmopmPackage::SetSaved( bool saved ){
	pSaved = saved;
}

void dmopmPackage::SetChanged( bool changed ){
	pChanged = changed;
}

void dmopmPackage::SetPathPF( const FXString &path ){
	pPathPF = path;
}

void dmopmPackage::SetPathHF( const FXString &path ){
	pPathHF = path;
}

void dmopmPackage::SetContentPathPrefix( const FXString &prefix ){
	pContentPathPrefix = prefix;
}

void dmopmPackage::SetVersion( int version ){
	if( version < 1 ){
		version = 1;
	}
	
	pVersion = version;
}

void dmopmPackage::SetGameType( int gameType ){
	pGameType = gameType;
}



int dmopmPackage::GetEntryCount() const{
	return pEntries.size();
}

dmopmPackageEntry *dmopmPackage::GetEntryAt( int index ) const{
	if( index < 0 || index >= ( int )pEntries.size() ){
		THROW( dueInvalidParam );
	}
	
	return pEntries.at( index );
}

dmopmPackageEntry *dmopmPackage::GetEntryWithID( unsigned int id ) const{
	std::vector<dmopmPackageEntry*>::const_iterator iter = std::find_if( pEntries.begin(), pEntries.end(), fEntryEqualID( id ) );
	return ( iter == pEntries.end() ) ? NULL : *iter;
}

void dmopmPackage::AddEntry( dmopmPackageEntry *entry ){
	if( ! entry || GetEntryWithID( entry->GetID() ) ){
		THROW( dueInvalidParam );
	}
	
	pEntries.push_back( entry );
}

void dmopmPackage::RemoveEntry( dmopmPackageEntry *entry ){
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

void dmopmPackage::RemoveAllEntries(){
	while( ! pEntries.empty() ){
		delete pEntries.back();
		pEntries.pop_back();
	}
}
