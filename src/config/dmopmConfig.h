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

#ifndef _DMOPMCONFIG_H_
#define _DMOPMCONFIG_H_

#include "../foxtoolkit.h"



/**
 * @brief Configuration.
 * @author Plüss Roland
 * @version 1.0
 * @date 2010
 */
class dmopmConfig{
private:
	FXString pPathDMO;
	bool pVerbose;
	int pGameType;
	
public:
	/** @name Constructors and Destructors */
	/*@{*/
	/** Creates a new configuration. */
	dmopmConfig();
	/** Cleans up the configuration. */
	~dmopmConfig();
	/*@}*/
	
	/** @name Management */
	/*@{*/
	/** Retrieves the path to the DMO game directory. */
	inline const FXString &GetPathDMO() const{ return pPathDMO; }
	/** Sets the path to the DMO game directory. */
	void SetPathDMO( const FXString &path );
	/** Determines if the output is verbose. */
	inline bool GetVerbose() const{ return pVerbose; }
	/** Sets if the output is verbose. */
	void SetVerbose( bool verbose );
	/** Retrieves the game type. */
	inline int GetGameType() const{ return pGameType; }
	/** Sets the game type. */
	void SetGameType( int gameType );
	/*@}*/
};

#endif
