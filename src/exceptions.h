/* 
 * Drag[en]gine Library -- Game Engine
 *
 * Copyright (C) 2008, Plüss Roland ( roland@rptd.ch )
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

// include only once
#ifndef _EXCEPTIONS_H_
#define _EXCEPTIONS_H_

// includes
#include "exceptions/decException.h"



/**
 * @brief Dragengine Invalid File Format Exception.
 *
 * @author Plüss Roland
 * @version 1.0
 * @date 2008
 */
class deeInvalidFormat : public duException{
public:
	inline deeInvalidFormat( const char *info, const char *file, int line ) :
	duException( "InvalidFormat", "Invalid File Format", info, file, line ){}
};

/**
 * @brief Dragengine Directory not found Exception.
 *
 * @author Plüss Roland
 * @version 1.0
 * @date 2008
 */
class deeDirectoryNotFound : public duException{
public:
	inline deeDirectoryNotFound( const char *info, const char *file, int line ) :
	duException( "DirectoryNotFound", "Directory does not exist", info, file, line ){}
};

/**
 * @brief Dragengine Can not read directory Exception.
 *
 * @author Plüss Roland
 * @version 1.0
 * @date 2008
 */
class deeDirectoryRead : public duException{
public:
	inline deeDirectoryRead( const char *info, const char *file, int line ) :
	duException( "DirectoryReadFailed", "Directory read error", info, file, line ){}
};

/**
 * @brief Dragengine Invalid Syntax Exception.
 *
 * @author Plüss Roland
 * @version 1.0
 * @date 2008
 */
class deeInvalidSyntax : public duException{
public:
	inline deeInvalidSyntax( const char *info, const char *file, int line ) :
	duException( "InvalidSyntax", "Invalid Script Syntax", info, file, line ){}
};

/**
 * @brief Dragengine Script Error Exception.
 *
 * @author Plüss Roland
 * @version 1.0
 * @date 2008
 */
class deeScriptError : public duException{
public:
	inline deeScriptError( const char *info, const char *file, int line ) :
	duException( "ScriptError", "Error in Script System", info, file, line ){}
};

/**
 * @brief Dragengine No Module Found Exception.
 *
 * @author Plüss Roland
 * @version 1.0
 * @date 2008
 */
class deeNoModuleFound : public duException{
public:
	inline deeNoModuleFound( const char *info, const char *file, int line ) :
	duException( "NoModuleFound", "No Working Module could be found", info, file, line ){}
};

// end of include only once
#endif
