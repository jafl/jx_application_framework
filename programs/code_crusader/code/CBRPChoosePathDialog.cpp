/******************************************************************************
 CBRPChoosePathDialog.cpp

	BASE CLASS = JXChoosePathDialog

	Copyright © 1999 by John Lindal.

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
	const JString&					fileFilter,
	const bool					selectOnlyWritable,
	const CBRelPathCSF::PathType	pathType,
	const JString&					message
	)
{
	auto* dlog =
		jnew CBRPChoosePathDialog(supervisor, dirInfo, fileFilter, selectOnlyWritable);
	assert( dlog != nullptr );
	dlog->BuildWindow(pathType, message);
	return dlog;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBRPChoosePathDialog::CBRPChoosePathDialog
	(
	JXDirector*		supervisor,
	JDirInfo*		dirInfo,
	const JString&	fileFilter,
	const bool	selectOnlyWritable
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
	const JString&					message
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 310,460, JString::empty);
	assert( window != nullptr );

	auto* openButton =
		jnew JXTextButton(JGetString("openButton::CBRPChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,280, 70,20);
	assert( openButton != nullptr );
	openButton->SetShortcuts(JGetString("openButton::CBRPChoosePathDialog::shortcuts::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::CBRPChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,310, 70,20);
	assert( cancelButton != nullptr );

	auto* homeButton =
		jnew JXTextButton(JGetString("homeButton::CBRPChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 250,140, 40,20);
	assert( homeButton != nullptr );

	auto* pathLabel =
		jnew JXStaticText(JGetString("pathLabel::CBRPChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,20, 40,20);
	assert( pathLabel != nullptr );
	pathLabel->SetToLabel();

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,140, 180,190);
	assert( scrollbarSet != nullptr );

	auto* selectButton =
		jnew JXTextButton(JGetString("selectButton::CBRPChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,250, 70,20);
	assert( selectButton != nullptr );

	auto* pathInput =
		jnew JXPathInput(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 60,20, 200,20);
	assert( pathInput != nullptr );

	auto* showHiddenCB =
		jnew JXTextCheckbox(JGetString("showHiddenCB::CBRPChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 60,80, 130,20);
	assert( showHiddenCB != nullptr );

	auto* filterLabel =
		jnew JXStaticText(JGetString("filterLabel::CBRPChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,50, 40,20);
	assert( filterLabel != nullptr );
	filterLabel->SetToLabel();

	auto* filterInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 60,50, 200,20);
	assert( filterInput != nullptr );

	auto* explanText =
		jnew JXStaticText(JGetString("explanText::CBRPChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,340, 270,20);
	assert( explanText != nullptr );
	explanText->SetToLabel();

	auto* pathHistory =
		jnew JXPathHistoryMenu(1, JString::empty, window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,20, 30,20);
	assert( pathHistory != nullptr );

	auto* filterHistory =
		jnew JXStringHistoryMenu(1, JString::empty, window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,50, 30,20);
	assert( filterHistory != nullptr );

	auto* upButton =
		jnew JXTextButton(JGetString("upButton::CBRPChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,140, 30,20);
	assert( upButton != nullptr );

	auto* newDirButton =
		jnew JXNewDirButton(window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,190, 70,20);
	assert( newDirButton != nullptr );

	itsPathTypeRG =
		jnew JXRadioGroup(window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,370, 204,84);
	assert( itsPathTypeRG != nullptr );

	auto* absolutePathRB =
		jnew JXTextRadioButton(CBRelPathCSF::kAbsolutePath, JGetString("absolutePathRB::CBRPChoosePathDialog::JXLayout"), itsPathTypeRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 180,20);
	assert( absolutePathRB != nullptr );

	auto* projectRelativeRB =
		jnew JXTextRadioButton(CBRelPathCSF::kProjectRelative, JGetString("projectRelativeRB::CBRPChoosePathDialog::JXLayout"), itsPathTypeRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,30, 180,20);
	assert( projectRelativeRB != nullptr );

	auto* homeDirRB =
		jnew JXTextRadioButton(CBRelPathCSF::kHomeRelative, JGetString("homeDirRB::CBRPChoosePathDialog::JXLayout"), itsPathTypeRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,50, 180,20);
	assert( homeDirRB != nullptr );

	auto* currPathMenu =
		jnew JXCurrentPathMenu(JString("/", JString::kNoCopy), window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,110, 180,20);
	assert( currPathMenu != nullptr );

	auto* desktopButton =
		jnew JXTextButton(JGetString("desktopButton::CBRPChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,160, 70,20);
	assert( desktopButton != nullptr );

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
