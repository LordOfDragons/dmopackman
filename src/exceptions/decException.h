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
#ifndef _DUEXCEPTION_H_
#define _DUEXCEPTION_H_



/**
 * @brief Exception handling classes.
 *
 * @file decException.h
 * @author Plüss Roland
 * @version 1.0
 * @date 2008
 */



/**
 * @brief Exception class.
 *
 * @author Plüss Roland
 * @version 1.0
 * @date 2008
 */
class duException{
private:
	const char *p_Name; // name of exception
	const char *p_Desc; // description of exception
	const char *p_Info; // additional info about the exception
	const char *p_File; // name of source file where the exception occured
	int p_Line; // number of the line where the exception occured
public:
	/** @name Constructors */
	/*@{*/
	/**
	 * Creates a new exception object.
	 * @param name Unique name to identify the exception
	 * @param description Description of the reason for the exception
	 * @param info Additional informations helpfull to track the reason for the exception
	 * @param file Name of the source file where the exception occured
	 * @param line Line number in 'file' where the exception occured
	 */
	duException( const char *name, const char *description, const char *info, const char *file, int line );
	/*@}*/
	
	/** @name Accessors */
	/*@{*/
	inline const char *GetName() const{ return p_Name; }
	inline const char *GetDescription() const{ return p_Desc; }
	inline const char *GetInfo() const{ return p_Info; }
	inline const char *GetFile() const{ return p_File; }
	inline int GetLine() const{ return p_Line; }
	/*@}*/
	
	/** @name Tests */
	/*@{*/
	/** Tests if the exception has the given name. */
	bool IsNamed( const char *name ) const;
	/*@}*/
	
	/** @name Display Functions */
	/*@{*/
	/** Displays a formated output of the stored informations. */
	void PrintError();
	/*@}*/
};



/**
 * @brief Generic Exception.
 *
 * @author Plüss Roland
 * @version 1.0
 * @date 2008
 */
class dueGeneric : public duException{
public:
	inline dueGeneric( const char *info, const char *file, int line ) :
	duException( "Generic" ,"An Exception has occured", info, file, line ){}
};

/**
 * @brief Invalid Parameter Exception.
 *
 * @author Plüss Roland
 * @version 1.0
 * @date 2008
 */
class dueInvalidParam : public duException{
public:
	inline dueInvalidParam( const char *info, const char *file, int line ) :
	duException( "InvalidParam" ,"Invalid Parameter specified", info, file, line ){}
};

/**
 * @brief Out of Memory Exception.
 *
 * @author Plüss Roland
 * @version 1.0
 * @date 2008
 */
class dueOutOfMemory : public duException{
public:
	inline dueOutOfMemory( const char *info, const char *file, int line ) :
	duException( "OutOfMemory" ,"There is not enough Memory left", info, file, line ){}
};

/**
 * @brief Out of Boundary Exception.
 *
 * @author Plüss Roland
 * @version 1.0
 * @date 2008
 */
class dueOutOfBoundary : public duException{
public:
	inline dueOutOfBoundary( const char *info, const char *file, int line ) :
	duException( "OutOfBoundary" ,"Index is outside allowed boundaries", info, file, line ){}
};

/**
 * @brief Empty Stack Exception.
 *
 * @author Plüss Roland
 * @version 1.0
 * @date 2008
 */
class dueStackEmpty : public duException{
public:
	inline dueStackEmpty( const char *info, const char *file, int line ) :
	duException( "StackEmpty" ,"Stack is empty", info, file, line ){}
};

/**
 * @brief Stack Overflow Exception.
 *
 * @author Plüss Roland
 * @version 1.0
 * @date 2008
 */
class dueStackOverflow : public duException{
public:
	inline dueStackOverflow( const char *info, const char *file, int line ) :
	duException( "StackOverflow" ,"Stack Overflow", info, file, line ){}
};

/**
 * @brief Division by Zero Exception.
 *
 * @author Plüss Roland
 * @version 1.0
 * @date 2008
 */
class dueDivisionByZero : public duException{
public:
	inline dueDivisionByZero( const char *info, const char *file, int line ) :
	duException( "DivisionByZero" ,"Division By Zero", info, file, line ){}
};

