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

#ifndef _DMOPMDIALOGEDITENTRY_H_
#define _DMOPMDIALOGEDITENTRY_H_

#include "../foxtoolkit.h"

class dmopmWindowMain;
class dmopmPackageEntry;



/**
 * @brief Edit Package Entry Dialog.
 * @author Plüss Roland
 * @version 1.0
 * @date 2011
 */
class dmopmDialogEditEntry : public FXDialogBox{
	FXDECLARE( dmopmDialogEditEntry )
	
protected:
	dmopmDialogEditEntry();
	
public:
	enum eFoxIDs{
		ID_LAST = FXDialogBox::ID_LAST,
		ID_BTN_ACCEPT
	};
	
private:
	dmopmWindowMain *pWindowMain;
	dmopmPackageEntry *pEntry;
	
	FXTextField *pEditID;
	FXTextField *pEditOffset;
	FXTextField *pEditSize;
	FXTextField *pEditExtra;
	FXTextField *pEditPath;
	
	FXButton *pBtnAccept;
	FXButton *pBtnCancel;
	
public:
	/** @name Constructors and Destructors */
	/*@{*/
	/** Creates a new dialog. */
	dmopmDialogEditEntry( FXWindow *owner, dmopmWindowMain *windowMain, dmopmPackageEntry *entry );
	/** Cleans up the dialog. */
	virtual ~dmopmDialogEditEntry();
	/*@}*/
	
	/** @name Management */
	/*@{*/
	/** Reset values. */
	void ResetValues();
	/** Verify if the input is value. */
	bool ValidInput();
	/** Apply changes to the file. */
	void ApplyChanges();
	/*@}*/
	
	/** @name Events */
	/*@{*/
	long onBtnAccept( FXObject *sender, FXSelector selector, void *data );
	/*@}*/
};

#endif
