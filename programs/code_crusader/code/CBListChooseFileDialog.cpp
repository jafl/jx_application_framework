/******************************************************************************
 CBListChooseFileDialog.cpp

	BASE CLASS = JXChooseFileDialog

	Copyright © 1998 by John Lindal.

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
	JXDirector*		supervisor,
	JDirInfo*		dirInfo,
	const JString&	fileFilter,
	const bool	allowSelectMultiple,
	const JString&	replaceListStr,
	const JString&	appendToListStr,
	const JString&	origName,
	const JString&	message
	)
{
	auto* dlog =
		jnew CBListChooseFileDialog(supervisor, dirInfo, fileFilter, allowSelectMultiple);
	assert( dlog != nullptr );
	dlog->BuildWindow(replaceListStr, appendToListStr, origName, message);
	return dlog;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBListChooseFileDialog::CBListChooseFileDialog
	(
	JXDirector*		supervisor,
	JDirInfo*		dirInfo,
	const JString&	fileFilter,
	const bool	allowSelectMultiple
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

bool
CBListChooseFileDialog::ReplaceExisting()
	const
{
	return itsAppendReplaceRG->GetSelectedItem() == kReplaceCmd;
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
CBListChooseFileDialog::BuildWindow
	(
	const JString& replaceListStr,
	const JString& appendToListStr,
	const JString& origName,
	const JString& message
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 310,420, JString::empty);
	assert( window != nullptr );

	auto* openButton =
		jnew JXTextButton(JGetString("openButton::CBListChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,280, 70,20);
	assert( openButton != nullptr );
	openButton->SetShortcuts(JGetString("openButton::CBListChooseFileDialog::shortcuts::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::CBListChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,310, 70,20);
	assert( cancelButton != nullptr );

	auto* homeButton =
		jnew JXTextButton(JGetString("homeButton::CBListChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 250,140, 40,20);
	assert( homeButton != nullptr );

	auto* pathLabel =
		jnew JXStaticText(JGetString("pathLabel::CBListChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,20, 40,20);
	assert( pathLabel != nullptr );
	pathLabel->SetToLabel();

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,140, 180,190);
	assert( scrollbarSet != nullptr );

	auto* filterLabel =
		jnew JXStaticText(JGetString("filterLabel::CBListChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,50, 40,20);
	assert( filterLabel != nullptr );
	filterLabel->SetToLabel();

	auto* showHiddenCB =
		jnew JXTextCheckbox(JGetString("showHiddenCB::CBListChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 60,80, 130,20);
	assert( showHiddenCB != nullptr );

	auto* pathInput =
		jnew JXPathInput(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 60,20, 200,20);
	assert( pathInput != nullptr );

	auto* filterInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 60,50, 200,20);
	assert( filterInput != nullptr );

	auto* pathHistory =
		jnew JXPathHistoryMenu(1, JString::empty, window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,20, 30,20);
	assert( pathHistory != nullptr );

	auto* filterHistory =
		jnew JXStringHistoryMenu(1, JString::empty, window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,50, 30,20);
	assert( filterHistory != nullptr );

	auto* upButton =
		jnew JXTextButton(JGetString("upButton::CBListChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,140, 30,20);
	assert( upButton != nullptr );

	auto* selectAllButton =
		jnew JXTextButton(JGetString("selectAllButton::CBListChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,190, 70,20);
	assert( selectAllButton != nullptr );

	itsAppendReplaceRG =
		jnew JXRadioGroup(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,340, 204,74);
	assert( itsAppendReplaceRG != nullptr );

	auto* replaceRB =
		jnew JXTextRadioButton(kReplaceCmd, JGetString("replaceRB::CBListChooseFileDialog::JXLayout"), itsAppendReplaceRG,
					JXWidget::kHElastic, JXWidget::kFixedTop, 10,10, 180,20);
	assert( replaceRB != nullptr );

	auto* appendRB =
		jnew JXTextRadioButton(kAppendCmd, JGetString("appendRB::CBListChooseFileDialog::JXLayout"), itsAppendReplaceRG,
					JXWidget::kHElastic, JXWidget::kFixedTop, 10,40, 180,20);
	assert( appendRB != nullptr );

	auto* currPathMenu =
		jnew JXCurrentPathMenu(JString("/", JString::kNoCopy), window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,110, 180,20);
	assert( currPathMenu != nullptr );

	auto* desktopButton =
		jnew JXTextButton(JGetString("desktopButton::CBListChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,160, 70,20);
	assert( desktopButton != nullptr );

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
