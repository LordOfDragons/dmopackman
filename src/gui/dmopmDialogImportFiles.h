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

#ifndef _DMOPMDIALOGIMPORTFILES_H_
#define _DMOPMDIALOGIMPORTFILES_H_

#include "../foxtoolkit.h"

class dmopmWindowMain;
class dmopmZipReader;
class dmopmPackage;



/**
 * @brief Import Files Dialog.
 * @author Plüss Roland
 * @version 1.0
 * @date 2011
 */
class dmopmDialogImportFiles : public FXDialogBox{
	FXDECLARE( dmopmDialogImportFiles )
	
protected:
	dmopmDialogImportFiles();
	
public:
	enum eFoxIDs{
		ID_TIMER_UPDATE = FXDialogBox::ID_LAST,
		ID_BTN_CANCEL,
		ID_LAST
	};
	
private:
	dmopmWindowMain *pWindowMain;
	
	FXString pPathImportZIP;
	dmopmZipReader *pImportZIP;
	int pPatchedFilesCount;
	
	FXLabel *pLabStatus;
	FXProgressBar *pPBOverall;
	FXButton *pBtnCancel;
	FXText *pEditLog;
	int pState;
	int pNextEntry;
	FXFileStream *pStreamFilemap;
	FXFileStream *pStreamArchive;
	FXString pPadding;
	
public:
	/** @name Constructors and Destructors */
	/*@{*/
	/** Creates a new dialog. */
	dmopmDialogImportFiles( FXWindow *owner, dmopmWindowMain *windowMain );
	/** Cleans up the dialog. */
	virtual ~dmopmDialogImportFiles();
	/*@}*/
	
	/** @name Management */
	/*@{*/
	/** Sets path to zip file to import. */
	void SetPathImportZIP( const FXString &path );
	
	/** Update the status information. */
	void UpdateStatus();
	/*@}*/
	
	/** @name Events */
	/*@{*/
	long onBtnCancel( FXObject *sender, FXSelector selector, void *data );
	long timerUpdate( FXObject *sender, FXSelector selector, void *data );
	/*@}*/
};

#endif
