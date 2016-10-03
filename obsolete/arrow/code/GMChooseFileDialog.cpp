/******************************************************************************
 GMChooseFileDialog.cc

	Clients must call BuildWindow() after constructing the object.

	BASE CLASS = JXChooseFileDialog

	Copyright (C) 1999 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include "GMChooseFileDialog.h"
#include "GMGlobals.h"

#include <JXCurrentPathMenu.h>
#include <JXPathHistoryMenu.h>
#include <JXPathInput.h>
#include <JXScrollbarSet.h>
#include <JXStaticText.h>
#include <JXTextButton.h>
#include <JXTextCheckbox.h>
#include <JXWindow.h>

#include <jAssert.h>

const JIndex kReplaceCmd = 1;
const JIndex kAppendCmd  = 2;

/******************************************************************************
 Constructor

 ******************************************************************************/

GMChooseFileDialog::GMChooseFileDialog
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

GMChooseFileDialog::~GMChooseFileDialog()
{
}

/******************************************************************************
 SaveHeaders

 ******************************************************************************/

JBoolean
GMChooseFileDialog::IsSavingHeaders()
	const
{
	return itsSaveHeadersCB->IsChecked();
}

void
GMChooseFileDialog::ShouldSaveHeaders
	(
	const JBoolean save
	)
{
	itsSaveHeadersCB->SetState(save);
}

/******************************************************************************
 BuildWindow

 ******************************************************************************/

void
GMChooseFileDialog::BuildWindow
	(
	const JCharacter* origName,
	const JCharacter* message
	)
{
// begin JXLayout

	JXWindow* window = new JXWindow(this, 310,370, "");
	assert( window != NULL );

	JXTextButton* openButton =
		new JXTextButton(JGetString("openButton::GMChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,280, 70,20);
	assert( openButton != NULL );
	openButton->SetShortcuts(JGetString("openButton::GMChooseFileDialog::shortcuts::JXLayout"));

	JXTextButton* cancelButton =
		new JXTextButton(JGetString("cancelButton::GMChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,310, 70,20);
	assert( cancelButton != NULL );
	cancelButton->SetShortcuts(JGetString("cancelButton::GMChooseFileDialog::shortcuts::JXLayout"));

	JXTextButton* homeButton =
		new JXTextButton(JGetString("homeButton::GMChooseFileDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 250,140, 40,20);
	assert( homeButton != NULL );
	homeButton->SetShortcuts(JGetString("homeButton::GMChooseFileDialog::shortcuts::JXLayout"));

	JXStaticText* pathLabel =
		new JXStaticText(JGetString("pathLabel::GMChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 20,20, 40,20);
	assert( pathLabel != NULL );
	pathLabel->SetToLabel();

	JXScrollbarSet* scrollbarSet =
		new JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,140, 180,190);
	assert( scrollbarSet != NULL );

	JXStaticText* filterLabel =
		new JXStaticText(JGetString("filterLabel::GMChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 20,50, 40,20);
	assert( filterLabel != NULL );
	filterLabel->SetToLabel();

	JXTextCheckbox* showHiddenCB =
		new JXTextCheckbox(JGetString("showHiddenCB::GMChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 60,80, 130,20);
	assert( showHiddenCB != NULL );
	showHiddenCB->SetShortcuts(JGetString("showHiddenCB::GMChooseFileDialog::shortcuts::JXLayout"));

	JXPathInput* pathInput =
		new JXPathInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 60,20, 200,20);
	assert( pathInput != NULL );

	JXInputField* filterInput =
		new JXInputField(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 60,50, 200,20);
	assert( filterInput != NULL );

	JXPathHistoryMenu* pathHistory =
		new JXPathHistoryMenu(1, "", window,
					JXWidget::kHElastic, JXWidget::kVElastic, 260,20, 30,20);
	assert( pathHistory != NULL );

	JXStringHistoryMenu* filterHistory =
		new JXStringHistoryMenu(1, "", window,
					JXWidget::kHElastic, JXWidget::kVElastic, 260,50, 30,20);
	assert( filterHistory != NULL );

	JXTextButton* upButton =
		new JXTextButton(JGetString("upButton::GMChooseFileDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 220,140, 30,20);
	assert( upButton != NULL );
	upButton->SetShortcuts(JGetString("upButton::GMChooseFileDialog::shortcuts::JXLayout"));

	JXTextButton* selectAllButton =
		new JXTextButton(JGetString("selectAllButton::GMChooseFileDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 220,190, 70,20);
	assert( selectAllButton != NULL );

	itsSaveHeadersCB =
		new JXTextCheckbox(JGetString("itsSaveHeadersCB::GMChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,340, 190,20);
	assert( itsSaveHeadersCB != NULL );

	JXCurrentPathMenu* currPathMenu =
		new JXCurrentPathMenu("/", window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,110, 180,20);
	assert( currPathMenu != NULL );

	JXTextButton* desktopButton =
		new JXTextButton(JGetString("desktopButton::GMChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,160, 70,20);
	assert( desktopButton != NULL );

// end JXLayout

	SetObjects(scrollbarSet, pathLabel, pathInput, pathHistory,
			   filterLabel, filterInput, filterHistory,
			   openButton, cancelButton, upButton, homeButton, desktopButton,
			   selectAllButton, showHiddenCB, currPathMenu, origName, message);
}
