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
#include "dmopmDialogEditEntry.h"
#include "../package/dmopmPackage.h"
#include "../package/dmopmPackageEntry.h"
#include "../exceptions.h"



// Events
///////////

FXDEFMAP( dmopmDialogEditEntry ) dmopmDialogEditEntryMap[] = {
	FXMAPFUNC( SEL_COMMAND, dmopmDialogEditEntry::ID_BTN_ACCEPT, dmopmDialogEditEntry::onBtnAccept ),
};



// Class dmopmDialogEditEntry
///////////////////////////////

// Constructor, destructor
////////////////////////////

FXIMPLEMENT( dmopmDialogEditEntry, FXDialogBox, dmopmDialogEditEntryMap, ARRAYNUMBER( dmopmDialogEditEntryMap ) )

dmopmDialogEditEntry::dmopmDialogEditEntry(){ }

dmopmDialogEditEntry::dmopmDialogEditEntry( FXWindow *owner, dmopmWindowMain *windowMain, dmopmPackageEntry *entry ) :
FXDialogBox( owner, "Edit Entry", DECOR_TITLE | DECOR_BORDER, 0, 0, 0, 0, 10, 10, 10, 5 ){
	if( ! windowMain || ! windowMain->GetPackage() || ! entry ){
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
	pEntry = entry;
	
	// create content
	content = new FXVerticalFrame( this, LAYOUT_SIDE_TOP | LAYOUT_FILL_X | LAYOUT_FILL_Y, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20 );
	
	frameBlock = new FXMatrix( content, 2, MATRIX_BY_COLUMNS | LAYOUT_SIDE_BOTTOM | LAYOUT_FILL_X );
	
	tooltip = "Identifier of the entry";
	label = new FXLabel( frameBlock, "ID:", NULL, LABEL_NORMAL, 0, 0, 0, 0, 0, blockPadding );
	label->setTipText( tooltip );
	frameLine = new FXHorizontalFrame( frameBlock, LAYOUT_SIDE_TOP | LAYOUT_FILL_X | LAYOUT_FILL_COLUMN, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 );
	pEditID = new FXTextField( frameLine, 12, NULL, 0, FRAME_SUNKEN | TEXTFIELD_READONLY );
	pEditID->setTipText( tooltip );
	pEditID->setBackColor( getApp()->getBaseColor() );
	
	tooltip = "Offset in bytes of the entry data in the archive file";
	label = new FXLabel( frameBlock, "Offset:", NULL, LABEL_NORMAL, 0, 0, 0, 0, 0, blockPadding );
	label->setTipText( tooltip );
	frameLine = new FXHorizontalFrame( frameBlock, LAYOUT_SIDE_TOP | LAYOUT_FILL_X | LAYOUT_FILL_COLUMN, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 );
	pEditOffset = new FXTextField( frameLine, 12, NULL, 0, FRAME_SUNKEN );
	pEditOffset->setTipText( tooltip );
	
	tooltip = "Size of the entry data in bytes";
	label = new FXLabel( frameBlock, "Size:", NULL, LABEL_NORMAL, 0, 0, 0, 0, 0, blockPadding );
	label->setTipText( tooltip );
	frameLine = new FXHorizontalFrame( frameBlock, LAYOUT_SIDE_TOP | LAYOUT_FILL_X | LAYOUT_FILL_COLUMN, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 );
	pEditSize = new FXTextField( frameLine, 12, NULL, 0, FRAME_SUNKEN );
	pEditSize->setTipText( tooltip );
	
	tooltip = "Extra data of the entry data";
	label = new FXLabel( frameBlock, "Extra:", NULL, LABEL_NORMAL, 0, 0, 0, 0, 0, blockPadding );
	label->setTipText( tooltip );
	frameLine = new FXHorizontalFrame( frameBlock, LAYOUT_SIDE_TOP | LAYOUT_FILL_X | LAYOUT_FILL_COLUMN, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 );
	pEditExtra = new FXTextField( frameLine, 12, NULL, 0, FRAME_SUNKEN );
	pEditExtra->setTipText( tooltip );
	
	tooltip = "Path name of the entry or empty if unknown";
	label = new FXLabel( frameBlock, "Path:", NULL, LABEL_NORMAL, 0, 0, 0, 0, 0, blockPadding );
	label->setTipText( tooltip );
	frameLine = new FXHorizontalFrame( frameBlock, LAYOUT_SIDE_TOP | LAYOUT_FILL_X | LAYOUT_FILL_COLUMN, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 );
	pEditPath = new FXTextField( frameLine, 50, NULL, 0, FRAME_SUNKEN | LAYOUT_FILL_X );
	pEditPath->setTipText( tooltip );
	
	// buttons below
	frameButtons = new FXVerticalFrame( content, LAYOUT_SIDE_TOP | LAYOUT_FILL_X, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5 );
	
	new FXSeparator( frameButtons );
	
	frameLine = new FXHorizontalFrame( frameButtons, LAYOUT_CENTER_X, 0, 0, 0, 0, 0, 0, 0, 0, 20, 0 );
	pBtnAccept = new FXButton( frameLine, "Accept", NULL, this, ID_BTN_ACCEPT, BUTTON_DEFAULT | BUTTON_INITIAL | LAYOUT_CENTER_X | FRAME_RAISED | JUSTIFY_NORMAL | ICON_BEFORE_TEXT, 0, 0, 0, 0, 30, 30 );
	pBtnCancel = new FXButton( frameLine, "Cancel", NULL, this, ID_CANCEL, LAYOUT_CENTER_X | FRAME_RAISED | JUSTIFY_NORMAL | ICON_BEFORE_TEXT, 0, 0, 0, 0, 30, 30 );
	
	// reset values
	ResetValues();
}

dmopmDialogEditEntry::~dmopmDialogEditEntry(){
}



// Management
///////////////

void dmopmDialogEditEntry::ResetValues(){
	FXString text;
	
	text.format( "%u", pEntry->GetID() );
	pEditID->setText( text );
	
	text.format( "%u", pEntry->GetOffset() );
	pEditOffset->setText( text );
	
	text.format( "%u", pEntry->GetSize() );
	pEditSize->setText( text );
	
	text.format( "%u", pEntry->GetExtra() );
	pEditExtra->setText( text );
	
	pEditPath->setText( pEntry->GetPath() );
}

bool dmopmDialogEditEntry::ValidInput(){
	return true;
}

void dmopmDialogEditEntry::ApplyChanges(){
	pEntry->SetOffset( FXUIntVal( pEditOffset->getText() ) );
	pEntry->SetSize( FXUIntVal( pEditSize->getText() ) );
	pEntry->SetExtra( FXUIntVal( pEditExtra->getText() ) );
	pEntry->SetPath( pEditPath->getText() );
}



// Events
///////////

long dmopmDialogEditEntry::onBtnAccept( FXObject *sender, FXSelector selector, void *data ){
	if( ValidInput() ){
		ApplyChanges();
		return handle( this, FXSEL( SEL_COMMAND, ID_ACCEPT ), NULL );
	}
	
	return 0;
}
