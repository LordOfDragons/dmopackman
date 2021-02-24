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

#ifndef _DMOPMDIALOGMAPDIRECTORY_H_
#define _DMOPMDIALOGMAPDIRECTORY_H_

#include <vector>
#include "../foxtoolkit.h"

class dmopmWindowMain;



/**
 * @brief Map Directory Dialog.
 * @author Plüss Roland
 * @version 1.0
 * @date 2011
 */
class dmopmDialogMapDirectory : public FXDialogBox{
	FXDECLARE( dmopmDialogMapDirectory )
	
protected:
	dmopmDialogMapDirectory();
	
public:
	enum eFoxIDs{
		ID_TIMER_UPDATE = FXDialogBox::ID_LAST,
		ID_BTN_CANCEL,
		ID_LAST
	};
	
private:
	dmopmWindowMain *pWindowMain;
	
	FXString pPathToMap;
	FXLabel *pLabFile;
	FXLabel *pLabStatus;
	FXProgressBar *pPBCurrent;
	FXText *pEditLog;
	FXButton *pBtnClose;
	int pState;
	std::vector<FXString> pFilesToMap;
	std::vector<FXString>::const_iterator pIterFileToMap;
	
public:
	/** @name Constructors and Destructors */
	/*@{*/
	/** Creates a new dialog. */
	dmopmDialogMapDirectory( FXWindow *owner, dmopmWindowMain *windowMain, const FXString &pathToMap );
	/** Cleans up the dialog. */
	virtual ~dmopmDialogMapDirectory();
	/*@}*/
	
	/** @name Management */
	/*@{*/
	/** Update the status information. */
	void UpdateStatus();
	/** Scan directory recursively for files and add them to the list of files to map. */
	void ScanDirectory( const FXString &path );
	/*@}*/
	
	/** @name Events */
	/*@{*/
	long onBtnCancel( FXObject *sender, FXSelector selector, void *data );
	long timerUpdate( FXObject *sender, FXSelector selector, void *data );
	/*@}*/
};

#endif