/**
 * @brief Null Pointer Exception.
 *
 * @author Plüss Roland
 * @version 1.0
 * @date 2008
 */
class dueNullPointer : public duException{
public:
	inline dueNullPointer( const char *info, const char *file, int line ) :
	duException( "NullPointer" ,"Null Pointer", info, file, line ){}
};

/**
 * @brief Invalid Action Exception.
 *
 * @author Plüss Roland
 * @version 1.0
 * @date 2008
 */
class dueInvalidAction : public duException{
public:
	inline dueInvalidAction( const char *info, const char *file, int line ) :
	duException( "InvalidAction" ,"Invalid Action (internal error)", info, file, line ){}
};



/**
 * @brief File not found Exception.
 *
 * @author Plüss Roland
 * @version 1.0
 * @date 2008
 */
class dueFileNotFound : public duException{
public:
	inline dueFileNotFound( const char *info, const char *file, int line ) :
	duException( "FileNotFound" ,"File does not exist", info, file, line ){}
};

/**
 * @brief File does not exist Exception.
 *
 * @author Plüss Roland
 * @version 1.0
 * @date 2008
 */
class dueFileExists : public duException{
public:
	inline dueFileExists( const char *info, const char *file, int line ) :
	duException( "FileExists" ,"File does exist already", info, file, line ){}
};

/**
 * @brief File can not be opened Exception.
 *
 * @author Plüss Roland
 * @version 1.0
 * @date 2008
 */
class dueOpenFile : public duException{
public:
	inline dueOpenFile( const char *info, const char *file, int line ) :
	duException( "OpenFileFailed" ,"Open File failed", info, file, line ){}
};

/**
 * @brief File can not be read Exception.
 *
 * @author Plüss Roland
 * @version 1.0
 * @date 2008
 */
class dueReadFile : public duException{
public:
	inline dueReadFile( const char *info, const char *file, int line ) :
	duException( "ReadFileFailed" ,"Can not read from file", info, file, line ){}
};

/**
 * @brief File can not be written Exception.
 *
 * @author Plüss Roland
 * @version 1.0
 * @date 2008
 */
class dueWriteFile : public duException{
public:
	inline dueWriteFile( const char *info, const char *file, int line ) :
	duException( "WriteFileFailed" ,"Can not write to file", info, file, line ){}
};

/**
 * @brief Invalid File Format Exception.
 *
 * @author Plüss Roland
 * @version 1.0
 * @date 2008
 */
class dueInvalidFileFormat : public duException{
public:
	inline dueInvalidFileFormat( const char *info, const char *file, int line ) :
	duException( "InvalidFileFormat" ,"Invalid File Format", info, file, line ){}
};



/**
 * @brief Test failed Exception.
 *
 * @author Plüss Roland
 * @version 1.0
 * @date 2008
 */
class dueTestFailed : public duException{
public:
	inline dueTestFailed( const char *info, const char *file, int line ) :
	duException( "TestCaseFailed" ,"Test Case failed", info, file, line ){}
};

/**
 * @brief Assertion Exception.
 *
 * @author Plüss Roland
 * @version 1.0
 * @date 2008
 */
class dueAssertion : public duException{
public:
	inline dueAssertion( const char *info, const char *file, int line ) :
	duException( "AssertionException" ,"Assertion exception has occured", info, file, line ){}
};



/**
 * Throws an exception of the given type. The type specified has to be
 * an exception class subclassing duException. The file and line
 * informations are obtained from the location the macro is expanded.
 * @param cls Class name of the exception to throw
 */
#define THROW( cls ) throw cls( "", __FILE__, __LINE__ )
/**
 * Throws an exception of the given type with additional informations.
 * The type specified has to be an exception class subclassing
 * duException. The file and line informations are obtained from the
 * location the macro is expanded. In additiona the information field
 * of the exception is assigned the constant string 'info'.
 * @param cls Class name of the exception to throw
 * @param info Constant string to assign to the exception object as
 *             additional informations
 */
#define THROW_INFO( cls, info ) throw cls( info, __FILE__, __LINE__ )



// end of include only once
#endif

