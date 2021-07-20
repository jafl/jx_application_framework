/******************************************************************************
 CBFindFileDialog.cpp

	BASE CLASS = JXWindowDirector, JPrefObject

	Copyright © 1996-98 by John Lindal.

 ******************************************************************************/

#include "CBFindFileDialog.h"
#include "cbGlobals.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXTextCheckbox.h>
#include <JXInputField.h>
#include <JXStringHistoryMenu.h>
#include <JXStaticText.h>
#include <JXDocumentMenu.h>
#include <JXChooseSaveFile.h>
#include <JString.h>
#include <jFileUtil.h>
#include <jAssert.h>

const JSize kHistoryLength = 20;

// setup information

const JFileVersion kCurrentSetupVersion = 1;

	// version 1 stores itsStayOpenCB

/******************************************************************************
 Constructor

 ******************************************************************************/

CBFindFileDialog::CBFindFileDialog
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor),
	JPrefObject(CBGetPrefsManager(), kCBFindFilePrefID)
{
	BuildWindow();
	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBFindFileDialog::~CBFindFileDialog()
{
	// prefs written by CBDeleteGlobals()
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
CBFindFileDialog::Activate()
{
	JXWindowDirector::Activate();

	if (IsActive())
		{
		itsFileName->Focus();
		itsFileName->SelectAll();
		}
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
CBFindFileDialog::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 290,140, JString::empty);
	assert( window != nullptr );

	itsCloseButton =
		jnew JXTextButton(JGetString("itsCloseButton::CBFindFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 130,110, 60,20);
	assert( itsCloseButton != nullptr );
	itsCloseButton->SetShortcuts(JGetString("itsCloseButton::CBFindFileDialog::shortcuts::JXLayout"));

	itsFindButton =
		jnew JXTextButton(JGetString("itsFindButton::CBFindFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 210,110, 60,20);
	assert( itsFindButton != nullptr );
	itsFindButton->SetShortcuts(JGetString("itsFindButton::CBFindFileDialog::shortcuts::JXLayout"));

	itsFileName =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,40, 220,20);
	assert( itsFileName != nullptr );

	auto* nameLabel =
		jnew JXStaticText(JGetString("nameLabel::CBFindFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 80,20);
	assert( nameLabel != nullptr );
	nameLabel->SetToLabel();

	itsFileHistoryMenu =
		jnew JXStringHistoryMenu(kHistoryLength, JString::empty, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 240,40, 30,20);
	assert( itsFileHistoryMenu != nullptr );

	itsIgnoreCaseCB =
		jnew JXTextCheckbox(JGetString("itsIgnoreCaseCB::CBFindFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,70, 230,20);
	assert( itsIgnoreCaseCB != nullptr );
	itsIgnoreCaseCB->SetShortcuts(JGetString("itsIgnoreCaseCB::CBFindFileDialog::shortcuts::JXLayout"));

	itsStayOpenCB =
		jnew JXTextCheckbox(JGetString("itsStayOpenCB::CBFindFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,110, 90,20);
	assert( itsStayOpenCB != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::CBFindFileDialog"));
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();
	window->ShouldFocusWhenShow(true);

	ListenTo(itsFindButton);
	ListenTo(itsCloseButton);
	ListenTo(itsFileHistoryMenu);

	itsFileName->GetText()->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);
	ListenTo(itsFileName);

	itsIgnoreCaseCB->SetState(true);
	itsStayOpenCB->SetState(true);

	// create hidden JXDocument so Meta-# shortcuts work

	auto* fileListMenu =
		jnew JXDocumentMenu(JString::empty, window,
						   JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,-20, 10,10);
	assert( fileListMenu != nullptr );

	UpdateDisplay();
}

/******************************************************************************
 UpdateDisplay (private)

 ******************************************************************************/

void
CBFindFileDialog::UpdateDisplay()
{
	if (itsFileName->GetText()->IsEmpty())
		{
		itsFindButton->Deactivate();
		}
	else
		{
		itsFindButton->Activate();
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBFindFileDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsFindButton && message.Is(JXButton::kPushed))
		{
		if (FindFile() && !itsStayOpenCB->IsChecked())
			{
			Deactivate();
			}
		}
	else if (sender == itsCloseButton && message.Is(JXButton::kPushed))
		{
		Deactivate();
		}

	else if (sender == itsFileHistoryMenu && message.Is(JXMenu::kItemSelected))
		{
		itsFileHistoryMenu->UpdateInputField(message, itsFileName);
		itsFileName->Focus();
		}

	else if (sender == itsFileName &&
			 (message.Is(JStyledText::kTextSet) ||
			  message.Is(JStyledText::kTextChanged)))
		{
		UpdateDisplay();
		}

	else
		{
		JXWindowDirector::Receive(sender, message);
		}
}

/******************************************************************************
 FindFile (private)

 ******************************************************************************/

bool
CBFindFileDialog::FindFile()
{
	JString fileName = itsFileName->GetText()->GetText();
	fileName.TrimWhitespace();
	itsFileName->GetText()->SetText(fileName);

	JIndexRange lineRange;
	JExtractFileAndLine(itsFileName->GetText()->GetText(), &fileName,
						&(lineRange.first), &(lineRange.last));
	if (itsFileName->InputValid() &&
		CBGetApplication()->FindAndViewFile(
			fileName, lineRange,
			itsIgnoreCaseCB->IsChecked() ? JString::kIgnoreCase : JString::kCompareCase))
		{
		itsFileHistoryMenu->AddString(fileName);
		return true;
		}
	else
		{
		return false;
		}
}

/******************************************************************************
 ReadPrefs (virtual)

 ******************************************************************************/

void
CBFindFileDialog::ReadPrefs
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers > kCurrentSetupVersion)
		{
		return;
		}

	JXWindow* window = GetWindow();
	window->ReadGeometry(input);
	window->Deiconify();

	itsFileHistoryMenu->ReadSetup(input);

	bool ignoreCase;
	input >> JBoolFromString(ignoreCase);
	itsIgnoreCaseCB->SetState(ignoreCase);

	if (vers >= 1)
		{
		bool stayOpen;
		input >> JBoolFromString(stayOpen);
		itsStayOpenCB->SetState(stayOpen);
		}
}

/******************************************************************************
 WritePrefs (virtual)

 ******************************************************************************/

void
CBFindFileDialog::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentSetupVersion;

	output << ' ';
	GetWindow()->WriteGeometry(output);

	output << ' ';
	itsFileHistoryMenu->WriteSetup(output);

	output << ' ' << JBoolToString(itsIgnoreCaseCB->IsChecked())
				  << JBoolToString(itsStayOpenCB->IsChecked());
	output << ' ';
}
