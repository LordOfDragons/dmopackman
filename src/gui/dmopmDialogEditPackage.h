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

#ifndef _DMOPMDIALOGEDITPACKAGE_H_
#define _DMOPMDIALOGEDITPACKAGE_H_

#include "../foxtoolkit.h"

class dmopmWindowMain;



/**
 * @brief Edit Package Dialog.
 * @author Plüss Roland
 * @version 1.0
 * @date 2011
 */
class dmopmDialogEditPackage : public FXDialogBox{
	FXDECLARE( dmopmDialogEditPackage )
	
protected:
	dmopmDialogEditPackage();
	
public:
	enum eFoxIDs{
		ID_LAST = FXDialogBox::ID_LAST,
		ID_BTN_PATHPF,
		ID_BTN_PATHHF,
		ID_BTN_ACCEPT
	};
	
private:
	dmopmWindowMain *pWindowMain;
	
	FXTextField *pEditPathPF;
	FXButton *pBtnPathPF;
	FXTextField *pEditPathHF;
	FXButton *pBtnPathHF;
	FXTextField *pEditContentPathPrefix;
	FXTextField *pEditVersion;
	FXComboBox *pCBGameType;
	
	FXButton *pBtnAccept;
	FXButton *pBtnCancel;
	
public:
	/** @name Constructors and Destructors */
	/*@{*/
	/** Creates a new dialog. */
	dmopmDialogEditPackage( FXWindow *owner, dmopmWindowMain *windowMain );
	/** Cleans up the dialog. */
	virtual ~dmopmDialogEditPackage();
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
	long onBtnPathPF( FXObject *sender, FXSelector selector, void *data );
	long onBtnPathHF( FXObject *sender, FXSelector selector, void *data );
	
	long onBtnAccept( FXObject *sender, FXSelector selector, void *data );
	/*@}*/
};

#endif
