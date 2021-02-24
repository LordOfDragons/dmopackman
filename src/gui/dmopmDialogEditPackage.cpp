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

#include "dmopmWindowMain.h"
#include "dmopmDialogEditPackage.h"
#include "../package/dmopmPackage.h"
#include "../exceptions.h"



// Events
///////////

FXDEFMAP( dmopmDialogEditPackage ) dmopmDialogEditPackageMap[] = {
	FXMAPFUNC( SEL_COMMAND, dmopmDialogEditPackage::ID_BTN_PATHPF, dmopmDialogEditPackage::onBtnPathPF ),
	FXMAPFUNC( SEL_COMMAND, dmopmDialogEditPackage::ID_BTN_PATHHF, dmopmDialogEditPackage::onBtnPathHF ),
	FXMAPFUNC( SEL_COMMAND, dmopmDialogEditPackage::ID_BTN_ACCEPT, dmopmDialogEditPackage::onBtnAccept ),
};



// Class dmopmDialogEditPackage
/////////////////////////////////

// Constructor, destructor
////////////////////////////

FXIMPLEMENT( dmopmDialogEditPackage, FXDialogBox, dmopmDialogEditPackageMap, ARRAYNUMBER( dmopmDialogEditPackageMap ) )

dmopmDialogEditPackage::dmopmDialogEditPackage(){ }

dmopmDialogEditPackage::dmopmDialogEditPackage( FXWindow *owner, dmopmWindowMain *windowMain ) :
FXDialogBox( owner, "Edit Package", DECOR_TITLE | DECOR_BORDER, 0, 0, 0, 0, 10, 10, 10, 5 ){
	if( ! windowMain || ! windowMain->GetPackage() ){
		THROW( dueInvalidParam );
	}
	
	FXVerticalFrame *content;
	FXVerticalFrame *frameButtons;
	FXHorizontalFrame *frameLine;
	int blockPadding = 15;
	FXMatrix *frameBlock;
	const char *tooltip;
	FXLabel *label;
	
	pWindowMain = windowMain;
	
	// create content
	content = new FXVerticalFrame( this, LAYOUT_SIDE_TOP | LAYOUT_FILL_X | LAYOUT_FILL_Y, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20 );
	if( ! content ) THROW( dueOutOfMemory );
	
	frameBlock = new FXMatrix( content, 2, MATRIX_BY_COLUMNS | LAYOUT_SIDE_BOTTOM | LAYOUT_FILL_X );
	if( ! frameBlock ) THROW( dueOutOfMemory );
	
	tooltip = "Path to the archive file (*.pf) relative to the game directory";
	label = new FXLabel( frameBlock, "Archive:", NULL, LABEL_NORMAL, 0, 0, 0, 0, 0, blockPadding );
	if( ! label ) THROW( dueOutOfMemory );
	label->setTipText( tooltip );
	frameLine = new FXHorizontalFrame( frameBlock, LAYOUT_SIDE_TOP | LAYOUT_FILL_X | LAYOUT_FILL_COLUMN, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 );
	if( ! frameLine ) THROW( dueOutOfMemory );
	pEditPathPF = new FXTextField( frameLine, 30, NULL, 0, FRAME_SUNKEN | LAYOUT_FILL_X );
	if( ! pEditPathPF ) THROW( dueOutOfMemory );
	pEditPathPF->setTipText( tooltip );
	pBtnPathPF = new FXButton( frameLine, "...", NULL, this, ID_BTN_PATHPF, FRAME_RAISED | JUSTIFY_NORMAL | ICON_BEFORE_TEXT );
	if( ! pBtnPathPF ) THROW( dueOutOfMemory );
	
	tooltip = "Path to the file map file (*.hf) relative to the game directory";
	label = new FXLabel( frameBlock, "File Map:", NULL, LABEL_NORMAL, 0, 0, 0, 0, 0, blockPadding );
	if( ! label ) THROW( dueOutOfMemory );
	label->setTipText( tooltip );
	frameLine = new FXHorizontalFrame( frameBlock, LAYOUT_SIDE_TOP | LAYOUT_FILL_X | LAYOUT_FILL_COLUMN, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 );
	if( ! frameLine ) THROW( dueOutOfMemory );
	pEditPathHF = new FXTextField( frameLine, 30, NULL, 0, FRAME_SUNKEN | LAYOUT_FILL_X );
	if( ! pEditPathHF ) THROW( dueOutOfMemory );
	pEditPathHF->setTipText( tooltip );
	pBtnPathHF = new FXButton( frameLine, "...", NULL, this, ID_BTN_PATHHF, FRAME_RAISED | JUSTIFY_NORMAL | ICON_BEFORE_TEXT );
	if( ! pBtnPathHF ) THROW( dueOutOfMemory );
	
	tooltip = "Content path prefix used to recognize content for this package";
	label = new FXLabel( frameBlock, "Content Path Prefix:", NULL, LABEL_NORMAL, 0, 0, 0, 0, 0, blockPadding );
	if( ! label ) THROW( dueOutOfMemory );
	label->setTipText( tooltip );
	frameLine = new FXHorizontalFrame( frameBlock, LAYOUT_SIDE_TOP | LAYOUT_FILL_X | LAYOUT_FILL_COLUMN, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 );
	if( ! frameLine ) THROW( dueOutOfMemory );
	pEditContentPathPrefix = new FXTextField( frameLine, 30, NULL, 0, FRAME_SUNKEN | LAYOUT_FILL_X );
	if( ! pEditContentPathPrefix ) THROW( dueOutOfMemory );
	pEditContentPathPrefix->setTipText( tooltip );
	
	tooltip = "Version this package has been updated to";
	label = new FXLabel( frameBlock, "Version:", NULL, LABEL_NORMAL, 0, 0, 0, 0, 0, blockPadding );
	if( ! label ) THROW( dueOutOfMemory );
	label->setTipText( tooltip );
	frameLine = new FXHorizontalFrame( frameBlock, LAYOUT_SIDE_TOP | LAYOUT_FILL_X | LAYOUT_FILL_COLUMN, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 );
	if( ! frameLine ) THROW( dueOutOfMemory );
	pEditVersion = new FXTextField( frameLine, 4, NULL, 0, FRAME_SUNKEN );
	if( ! pEditVersion ) THROW( dueOutOfMemory );
	pEditVersion->setTipText( tooltip );
	
	tooltip = "Game type this package is designed for";
	label = new FXLabel( frameBlock, "Game Type:", NULL, LABEL_NORMAL, 0, 0, 0, 0, 0, blockPadding );
	if( ! label ) THROW( dueOutOfMemory );
	label->setTipText( tooltip );
	pCBGameType = new FXComboBox( frameBlock, 20, NULL, 0, COMBOBOX_NORMAL );
	pCBGameType->setTipText( tooltip );
	pCBGameType->setEditable( FALSE );
	pCBGameType->setNumVisible( 2 );
	pCBGameType->appendItem( "Korean (KDMO)" );
	pCBGameType->appendItem( "English (GDMO)" );
	
	
	
	// buttons below
	frameButtons = new FXVerticalFrame( content, LAYOUT_SIDE_TOP | LAYOUT_FILL_X, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5 );
	if( ! frameButtons ) THROW( dueOutOfMemory );
	
	new FXSeparator( frameButtons );
	
	frameLine = new FXHorizontalFrame( frameButtons, LAYOUT_CENTER_X, 0, 0, 0, 0, 0, 0, 0, 0, 20, 0 );
	if( ! frameLine ) THROW( dueOutOfMemory );
	pBtnAccept = new FXButton( frameLine, "Accept", NULL, this, ID_BTN_ACCEPT, BUTTON_DEFAULT | BUTTON_INITIAL | LAYOUT_CENTER_X | FRAME_RAISED | JUSTIFY_NORMAL | ICON_BEFORE_TEXT, 0, 0, 0, 0, 30, 30 );
	if( ! pBtnAccept ) THROW( dueOutOfMemory );
	pBtnCancel = new FXButton( frameLine, "Cancel", NULL, this, ID_CANCEL, LAYOUT_CENTER_X | FRAME_RAISED | JUSTIFY_NORMAL | ICON_BEFORE_TEXT, 0, 0, 0, 0, 30, 30 );
	if( ! pBtnCancel ) THROW( dueOutOfMemory );
	
	// reset values
	ResetValues();
}

