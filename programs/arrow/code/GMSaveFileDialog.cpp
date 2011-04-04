/******************************************************************************
 GMSaveFileDialog.cc

	Clients must call BuildWindow() after constructing the object.

	BASE CLASS = JXSaveFileDialog

	Written by John Lindal.

 ******************************************************************************/

#include "GMSaveFileDialog.h"
#include "GMGlobals.h"

#include <JXCurrentPathMenu.h>
#include <JXNewDirButton.h>
#include <JXPathHistoryMenu.h>
#include <JXPathInput.h>
#include <JXSaveFileInput.h>
#include <JXScrollbarSet.h>
#include <JXStaticText.h>
#include <JXTextButton.h>
#include <JXTextCheckbox.h>
#include <JXWindow.h>

#include <JString.h>

#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GMSaveFileDialog::GMSaveFileDialog
	(
	JXDirector*			supervisor,
	JDirInfo*			dirInfo,
	const JCharacter*	fileFilter,
	const JCharacter*	origName,
	const JCharacter*	prompt,
	const JCharacter*	message
	)
	:
	JXSaveFileDialog(supervisor, dirInfo, fileFilter)
{
	BuildWindow(origName, prompt, message);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GMSaveFileDialog::~GMSaveFileDialog()
{
}

/******************************************************************************
 SaveHeaders

 ******************************************************************************/

JBoolean
GMSaveFileDialog::IsSavingHeaders()
	const
{
	return itsShowHeadersCB->IsChecked();
}

void
GMSaveFileDialog::ShouldSaveHeaders
	(
	const JBoolean save
	)
{
	itsShowHeadersCB->SetState(save);
}

/******************************************************************************
 BuildWindow

 ******************************************************************************/

void
GMSaveFileDialog::BuildWindow
	(
	const JCharacter*	origName,
	const JCharacter*	prompt,
	const JCharacter*	message
	)
{
// begin JXLayout

	JXWindow* window = new JXWindow(this, 310,370, "");
	assert( window != NULL );

	JXScrollbarSet* scrollbarSet =
		new JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,140, 180,140);
	assert( scrollbarSet != NULL );

	JXTextButton* saveButton =
		new JXTextButton(JGetString("saveButton::GMSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,280, 70,20);
	assert( saveButton != NULL );
	saveButton->SetShortcuts(JGetString("saveButton::GMSaveFileDialog::shortcuts::JXLayout"));

	JXTextButton* cancelButton =
		new JXTextButton(JGetString("cancelButton::GMSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,310, 70,20);
	assert( cancelButton != NULL );
	cancelButton->SetShortcuts(JGetString("cancelButton::GMSaveFileDialog::shortcuts::JXLayout"));

	JXTextButton* homeButton =
		new JXTextButton(JGetString("homeButton::GMSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,140, 40,20);
	assert( homeButton != NULL );
	homeButton->SetShortcuts(JGetString("homeButton::GMSaveFileDialog::shortcuts::JXLayout"));

	JXStaticText* pathLabel =
		new JXStaticText(JGetString("pathLabel::GMSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 40,20);
	assert( pathLabel != NULL );
	pathLabel->SetToLabel();

	JXSaveFileInput* fileNameInput =
		new JXSaveFileInput(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,310, 180,20);
	assert( fileNameInput != NULL );

	JXPathInput* pathInput =
		new JXPathInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 60,20, 200,20);
	assert( pathInput != NULL );

	JXTextCheckbox* showHiddenCB =
		new JXTextCheckbox(JGetString("showHiddenCB::GMSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 60,80, 130,20);
	assert( showHiddenCB != NULL );
	showHiddenCB->SetShortcuts(JGetString("showHiddenCB::GMSaveFileDialog::shortcuts::JXLayout"));

	JXStaticText* promptLabel =
		new JXStaticText(JGetString("promptLabel::GMSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,290, 180,20);
	assert( promptLabel != NULL );
	promptLabel->SetToLabel();

	JXStaticText* filterLabel =
		new JXStaticText(JGetString("filterLabel::GMSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,50, 40,20);
	assert( filterLabel != NULL );
	filterLabel->SetToLabel();

	JXInputField* filterInput =
		new JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 60,50, 200,20);
	assert( filterInput != NULL );

	JXPathHistoryMenu* pathHistory =
		new JXPathHistoryMenu(1, "", window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 260,20, 30,20);
	assert( pathHistory != NULL );

	JXStringHistoryMenu* filterHistory =
		new JXStringHistoryMenu(1, "", window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 260,50, 30,20);
	assert( filterHistory != NULL );

	JXTextButton* upButton =
		new JXTextButton(JGetString("upButton::GMSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 220,140, 30,20);
	assert( upButton != NULL );
	upButton->SetShortcuts(JGetString("upButton::GMSaveFileDialog::shortcuts::JXLayout"));

	JXNewDirButton* newDirButton =
		new JXNewDirButton(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 220,190, 70,20);
	assert( newDirButton != NULL );

	itsShowHeadersCB =
		new JXTextCheckbox(JGetString("itsShowHeadersCB::GMSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,340, 190,20);
	assert( itsShowHeadersCB != NULL );

	JXCurrentPathMenu* currPathMenu =
		new JXCurrentPathMenu("/", window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 30,110, 180,20);
	assert( currPathMenu != NULL );

	JXTextButton* desktopButton =
		new JXTextButton(JGetString("desktopButton::GMSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,160, 70,20);
	assert( desktopButton != NULL );

// end JXLayout

	SetObjects(scrollbarSet, promptLabel, prompt, fileNameInput, origName,
			   pathLabel, pathInput, pathHistory,
			   filterLabel, filterInput, filterHistory,
			   saveButton, cancelButton, upButton, homeButton, desktopButton,
			   newDirButton, showHiddenCB, currPathMenu, message);

	itsShowHeadersCB->SetState(kJFalse);
}
