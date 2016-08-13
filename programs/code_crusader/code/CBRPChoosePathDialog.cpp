/******************************************************************************
 CBRPChoosePathDialog.cpp

	BASE CLASS = JXChoosePathDialog

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBRPChoosePathDialog.h"
#include <JXWindow.h>
#include <JXStaticText.h>
#include <JXPathInput.h>
#include <JXNewDirButton.h>
#include <JXTextButton.h>
#include <JXTextCheckbox.h>
#include <JXRadioGroup.h>
#include <JXTextRadioButton.h>
#include <JXPathHistoryMenu.h>
#include <JXCurrentPathMenu.h>
#include <JXScrollbarSet.h>
#include <jDirUtil.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor function (static)

 ******************************************************************************/

CBRPChoosePathDialog*
CBRPChoosePathDialog::Create
	(
	JXDirector*						supervisor,
	JDirInfo*						dirInfo,
	const JCharacter*				fileFilter,
	const JBoolean					selectOnlyWritable,
	const CBRelPathCSF::PathType	pathType,
	const JCharacter*				message
	)
{
	CBRPChoosePathDialog* dlog =
		new CBRPChoosePathDialog(supervisor, dirInfo, fileFilter, selectOnlyWritable);
	assert( dlog != NULL );
	dlog->BuildWindow(pathType, message);
	return dlog;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBRPChoosePathDialog::CBRPChoosePathDialog
	(
	JXDirector*			supervisor,
	JDirInfo*			dirInfo,
	const JCharacter*	fileFilter,
	const JBoolean		selectOnlyWritable
	)
	:
	JXChoosePathDialog(supervisor, dirInfo, fileFilter, selectOnlyWritable)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBRPChoosePathDialog::~CBRPChoosePathDialog()
{
}

/******************************************************************************
 GetPathType

 ******************************************************************************/

CBRelPathCSF::PathType
CBRPChoosePathDialog::GetPathType()
	const
{
	return (CBRelPathCSF::PathType) itsPathTypeRG->GetSelectedItem();
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
CBRPChoosePathDialog::BuildWindow
	(
	const CBRelPathCSF::PathType	pathType,
	const JCharacter*				message
	)
{
// begin JXLayout

	JXWindow* window = new JXWindow(this, 310,460, "");
	assert( window != NULL );

	JXTextButton* openButton =
		new JXTextButton(JGetString("openButton::CBRPChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,280, 70,20);
	assert( openButton != NULL );
	openButton->SetShortcuts(JGetString("openButton::CBRPChoosePathDialog::shortcuts::JXLayout"));

	JXTextButton* cancelButton =
		new JXTextButton(JGetString("cancelButton::CBRPChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,310, 70,20);
	assert( cancelButton != NULL );

	JXTextButton* homeButton =
		new JXTextButton(JGetString("homeButton::CBRPChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 250,140, 40,20);
	assert( homeButton != NULL );

	JXStaticText* pathLabel =
		new JXStaticText(JGetString("pathLabel::CBRPChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,20, 40,20);
	assert( pathLabel != NULL );
	pathLabel->SetToLabel();

	JXScrollbarSet* scrollbarSet =
		new JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,140, 180,190);
	assert( scrollbarSet != NULL );

	JXTextButton* selectButton =
		new JXTextButton(JGetString("selectButton::CBRPChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,250, 70,20);
	assert( selectButton != NULL );

	JXPathInput* pathInput =
		new JXPathInput(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 60,20, 200,20);
	assert( pathInput != NULL );

	JXTextCheckbox* showHiddenCB =
		new JXTextCheckbox(JGetString("showHiddenCB::CBRPChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 60,80, 130,20);
	assert( showHiddenCB != NULL );

	JXStaticText* filterLabel =
		new JXStaticText(JGetString("filterLabel::CBRPChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,50, 40,20);
	assert( filterLabel != NULL );
	filterLabel->SetToLabel();

	JXInputField* filterInput =
		new JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 60,50, 200,20);
	assert( filterInput != NULL );

	JXStaticText* explanText =
		new JXStaticText(JGetString("explanText::CBRPChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,340, 270,20);
	assert( explanText != NULL );
	explanText->SetToLabel();

	JXPathHistoryMenu* pathHistory =
		new JXPathHistoryMenu(1, "", window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,20, 30,20);
	assert( pathHistory != NULL );

	JXStringHistoryMenu* filterHistory =
		new JXStringHistoryMenu(1, "", window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,50, 30,20);
	assert( filterHistory != NULL );

	JXTextButton* upButton =
		new JXTextButton(JGetString("upButton::CBRPChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,140, 30,20);
	assert( upButton != NULL );

	JXNewDirButton* newDirButton =
		new JXNewDirButton(window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,190, 70,20);
	assert( newDirButton != NULL );

	itsPathTypeRG =
		new JXRadioGroup(window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,370, 204,84);
	assert( itsPathTypeRG != NULL );

	JXTextRadioButton* obj1_JXLayout =
		new JXTextRadioButton(CBRelPathCSF::kAbsolutePath, JGetString("obj1_JXLayout::CBRPChoosePathDialog::JXLayout"), itsPathTypeRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 180,20);
	assert( obj1_JXLayout != NULL );

	JXTextRadioButton* obj2_JXLayout =
		new JXTextRadioButton(CBRelPathCSF::kProjectRelative, JGetString("obj2_JXLayout::CBRPChoosePathDialog::JXLayout"), itsPathTypeRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,30, 180,20);
	assert( obj2_JXLayout != NULL );

	JXTextRadioButton* homeDirRB =
		new JXTextRadioButton(CBRelPathCSF::kHomeRelative, JGetString("homeDirRB::CBRPChoosePathDialog::JXLayout"), itsPathTypeRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,50, 180,20);
	assert( homeDirRB != NULL );

	JXCurrentPathMenu* currPathMenu =
		new JXCurrentPathMenu("/", window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,110, 180,20);
	assert( currPathMenu != NULL );

	JXTextButton* desktopButton =
		new JXTextButton(JGetString("desktopButton::CBRPChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,160, 70,20);
	assert( desktopButton != NULL );

// end JXLayout

	SetObjects(scrollbarSet, pathLabel, pathInput, pathHistory,
			   filterLabel, filterInput, filterHistory,
			   openButton, selectButton, cancelButton, upButton,
			   homeButton, desktopButton,
			   newDirButton, showHiddenCB, currPathMenu, message);

	itsPathTypeRG->SelectItem(pathType);

	JString homeDir;
	if (!JGetHomeDirectory(&homeDir))
		{
		homeDirRB->Deactivate();
		}
}
