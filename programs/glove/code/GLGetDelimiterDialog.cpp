/******************************************************************************
 GLGetDelimiterDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "GLGetDelimiterDialog.h"
#include "GLPlotApp.h"
#include "GLPrefsMgr.h"
#include "GLGlobals.h"

#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXTextRadioButton.h>
#include <JXTextCheckbox.h>
#include <JXIntegerInput.h>
#include <JXTextMenu.h>
#include <JXRadioGroup.h>
#include <JXStaticText.h>
#include <JXScrollbarSet.h>
#include <JXColorManager.h>
#include <JXInputField.h>

#include <jStreamUtil.h>
#include <jFStreamUtil.h>
#include <jAssert.h>

const JSize kFileByteCount 	= 1000;
const JCharacter kSpaceChar	= 0xb7;
const JCharacter kTabChar	= 0xbb;

const JIndex kDelimiterPrefsVersionID = 1;

/******************************************************************************
 Constructor

 ******************************************************************************/

GLGetDelimiterDialog::GLGetDelimiterDialog
	(
	JXWindowDirector* supervisor,
	const JString& 			text
	)
	:
	JXDialogDirector(supervisor, kJTrue),
	JPrefObject(GetPrefsMgr(), kDelimiterPrefsID)
{
	BuildWindow();
	JString filteredText(text);
	JIndex findex = 1;
	while (filteredText.LocateNextSubstring(" ", &findex))
		{
		filteredText.SetCharacter(findex, kSpaceChar);
		}
	JArray<JIndex> tabs;
	findex = 1;
	while (filteredText.LocateNextSubstring("\t", &findex))
		{
		filteredText.SetCharacter(findex, kTabChar);
		tabs.AppendElement(findex);
		}
	itsFileText->SetText(filteredText);
	JSize count = tabs.GetElementCount();
	for (JSize i = 1; i <= count; i++)
		{
		findex = tabs.GetElement(i);
		itsFileText->JTextEditor::SetFontName(findex, findex, JGetGreekFontName(), kJTrue);
		}
	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GLGetDelimiterDialog::~GLGetDelimiterDialog()
{
	JPrefObject::WritePrefs();
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
GLGetDelimiterDialog::BuildWindow()
{

	JXTextRadioButton* rb[4];

// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 330,360, "");
	assert( window != nullptr );

	JXScrollbarSet* scollbarSet =
		jnew JXScrollbarSet( window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,220, 310,100);
	assert( scollbarSet != nullptr );

	JXTextButton* okButton =
		jnew JXTextButton("OK", window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 210,330, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts("^M");

	JXTextButton* cancelButton =
		jnew JXTextButton("Cancel", window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 50,330, 70,20);
	assert( cancelButton != nullptr );
	cancelButton->SetShortcuts("^[");

	itsRG =
		jnew JXRadioGroup(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 215,110);
	assert( itsRG != nullptr );

	rb[0] =
		jnew JXTextRadioButton(1, "White space", itsRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,10, 120,20);
	assert( rb[0] != nullptr );

	rb[1] =
		jnew JXTextRadioButton(2, "Space", itsRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,30, 120,20);
	assert( rb[1] != nullptr );

	rb[2] =
		jnew JXTextRadioButton(3, "Tab", itsRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,50, 120,20);
	assert( rb[2] != nullptr );

	rb[3] =
		jnew JXTextRadioButton(4, "Character", itsRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,70, 100,20);
	assert( rb[3] != nullptr );

	itsCharInput =
		jnew JXInputField(itsRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 125,70, 40,20);
	assert( itsCharInput != nullptr );

	JXStaticText* helplabel =
		jnew JXStaticText("", window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,200, 310,20);
	assert( helplabel != nullptr );

	itsSkipCB =
		jnew JXTextCheckbox("Skip first", window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,140, 80,20);
	assert( itsSkipCB != nullptr );

	itsCommentCB =
		jnew JXTextCheckbox("Skip lines beginning with", window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,160, 170,20);
	assert( itsCommentCB != nullptr );

	itsSkipCountInput =
		jnew JXIntegerInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 90,140, 40,20);
	assert( itsSkipCountInput != nullptr );

	JXStaticText* obj1 =
		jnew JXStaticText("lines.", window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 135,143, 60,20);
	assert( obj1 != nullptr );

	itsCommentInput =
		jnew JXInputField(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 180,160, 50,20);
	assert( itsCommentInput != nullptr );
	itsCommentInput->SetFontSize(10);

// end JXLayout

	window->SetTitle("Choose Delimiter");
	window->PlaceAsDialogWindow();
	UseModalPlacement(kJTrue);
	SetButtons(okButton, cancelButton);

	itsFileText =
		jnew JXStaticText("", kJFalse, kJFalse,
			scollbarSet, scollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic, 10,60, 310,90);
	assert(itsFileText != nullptr);
	itsFileText->FitToEnclosure();

	itsCharInput->Deactivate();
	itsCharInput->SetMaxLength(1);

	itsSkipCountInput->Deactivate();
	itsCommentInput->Deactivate();

	JString helpText("In the text below, \'a\' = tab and \'b\' = space.");
	helpText.SetCharacter(21, kTabChar);
	helpText.SetCharacter(35, kSpaceChar);
	helplabel->SetText(helpText);
	helplabel->JTextEditor::SetFontName(21, 21, JGetGreekFontName(), kJTrue);

	ListenTo(itsRG);
	ListenTo(itsSkipCB);
	ListenTo(itsCommentCB);

	itsCommentInput->SetText("#");
}

/******************************************************************************
 Receive

 ******************************************************************************/

void
GLGetDelimiterDialog::Receive
	(
	JBroadcaster* sender,
	const Message& message
	)
{
	if (sender == itsRG && message.Is(JXRadioGroup::kSelectionChanged))
		{
		JIndex showIndex = itsRG->GetSelectedItem();
		if (showIndex == kChar)
			{
			itsCharInput->Activate();
			}
		else
			{
			itsCharInput->Deactivate();
			}
		}
	else if (sender == itsSkipCB && message.Is(JXCheckbox::kPushed))
		{
		if (itsSkipCB->IsChecked())
			{
			itsSkipCountInput->Activate();
			}
		else
			{
			itsSkipCountInput->Deactivate();
			}
		}
	else if (sender == itsCommentCB && message.Is(JXCheckbox::kPushed))
		{
		if (itsCommentCB->IsChecked())
			{
			itsCommentInput->Activate();
			}
		else
			{
			itsCommentInput->Deactivate();
			}
		}
	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}

/******************************************************************************
 GetFilterIndex

 ******************************************************************************/

GLGetDelimiterDialog::DelimiterType
GLGetDelimiterDialog::GetDelimiterType()
{
	return (GLGetDelimiterDialog::DelimiterType) itsRG->GetSelectedItem();
}

/******************************************************************************
 GetFileText

 ******************************************************************************/

JUtf8Byte
GLGetDelimiterDialog::GetCharacter()
{
	assert(itsRG->GetSelectedItem() == kChar);
	assert(itsCharInput->GetTextLength() > 0);
	const JString& temp = itsCharInput->GetText();
	return temp.GetRawBytes()[0];
}

/******************************************************************************
 IsSkippingLines

 ******************************************************************************/

JBoolean
GLGetDelimiterDialog::IsSkippingLines()
{
	return itsSkipCB->IsChecked();
}

/******************************************************************************
 GetSkipLineCount

 ******************************************************************************/

JSize
GLGetDelimiterDialog::GetSkipLineCount()
{
	JInteger value;
	if (itsSkipCountInput->GetValue(&value))
		{
		return value;
		}
	return 0;
}

/******************************************************************************
 HasComments

 ******************************************************************************/

JBoolean
GLGetDelimiterDialog::HasComments()
{
	return itsCommentCB->IsChecked();
}

/******************************************************************************
 GetCommentString

 ******************************************************************************/

const JString&
GLGetDelimiterDialog::GetCommentString()
{
	return itsCommentInput->GetText();
}

/******************************************************************************
 ReadPrefs (protected)

 ******************************************************************************/

void
GLGetDelimiterDialog::ReadPrefs
	(
	std::istream& input
	)
{
	JIndex id;
	input >> id;
	if (id <= kDelimiterPrefsVersionID)
		{
		input >> id;
		itsRG->SelectItem(id);
		JString str;
		input >> str;
		itsCharInput->SetText(str);
		JBoolean checked;
		input >> checked;
		itsSkipCB->SetState(checked);
		JBoolean ok;
		input >> ok;
		if (ok)
			{
			JInteger value;
			input >> value;
			itsSkipCountInput->SetValue(value);
			}
		input >> checked;
		itsCommentCB->SetState(checked);
		input >> str;
		itsCommentInput->SetText(str);
		JPoint loc;
		input >> loc;
		JCoordinate w;
		JCoordinate h;
		input >> w;
		input >> h;
		GetWindow()->SetSize(w, h);
		GetWindow()->Place(loc.x, loc.y);
		}
}

/******************************************************************************
 WritePrefs (protected)

 ******************************************************************************/

void
GLGetDelimiterDialog::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kDelimiterPrefsVersionID << ' ';
	output << itsRG->GetSelectedItem() << ' ';
	output << itsCharInput->GetText() << ' ';
	output << itsSkipCB->IsChecked() << ' ';
	JInteger value;
	JBoolean ok = itsSkipCountInput->GetValue(&value);
	output << ok << ' ';
	if (ok)
		{
		output << value << ' ';
		}
	output << itsCommentCB->IsChecked() << ' ';
	output << itsCommentInput->GetText() << ' ';
	output << GetWindow()->GetDesktopLocation() << ' ';
	output << GetWindow()->GetFrameWidth() << ' ';
	output << GetWindow()->GetFrameHeight() << ' ';
}
