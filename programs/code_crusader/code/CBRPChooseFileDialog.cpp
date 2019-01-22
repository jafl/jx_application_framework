/******************************************************************************
 CBRPChooseFileDialog.cpp

	BASE CLASS = JXChooseFileDialog

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include "CBRPChooseFileDialog.h"
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
#include <jDirUtil.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor function (static)

 ******************************************************************************/

CBRPChooseFileDialog*
CBRPChooseFileDialog::Create
	(
	JXDirector*						supervisor,
	JDirInfo*						dirInfo,
	const JString&					fileFilter,
	const JBoolean					allowSelectMultiple,
	const CBRelPathCSF::PathType	pathType,
	const JString&					origName,
	const JString&					message
	)
{
	CBRPChooseFileDialog* dlog =
		jnew CBRPChooseFileDialog(supervisor, dirInfo, fileFilter, allowSelectMultiple);
	assert( dlog != nullptr );
	dlog->BuildWindow(pathType, origName, message);
	return dlog;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBRPChooseFileDialog::CBRPChooseFileDialog
	(
	JXDirector*		supervisor,
	JDirInfo*		dirInfo,
	const JString&	fileFilter,
	const JBoolean	allowSelectMultiple
	)
	:
	JXChooseFileDialog(supervisor, dirInfo, fileFilter, allowSelectMultiple)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBRPChooseFileDialog::~CBRPChooseFileDialog()
{
}

/******************************************************************************
 GetPathType

 ******************************************************************************/

CBRelPathCSF::PathType
CBRPChooseFileDialog::GetPathType()
	const
{
	return (CBRelPathCSF::PathType) itsPathTypeRG->GetSelectedItem();
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
CBRPChooseFileDialog::BuildWindow
	(
	const CBRelPathCSF::PathType	pathType,
	const JString&					origName,
	const JString&					message
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 310,430, JString::empty);
	assert( window != nullptr );

	JXTextButton* openButton =
		jnew JXTextButton(JGetString("openButton::CBRPChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,280, 70,20);
	assert( openButton != nullptr );
	openButton->SetShortcuts(JGetString("openButton::CBRPChooseFileDialog::shortcuts::JXLayout"));

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::CBRPChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,310, 70,20);
	assert( cancelButton != nullptr );

	JXTextButton* homeButton =
		jnew JXTextButton(JGetString("homeButton::CBRPChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 250,140, 40,20);
	assert( homeButton != nullptr );

	JXStaticText* pathLabel =
		jnew JXStaticText(JGetString("pathLabel::CBRPChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,20, 40,20);
	assert( pathLabel != nullptr );
	pathLabel->SetToLabel();

	JXScrollbarSet* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,140, 180,190);
	assert( scrollbarSet != nullptr );

	JXStaticText* filterLabel =
		jnew JXStaticText(JGetString("filterLabel::CBRPChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,50, 40,20);
	assert( filterLabel != nullptr );
	filterLabel->SetToLabel();

	JXTextCheckbox* showHiddenCB =
		jnew JXTextCheckbox(JGetString("showHiddenCB::CBRPChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 60,80, 130,20);
	assert( showHiddenCB != nullptr );

	JXPathInput* pathInput =
		jnew JXPathInput(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 60,20, 200,20);
	assert( pathInput != nullptr );

	JXInputField* filterInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 60,50, 200,20);
	assert( filterInput != nullptr );

	JXPathHistoryMenu* pathHistory =
		jnew JXPathHistoryMenu(1, JString::empty, window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,20, 30,20);
	assert( pathHistory != nullptr );

	JXStringHistoryMenu* filterHistory =
		jnew JXStringHistoryMenu(1, JString::empty, window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,50, 30,20);
	assert( filterHistory != nullptr );

	JXTextButton* upButton =
		jnew JXTextButton(JGetString("upButton::CBRPChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,140, 30,20);
	assert( upButton != nullptr );

	JXTextButton* selectAllButton =
		jnew JXTextButton(JGetString("selectAllButton::CBRPChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,190, 70,20);
	assert( selectAllButton != nullptr );

	itsPathTypeRG =
		jnew JXRadioGroup(window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,340, 204,84);
	assert( itsPathTypeRG != nullptr );

	JXTextRadioButton* absolutePathRB =
		jnew JXTextRadioButton(CBRelPathCSF::kAbsolutePath, JGetString("absolutePathRB::CBRPChooseFileDialog::JXLayout"), itsPathTypeRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 180,20);
	assert( absolutePathRB != nullptr );

	JXTextRadioButton* projectRelativeRB =
		jnew JXTextRadioButton(CBRelPathCSF::kProjectRelative, JGetString("projectRelativeRB::CBRPChooseFileDialog::JXLayout"), itsPathTypeRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,30, 180,20);
	assert( projectRelativeRB != nullptr );

	JXTextRadioButton* homeDirRB =
		jnew JXTextRadioButton(CBRelPathCSF::kHomeRelative, JGetString("homeDirRB::CBRPChooseFileDialog::JXLayout"), itsPathTypeRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,50, 180,20);
	assert( homeDirRB != nullptr );

	JXCurrentPathMenu* currPathMenu =
		jnew JXCurrentPathMenu(JString("/", kJFalse), window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,110, 180,20);
	assert( currPathMenu != nullptr );

	JXTextButton* desktopButton =
		jnew JXTextButton(JGetString("desktopButton::CBRPChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,160, 70,20);
	assert( desktopButton != nullptr );

// end JXLayout

	SetObjects(scrollbarSet, pathLabel, pathInput, pathHistory,
			   filterLabel, filterInput, filterHistory,
			   openButton, cancelButton, upButton, homeButton,
			   desktopButton, selectAllButton,
			   showHiddenCB, currPathMenu, origName, message);

	itsPathTypeRG->SelectItem(pathType);

	JString homeDir;
	if (!JGetHomeDirectory(&homeDir))
		{
		homeDirRB->Deactivate();
		}
}
