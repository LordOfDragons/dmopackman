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

#include "dmopackman.h"
#include "gui/dmopmWindowMain.h"
#include "exceptions.h"



// Entry point
////////////////

int main( int argc, char **argv ){
	//dmopmWindowMain *windowMain;
	FXApp *app;
	
#ifdef DEBUG_RELOCATE_STDOUT
	FILE *w32ConH4xFile = freopen( "dmopackman.log", "w", stdout );
	if( ! w32ConH4xFile ) return -1;
	setvbuf( w32ConH4xFile, NULL, _IONBF, 0 );
#endif
	
	try{
		app = new FXApp( "DMO Pack Manager", "RPTD" );
		if( ! app ) throw( new FXMemoryException );
		
		app->init( argc, argv );
		
		/*windowMain = */ new dmopmWindowMain( app, argc, argv );
		
		app->create();
		app->run();
		
		// fox bug: writes before main window is destroyed so the written
		// changes are never persisted
		app->reg().write();
		
		delete app;
		
	}catch( duException e ){
#ifdef DEBUG_RELOCATE_STDOUT
		fflush( stdout );
#endif
		e.PrintError();
		return -1;
	}
	
#ifdef DEBUG_RELOCATE_STDOUT
	fflush( stdout );
#endif
	
	return 0;
}
