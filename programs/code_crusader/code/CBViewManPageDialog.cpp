/******************************************************************************
 CBViewManPageDialog.cpp

	BASE CLASS = JXWindowDirector, JPrefObject

	Copyright (C) 1996-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBViewManPageDialog.h"
#include "CBManPageDocument.h"
#include "cbGlobals.h"
#include <JXWindow.h>
#include <JXHelpManager.h>
#include <JXTextButton.h>
#include <JXCharInput.h>
#include <JXStaticText.h>
#include <JXTextCheckbox.h>
#include <JXStringHistoryMenu.h>
#include <JXDocumentMenu.h>
#include <JXChooseSaveFile.h>
#include <JString.h>
#include <jAssert.h>

const JSize kHistoryLength = 20;

// setup information

const JFileVersion kCurrentSetupVersion = 1;

	// version 1 stores itsStayOpenCB

/******************************************************************************
 Constructor

 ******************************************************************************/

CBViewManPageDialog::CBViewManPageDialog
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor),
	JPrefObject(CBGetPrefsManager(), kCBViewManPagePrefID)
{
	BuildWindow();
	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBViewManPageDialog::~CBViewManPageDialog()
{
	// prefs written by CBDeleteGlobals()
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
CBViewManPageDialog::Activate()
{
	JXWindowDirector::Activate();

	if (IsActive())
		{
		itsFnName->Focus();
		itsFnName->SelectAll();
		}
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
CBViewManPageDialog::BuildWindow()
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 360,150, "");
	assert( window != NULL );

	itsCloseButton =
		jnew JXTextButton(JGetString("itsCloseButton::CBViewManPageDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 120,120, 60,20);
	assert( itsCloseButton != NULL );
	itsCloseButton->SetShortcuts(JGetString("itsCloseButton::CBViewManPageDialog::shortcuts::JXLayout"));

	itsViewButton =
		jnew JXTextButton(JGetString("itsViewButton::CBViewManPageDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 280,120, 60,20);
	assert( itsViewButton != NULL );
	itsViewButton->SetShortcuts(JGetString("itsViewButton::CBViewManPageDialog::shortcuts::JXLayout"));

	itsFnName =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,40, 290,20);
	assert( itsFnName != NULL );

	JXStaticText* obj1_JXLayout =
		jnew JXStaticText(JGetString("obj1_JXLayout::CBViewManPageDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 100,20);
	assert( obj1_JXLayout != NULL );
	obj1_JXLayout->SetToLabel();

	JXStaticText* obj2_JXLayout =
		jnew JXStaticText(JGetString("obj2_JXLayout::CBViewManPageDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,80, 90,20);
	assert( obj2_JXLayout != NULL );
	obj2_JXLayout->SetToLabel();

	itsAproposCheckbox =
		jnew JXTextCheckbox(JGetString("itsAproposCheckbox::CBViewManPageDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 210,80, 80,20);
	assert( itsAproposCheckbox != NULL );
	itsAproposCheckbox->SetShortcuts(JGetString("itsAproposCheckbox::CBViewManPageDialog::shortcuts::JXLayout"));

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::CBViewManPageDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 200,120, 60,20);
	assert( itsHelpButton != NULL );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::CBViewManPageDialog::shortcuts::JXLayout"));

	itsFnHistoryMenu =
		jnew JXStringHistoryMenu(kHistoryLength, "", window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 310,40, 30,20);
	assert( itsFnHistoryMenu != NULL );

	itsManIndex =
		jnew JXCharInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 110,80, 30,20);
	assert( itsManIndex != NULL );

	itsStayOpenCB =
		jnew JXTextCheckbox(JGetString("itsStayOpenCB::CBViewManPageDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,120, 90,20);
	assert( itsStayOpenCB != NULL );

// end JXLayout

	window->SetTitle("Man pages");
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();
	window->ShouldFocusWhenShow(kJTrue);

	ListenTo(itsViewButton);
	ListenTo(itsCloseButton);
	ListenTo(itsHelpButton);
	ListenTo(itsFnHistoryMenu);

	itsFnName->ShouldBroadcastAllTextChanged(kJTrue);
	itsFnName->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);
	ListenTo(itsFnName);

	itsManIndex->SetIsRequired(kJFalse);
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
CBViewManPageDialog::UpdateDisplay()
{
	if (itsFnName->IsEmpty())
		{
		itsViewButton->Deactivate();
		}
	else
		{
		itsViewButton->Activate();
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBViewManPageDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsViewButton && message.Is(JXButton::kPushed))
		{
		ViewManPage();
		if (!itsStayOpenCB->IsChecked())
			{
			Deactivate();
			}
		}
	else if (sender == itsCloseButton && message.Is(JXButton::kPushed))
		{
		Deactivate();
		}
	else if (sender == itsHelpButton && message.Is(JXButton::kPushed))
		{
		(JXGetHelpManager())->ShowSection("CBManPageHelp");
		}

	else if (sender == itsFnHistoryMenu && message.Is(JXMenu::kItemSelected))
		{
		SetFunction(itsFnHistoryMenu->GetItemText(message));
		itsFnName->Focus();
		}

	else if (sender == itsFnName &&
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
 ViewManPage (private)

 ******************************************************************************/

void
CBViewManPageDialog::ViewManPage()
{
	itsFnName->DeactivateCurrentUndo();

	if (itsFnName->InputValid() && itsManIndex->InputValid())
		{
		JCharacter manIndex = ' ';
		if (!itsManIndex->IsEmpty())
			{
			manIndex = (itsManIndex->GetText()).GetFirstCharacter();
			}

		CBManPageDocument::Create(NULL, itsFnName->GetText(), manIndex,
								  itsAproposCheckbox->IsChecked());
		}
}

/******************************************************************************
 AddToHistory

	By having CBManPageDocument call this function, others can create
	CBManPageDocuments directly without telling us.

 ******************************************************************************/

void
CBViewManPageDialog::AddToHistory
	(
	const JCharacter*	pageName,
	const JCharacter	pageIndex,
	const JBoolean		apropos
	)
{
	JString historyStr = pageName;
	if (apropos)
		{
		historyStr += " (*)";
		}
	else if (pageIndex != ' ')
		{
		historyStr.AppendCharacter(' ');
		historyStr.AppendCharacter('(');
		historyStr.AppendCharacter(pageIndex);
		historyStr.AppendCharacter(')');
		}

	itsFnHistoryMenu->AddString(historyStr);
}

/******************************************************************************
 SetFunction (private)

 ******************************************************************************/

void
CBViewManPageDialog::SetFunction
	(
	const JCharacter* historyStr
	)
{
	JString fnName = historyStr;

	JCharacter manIndex[] = { '\0', '\0' };
	if (fnName.GetLastCharacter() == ')')
		{
		manIndex[0] = fnName.GetCharacter(fnName.GetLength()-1);
		fnName.RemoveSubstring(fnName.GetLength()-3, fnName.GetLength());
		}

	if (manIndex[0] == '*')
		{
		manIndex[0] = '\0';
		itsAproposCheckbox->SetState(kJTrue);
		}
	else
		{
		itsAproposCheckbox->SetState(kJFalse);
		}

	itsFnName->SetText(fnName);
	itsManIndex->SetText(manIndex);
}

/******************************************************************************
 ReadPrefs (virtual protected)

 ******************************************************************************/

void
CBViewManPageDialog::ReadPrefs
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

	itsFnHistoryMenu->ReadSetup(input);

	if (vers >= 1)
		{
		JBoolean stayOpen;
		input >> stayOpen;
		itsStayOpenCB->SetState(stayOpen);
		}
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

void
CBViewManPageDialog::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentSetupVersion;

	output << ' ';
	GetWindow()->WriteGeometry(output);

	output << ' ';
	itsFnHistoryMenu->WriteSetup(output);

	output << ' ' << itsStayOpenCB->IsChecked();
	output << ' ';
}
