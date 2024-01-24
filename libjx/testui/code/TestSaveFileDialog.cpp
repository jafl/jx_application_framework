/******************************************************************************
 TestSaveFileDialog.cpp

	BASE CLASS = JXSaveFileDialog

	Written by John Lindal.

 ******************************************************************************/

#include "TestSaveFileDialog.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXSaveFileInput.h>
#include <jx-af/jx/JXPathInput.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jx/JXRadioGroup.h>
#include <jx-af/jx/JXTextRadioButton.h>
#include <jx-af/jx/JXPathHistoryMenu.h>
#include <jx-af/jx/JXCurrentPathMenu.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXNewDirButton.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor function (static)

 ******************************************************************************/

TestSaveFileDialog*
TestSaveFileDialog::Create
	(
	const SaveFormat	saveFormat,
	const JString&		prompt,
	const JString&		startName,
	const JString&		fileFilter,
	const JString&		message
	)
{
	TestSaveFileDialog* dlog = jnew TestSaveFileDialog(saveFormat, fileFilter);
	dlog->BuildWindow(startName, prompt, message);
	return dlog;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

TestSaveFileDialog::TestSaveFileDialog
	(
	const SaveFormat	saveFormat,
	const JString&		fileFilter
	)
	:
	JXSaveFileDialog(fileFilter),
	itsSaveFormat(saveFormat)
{
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

TestSaveFileDialog::SaveFormat
TestSaveFileDialog::GetSaveFormat()
	const
{
	return (SaveFormat) itsFormatRG->GetSelectedItem();
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
TestSaveFileDialog::BuildWindow
	(
	const JString& startName,
	const JString& prompt,
	const JString& message
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 310,390, JString::empty);

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,140, 180,140);
	assert( scrollbarSet != nullptr );

	auto* saveButton =
		jnew JXTextButton(JGetString("saveButton::TestSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,280, 70,20);
	assert( saveButton != nullptr );
	saveButton->SetShortcuts(JGetString("saveButton::TestSaveFileDialog::shortcuts::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::TestSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,310, 70,20);
	assert( cancelButton != nullptr );

	auto* homeButton =
		jnew JXTextButton(JGetString("homeButton::TestSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 250,140, 40,20);
	assert( homeButton != nullptr );

	auto* pathLabel =
		jnew JXStaticText(JGetString("pathLabel::TestSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,20, 40,20);
	assert( pathLabel != nullptr );
	pathLabel->SetToLabel();

	auto* fileNameInput =
		jnew JXSaveFileInput(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,310, 180,20);
	assert( fileNameInput != nullptr );

	auto* pathInput =
		jnew JXPathInput(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 60,20, 200,20);
	assert( pathInput != nullptr );

	auto* showHiddenCB =
		jnew JXTextCheckbox(JGetString("showHiddenCB::TestSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 60,80, 130,20);
	assert( showHiddenCB != nullptr );

	auto* promptLabel =
		jnew JXStaticText(JGetString("promptLabel::TestSaveFileDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,290, 180,20);
	assert( promptLabel != nullptr );
	promptLabel->SetToLabel();

	auto* filterLabel =
		jnew JXStaticText(JGetString("filterLabel::TestSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,50, 40,20);
	assert( filterLabel != nullptr );
	filterLabel->SetToLabel();

	auto* filterInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 60,50, 200,20);
	assert( filterInput != nullptr );

	auto* saveLabel =
		jnew JXStaticText(JGetString("saveLabel::TestSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,350, 60,20);
	assert( saveLabel != nullptr );
	saveLabel->SetToLabel();

	itsFormatRG =
		jnew JXRadioGroup(window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 90,340, 192,36);
	assert( itsFormatRG != nullptr );

	auto* gifRB =
		jnew JXTextRadioButton(kGIFFormat, JGetString("gifRB::TestSaveFileDialog::JXLayout"), itsFormatRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,5, 50,20);
	assert( gifRB != nullptr );

	auto* jpegRB =
		jnew JXTextRadioButton(kJPEGFormat, JGetString("jpegRB::TestSaveFileDialog::JXLayout"), itsFormatRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 120,5, 60,20);
	assert( jpegRB != nullptr );

	auto* pngRB =
		jnew JXTextRadioButton(kPNGFormat, JGetString("pngRB::TestSaveFileDialog::JXLayout"), itsFormatRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 60,5, 50,20);
	assert( pngRB != nullptr );

	auto* pathHistory =
		jnew JXPathHistoryMenu(1, window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,20, 30,20);
	assert( pathHistory != nullptr );

	auto* filterHistory =
		jnew JXStringHistoryMenu(1, window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,50, 30,20);
	assert( filterHistory != nullptr );

	auto* upButton =
		jnew JXTextButton(JGetString("upButton::TestSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,140, 30,20);
	assert( upButton != nullptr );

	auto* desktopButton =
		jnew JXTextButton(JGetString("desktopButton::TestSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,170, 70,20);
	assert( desktopButton != nullptr );

	auto* newDirButton =
		jnew JXNewDirButton(window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,190, 70,20);
	assert( newDirButton != nullptr );

	auto* currPathMenu =
		jnew JXCurrentPathMenu(JGetRootDirectory(), window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,110, 180,20);
	assert( currPathMenu != nullptr );

// end JXLayout

	SetObjects(scrollbarSet, promptLabel, prompt, fileNameInput,
			   pathLabel, pathInput, pathHistory,
			   filterLabel, filterInput, filterHistory,
			   saveButton, cancelButton, upButton,
			   homeButton, desktopButton, newDirButton,
			   showHiddenCB, currPathMenu, startName, message);

	ListenTo(itsFormatRG, std::function([this](const JXRadioGroup::SelectionChanged& msg)
	{
		HandleFormatChange(msg.GetID());
	}));

	itsFormatRG->SelectItem(itsSaveFormat);

	HandleFormatChange(itsFormatRG->GetSelectedItem());
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
	JString fileName            = fileNameInput->GetText()->GetText();

	if (fileName.IsEmpty())
	{
		return;
	}

	JStringIterator iter(&fileName, JStringIterator::kStartAtEnd);
	if (iter.Prev("."))
	{
		iter.RemoveAllNext();
	}
	iter.Invalidate();

	const JSize nameLength = fileName.GetCharacterCount();

	if (id == kGIFFormat)
	{
		fileName += ".gif";
	}
	else if (id == kPNGFormat)
	{
		fileName += ".png";
	}
	else
	{
		assert( id == kJPEGFormat );
		fileName += ".jpg";
	}

	fileNameInput->GetText()->SetText(fileName);
	fileNameInput->SetSelection(JCharacterRange(1, nameLength));
}
