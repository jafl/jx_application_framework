/******************************************************************************
 TestSaveFileDialog.cpp

	BASE CLASS = JXSaveFileDialog

	Written by John Lindal.

 ******************************************************************************/

#include "TestSaveFileDialog.h"
#include <JXWindow.h>
#include <JXStaticText.h>
#include <JXSaveFileInput.h>
#include <JXPathInput.h>
#include <JXTextButton.h>
#include <JXTextCheckbox.h>
#include <JXRadioGroup.h>
#include <JXTextRadioButton.h>
#include <JXPathHistoryMenu.h>
#include <JXCurrentPathMenu.h>
#include <JXScrollbarSet.h>
#include <JXNewDirButton.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor function (static)

 ******************************************************************************/

TestSaveFileDialog*
TestSaveFileDialog::Create
	(
	JXDirector*								supervisor,
	JDirInfo*								dirInfo,
	const JCharacter*						fileFilter,
	const TestChooseSaveFile::SaveFormat	saveFormat,
	const JCharacter*						origName,
	const JCharacter*						prompt,
	const JCharacter*						message
	)
{
	TestSaveFileDialog* dlog =
		jnew TestSaveFileDialog(supervisor, dirInfo, fileFilter, saveFormat);
	assert( dlog != NULL );
	dlog->BuildWindow(origName, prompt, message);
	return dlog;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

TestSaveFileDialog::TestSaveFileDialog
	(
	JXDirector*								supervisor,
	JDirInfo*								dirInfo,
	const JCharacter*						fileFilter,
	const TestChooseSaveFile::SaveFormat	saveFormat
	)
	:
	JXSaveFileDialog(supervisor, dirInfo, fileFilter)
{
	itsSaveFormat = saveFormat;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TestSaveFileDialog::~TestSaveFileDialog()
{
}

/******************************************************************************
 GetSaveFormat

	itsSaveFormat is only stored for use by BuildWindow().

 ******************************************************************************/

TestChooseSaveFile::SaveFormat
TestSaveFileDialog::GetSaveFormat()
	const
{
	return (TestChooseSaveFile::SaveFormat) itsFormatRG->GetSelectedItem();
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
TestSaveFileDialog::BuildWindow
	(
	const JCharacter*	origName,
	const JCharacter*	prompt,
	const JCharacter*	message
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 310,390, JString::empty);
	assert( window != NULL );

	JXScrollbarSet* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,140, 180,140);
	assert( scrollbarSet != NULL );

	JXTextButton* saveButton =
		jnew JXTextButton(JGetString("saveButton::TestSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,280, 70,20);
	assert( saveButton != NULL );
	saveButton->SetShortcuts(JGetString("saveButton::TestSaveFileDialog::shortcuts::JXLayout"));

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::TestSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,310, 70,20);
	assert( cancelButton != NULL );

	JXTextButton* homeButton =
		jnew JXTextButton(JGetString("homeButton::TestSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 250,140, 40,20);
	assert( homeButton != NULL );

	JXStaticText* pathLabel =
		jnew JXStaticText(JGetString("pathLabel::TestSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,20, 40,20);
	assert( pathLabel != NULL );
	pathLabel->SetToLabel();

	JXSaveFileInput* fileNameInput =
		jnew JXSaveFileInput(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,310, 180,20);
	assert( fileNameInput != NULL );

	JXPathInput* pathInput =
		jnew JXPathInput(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 60,20, 200,20);
	assert( pathInput != NULL );

	JXTextCheckbox* showHiddenCB =
		jnew JXTextCheckbox(JGetString("showHiddenCB::TestSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 60,80, 130,20);
	assert( showHiddenCB != NULL );

	JXStaticText* promptLabel =
		jnew JXStaticText(JGetString("promptLabel::TestSaveFileDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,290, 180,20);
	assert( promptLabel != NULL );
	promptLabel->SetToLabel();

	JXStaticText* filterLabel =
		jnew JXStaticText(JGetString("filterLabel::TestSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,50, 40,20);
	assert( filterLabel != NULL );
	filterLabel->SetToLabel();

	JXInputField* filterInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 60,50, 200,20);
	assert( filterInput != NULL );

	JXStaticText* obj1_JXLayout =
		jnew JXStaticText(JGetString("obj1_JXLayout::TestSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,350, 60,20);
	assert( obj1_JXLayout != NULL );
	obj1_JXLayout->SetToLabel();

	itsFormatRG =
		jnew JXRadioGroup(window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 90,340, 192,36);
	assert( itsFormatRG != NULL );

	JXTextRadioButton* obj2_JXLayout =
		jnew JXTextRadioButton(TestChooseSaveFile::kGIFFormat, JGetString("obj2_JXLayout::TestSaveFileDialog::JXLayout"), itsFormatRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,5, 50,20);
	assert( obj2_JXLayout != NULL );

	JXTextRadioButton* obj3_JXLayout =
		jnew JXTextRadioButton(TestChooseSaveFile::kJPEGFormat, JGetString("obj3_JXLayout::TestSaveFileDialog::JXLayout"), itsFormatRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 120,5, 60,20);
	assert( obj3_JXLayout != NULL );

	JXTextRadioButton* obj4_JXLayout =
		jnew JXTextRadioButton(TestChooseSaveFile::kPNGFormat, JGetString("obj4_JXLayout::TestSaveFileDialog::JXLayout"), itsFormatRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 60,5, 50,20);
	assert( obj4_JXLayout != NULL );

	JXPathHistoryMenu* pathHistory =
		jnew JXPathHistoryMenu(1, JString::empty, window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,20, 30,20);
	assert( pathHistory != NULL );

	JXStringHistoryMenu* filterHistory =
		jnew JXStringHistoryMenu(1, JString::empty, window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,50, 30,20);
	assert( filterHistory != NULL );

	JXTextButton* upButton =
		jnew JXTextButton(JGetString("upButton::TestSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,140, 30,20);
	assert( upButton != NULL );

	JXTextButton* desktopButton =
		jnew JXTextButton(JGetString("desktopButton::TestSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,170, 70,20);
	assert( desktopButton != NULL );

	JXNewDirButton* newDirButton =
		jnew JXNewDirButton(window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,190, 70,20);
	assert( newDirButton != NULL );

	JXCurrentPathMenu* currPathMenu =
		jnew JXCurrentPathMenu("/", window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,110, 180,20);
	assert( currPathMenu != NULL );

// end JXLayout

	SetObjects(scrollbarSet, promptLabel, prompt, fileNameInput, origName,
			   pathLabel, pathInput, pathHistory,
			   filterLabel, filterInput, filterHistory,
			   saveButton, cancelButton, upButton,
			   homeButton, desktopButton, newDirButton,
			   showHiddenCB, currPathMenu, message);

	itsFormatRG->SelectItem(itsSaveFormat);
	ListenTo(itsFormatRG);

	HandleFormatChange(itsFormatRG->GetSelectedItem());
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
TestSaveFileDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsFormatRG && message.Is(JXRadioGroup::kSelectionChanged))
		{
		const JXRadioGroup::SelectionChanged* selection =
			dynamic_cast<const JXRadioGroup::SelectionChanged*>(&message);
		assert( selection != NULL );
		HandleFormatChange(selection->GetID());
		}

	else
		{
		JXSaveFileDialog::Receive(sender, message);
		}
}

/******************************************************************************
 HandleFormatChange (private)

 ******************************************************************************/

void
TestSaveFileDialog::HandleFormatChange
	(
	const JIndex id
	)
{
	JXInputField* fileNameInput = GetFileNameInput();
	JString fileName            = fileNameInput->GetText();
	const JSize origLength      = fileName.GetLength();

	if (origLength == 0)
		{
		return;
		}

	JIndex dotIndex;
	if (fileName.LocateLastSubstring(".", &dotIndex) && dotIndex > 1)
		{
		fileName.RemoveSubstring(dotIndex, origLength);
		}
	else
		{
		dotIndex = origLength + 1;
		}

	if (id == TestChooseSaveFile::kGIFFormat)
		{
		fileName += ".gif";
		}
	else if (id == TestChooseSaveFile::kPNGFormat)
		{
		fileName += ".png";
		}
	else
		{
		assert( id == TestChooseSaveFile::kJPEGFormat );
		fileName += ".jpg";
		}

	fileNameInput->SetText(fileName);
	fileNameInput->SetSelection(1, dotIndex-1);
}