dmopmDialogEditPackage::~dmopmDialogEditPackage(){
}



// Management
///////////////

void dmopmDialogEditPackage::ResetValues(){
	const dmopmPackage &package = *pWindowMain->GetPackage();
	FXString text;
	
	pEditPathPF->setText( package.GetPathPF() );
	pEditPathHF->setText( package.GetPathHF() );
	pEditContentPathPrefix->setText( package.GetContentPathPrefix() );
	text.format( "%i", package.GetVersion() );
	pEditVersion->setText( text );
	pCBGameType->setCurrentItem( package.GetGameType() );
}

bool dmopmDialogEditPackage::ValidInput(){
	const int version = FXIntVal( pEditVersion->getText() );
	
	if( version < 1 ){
		FXMessageBox::error( this, MBOX_OK, "Invalid Input", "Version can not be less than 1." );
		return false;
	}
	
	return true;
}

void dmopmDialogEditPackage::ApplyChanges(){
	dmopmPackage &package = *pWindowMain->GetPackage();
	
	package.SetPathPF( pEditPathPF->getText() );
	package.SetPathHF( pEditPathHF->getText() );
	package.SetContentPathPrefix( pEditContentPathPrefix->getText() );
	package.SetVersion( FXIntVal( pEditVersion->getText() ) );
	package.SetGameType( pCBGameType->getCurrentItem() );
}



// Events
///////////

long dmopmDialogEditPackage::onBtnPathPF( FXObject *sender, FXSelector selector, void *data ){
	return 0;
}

long dmopmDialogEditPackage::onBtnPathHF( FXObject *sender, FXSelector selector, void *data ){
	return 0;
}



long dmopmDialogEditPackage::onBtnAccept( FXObject *sender, FXSelector selector, void *data ){
	if( ValidInput() ){
		ApplyChanges();
		return handle( this, FXSEL( SEL_COMMAND, ID_ACCEPT ), NULL );
	}
	
	return 0;
}
