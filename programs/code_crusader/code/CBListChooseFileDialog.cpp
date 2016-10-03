/******************************************************************************
 CBListChooseFileDialog.cpp

	BASE CLASS = JXChooseFileDialog

	Copyright (C) 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBListChooseFileDialog.h"
#include <JXWindow.h>
#include <JXStaticText.h>
#include <JXPathInput.h>
#include <JXTextButton.h>
#include <JXTextCheckbox.h>
#include <JXRadioGroup.h>
#include <JXTextRadioButton.h>
#include <JXPathHistoryMenu.h>
#include <JXCurrentPathMenu.h>
#include <JXScrollbarSet.h>
#include <jGlobals.h>
#include <jAssert.h>

const JIndex kReplaceCmd = 1;
const JIndex kAppendCmd  = 2;

/******************************************************************************
 Constructor function (static)

 ******************************************************************************/

CBListChooseFileDialog*
CBListChooseFileDialog::Create
	(
	JXDirector*			supervisor,
	JDirInfo*			dirInfo,
	const JCharacter*	fileFilter,
	const JBoolean		allowSelectMultiple,
	const JCharacter*	replaceListStr,
	const JCharacter*	appendToListStr,
	const JCharacter*	origName,
	const JCharacter*	message
	)
{
	CBListChooseFileDialog* dlog =
		new CBListChooseFileDialog(supervisor, dirInfo, fileFilter, allowSelectMultiple);
	assert( dlog != NULL );
	dlog->BuildWindow(replaceListStr, appendToListStr, origName, message);
	return dlog;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBListChooseFileDialog::CBListChooseFileDialog
	(
	JXDirector*			supervisor,
	JDirInfo*			dirInfo,
	const JCharacter*	fileFilter,
	const JBoolean		allowSelectMultiple
	)
	:
	JXChooseFileDialog(supervisor, dirInfo, fileFilter, allowSelectMultiple)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBListChooseFileDialog::~CBListChooseFileDialog()
{
}

/******************************************************************************
 ReplaceExisting

 ******************************************************************************/

JBoolean
CBListChooseFileDialog::ReplaceExisting()
	const
{
	return JI2B( itsAppendReplaceRG->GetSelectedItem() == kReplaceCmd );
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
CBListChooseFileDialog::BuildWindow
	(
	const JCharacter* replaceListStr,
	const JCharacter* appendToListStr,
	const JCharacter* origName,
	const JCharacter* message
	)
{
// begin JXLayout

	JXWindow* window = new JXWindow(this, 310,420, "");
	assert( window != NULL );

	JXTextButton* openButton =
		new JXTextButton(JGetString("openButton::CBListChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,280, 70,20);
	assert( openButton != NULL );
	openButton->SetShortcuts(JGetString("openButton::CBListChooseFileDialog::shortcuts::JXLayout"));

	JXTextButton* cancelButton =
		new JXTextButton(JGetString("cancelButton::CBListChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,310, 70,20);
	assert( cancelButton != NULL );

	JXTextButton* homeButton =
		new JXTextButton(JGetString("homeButton::CBListChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 250,140, 40,20);
	assert( homeButton != NULL );

	JXStaticText* pathLabel =
		new JXStaticText(JGetString("pathLabel::CBListChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,20, 40,20);
	assert( pathLabel != NULL );
	pathLabel->SetToLabel();

	JXScrollbarSet* scrollbarSet =
		new JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,140, 180,190);
	assert( scrollbarSet != NULL );

	JXStaticText* filterLabel =
		new JXStaticText(JGetString("filterLabel::CBListChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,50, 40,20);
	assert( filterLabel != NULL );
	filterLabel->SetToLabel();

	JXTextCheckbox* showHiddenCB =
		new JXTextCheckbox(JGetString("showHiddenCB::CBListChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 60,80, 130,20);
	assert( showHiddenCB != NULL );

	JXPathInput* pathInput =
		new JXPathInput(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 60,20, 200,20);
	assert( pathInput != NULL );

	JXInputField* filterInput =
		new JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 60,50, 200,20);
	assert( filterInput != NULL );

	JXPathHistoryMenu* pathHistory =
		new JXPathHistoryMenu(1, "", window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,20, 30,20);
	assert( pathHistory != NULL );

	JXStringHistoryMenu* filterHistory =
		new JXStringHistoryMenu(1, "", window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,50, 30,20);
	assert( filterHistory != NULL );

	JXTextButton* upButton =
		new JXTextButton(JGetString("upButton::CBListChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,140, 30,20);
	assert( upButton != NULL );

	JXTextButton* selectAllButton =
		new JXTextButton(JGetString("selectAllButton::CBListChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,190, 70,20);
	assert( selectAllButton != NULL );

	itsAppendReplaceRG =
		new JXRadioGroup(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,340, 204,74);
	assert( itsAppendReplaceRG != NULL );

	JXTextRadioButton* replaceRB =
		new JXTextRadioButton(kReplaceCmd, JGetString("replaceRB::CBListChooseFileDialog::JXLayout"), itsAppendReplaceRG,
					JXWidget::kHElastic, JXWidget::kFixedTop, 10,10, 180,20);
	assert( replaceRB != NULL );

	JXTextRadioButton* appendRB =
		new JXTextRadioButton(kAppendCmd, JGetString("appendRB::CBListChooseFileDialog::JXLayout"), itsAppendReplaceRG,
					JXWidget::kHElastic, JXWidget::kFixedTop, 10,40, 180,20);
	assert( appendRB != NULL );

	JXCurrentPathMenu* currPathMenu =
		new JXCurrentPathMenu("/", window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,110, 180,20);
	assert( currPathMenu != NULL );

	JXTextButton* desktopButton =
		new JXTextButton(JGetString("desktopButton::CBListChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,160, 70,20);
	assert( desktopButton != NULL );

// end JXLayout

	SetObjects(scrollbarSet, pathLabel, pathInput, pathHistory,
			   filterLabel, filterInput, filterHistory,
			   openButton, cancelButton, upButton, homeButton,
			   desktopButton, selectAllButton,
			   showHiddenCB, currPathMenu, origName, message);

	itsAppendReplaceRG->SelectItem(kAppendCmd);
	replaceRB->SetLabel(replaceListStr);
	appendRB->SetLabel(appendToListStr);
}
