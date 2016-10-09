/******************************************************************************
 CBFindFileDialog.cpp

	BASE CLASS = JXWindowDirector, JPrefObject

	Copyright (C) 1996-98 by John Lindal. All rights reserved.

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

	JXWindow* window = jnew JXWindow(this, 290,140, "");
	assert( window != NULL );

	itsCloseButton =
		jnew JXTextButton(JGetString("itsCloseButton::CBFindFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 130,110, 60,20);
	assert( itsCloseButton != NULL );
	itsCloseButton->SetShortcuts(JGetString("itsCloseButton::CBFindFileDialog::shortcuts::JXLayout"));

	itsFindButton =
		jnew JXTextButton(JGetString("itsFindButton::CBFindFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 210,110, 60,20);
	assert( itsFindButton != NULL );
	itsFindButton->SetShortcuts(JGetString("itsFindButton::CBFindFileDialog::shortcuts::JXLayout"));

	itsFileName =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,40, 220,20);
	assert( itsFileName != NULL );

	JXStaticText* obj1_JXLayout =
		jnew JXStaticText(JGetString("obj1_JXLayout::CBFindFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 80,20);
	assert( obj1_JXLayout != NULL );
	obj1_JXLayout->SetToLabel();

	itsFileHistoryMenu =
		jnew JXStringHistoryMenu(kHistoryLength, "", window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 240,40, 30,20);
	assert( itsFileHistoryMenu != NULL );

	itsIgnoreCaseCB =
		jnew JXTextCheckbox(JGetString("itsIgnoreCaseCB::CBFindFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,70, 230,20);
	assert( itsIgnoreCaseCB != NULL );
	itsIgnoreCaseCB->SetShortcuts(JGetString("itsIgnoreCaseCB::CBFindFileDialog::shortcuts::JXLayout"));

	itsStayOpenCB =
		jnew JXTextCheckbox(JGetString("itsStayOpenCB::CBFindFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,110, 90,20);
	assert( itsStayOpenCB != NULL );

// end JXLayout

	window->SetTitle("Find file");
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();
	window->ShouldFocusWhenShow(kJTrue);

	ListenTo(itsFindButton);
	ListenTo(itsCloseButton);
	ListenTo(itsFileHistoryMenu);

	itsFileName->ShouldBroadcastAllTextChanged(kJTrue);
	itsFileName->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);
	ListenTo(itsFileName);

	itsIgnoreCaseCB->SetState(kJTrue);
	itsStayOpenCB->SetState(kJTrue);

	// create hidden JXDocument so Meta-# shortcuts work

	JXDocumentMenu* fileListMenu =
		jnew JXDocumentMenu("", window,
						   JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,-20, 10,10);
	assert( fileListMenu != NULL );

	UpdateDisplay();
}

/******************************************************************************
 UpdateDisplay (private)

 ******************************************************************************/

void
CBFindFileDialog::UpdateDisplay()
{
	if (itsFileName->IsEmpty())
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
			 (message.Is(JTextEditor::kTextSet) ||
			  message.Is(JTextEditor::kTextChanged)))
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

JBoolean
CBFindFileDialog::FindFile()
{
	JString fileName = itsFileName->GetText();
	fileName.TrimWhitespace();
	itsFileName->SetText(fileName);

	JIndexRange lineRange;
	JExtractFileAndLine(itsFileName->GetText(), &fileName,
						&(lineRange.first), &(lineRange.last));
	if (itsFileName->InputValid() &&
		(CBGetApplication())->FindAndViewFile(fileName, lineRange,
											  !itsIgnoreCaseCB->IsChecked()))
		{
		itsFileHistoryMenu->AddString(fileName);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 ReadPrefs (virtual)

 ******************************************************************************/

void
CBFindFileDialog::ReadPrefs
	(
	istream& input
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

	JBoolean ignoreCase;
	input >> ignoreCase;
	itsIgnoreCaseCB->SetState(ignoreCase);

	if (vers >= 1)
		{
		JBoolean stayOpen;
		input >> stayOpen;
		itsStayOpenCB->SetState(stayOpen);
		}
}

/******************************************************************************
 WritePrefs (virtual)

 ******************************************************************************/

void
CBFindFileDialog::WritePrefs
	(
	ostream& output
	)
	const
{
	output << kCurrentSetupVersion;

	output << ' ';
	GetWindow()->WriteGeometry(output);

	output << ' ';
	itsFileHistoryMenu->WriteSetup(output);

	output << ' ' << itsIgnoreCaseCB->IsChecked();
	output << ' ' << itsStayOpenCB->IsChecked();
	output << ' ';
}
