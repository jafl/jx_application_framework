/******************************************************************************
 SyGViewManPageDialog.cpp

	BASE CLASS = JXWindowDirector, JPrefObject

	Copyright © 1996-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <SyGStdInc.h>
#include "SyGViewManPageDialog.h"
#include "SyGGlobals.h"
#include "SyGHelpText.h"
#include "SyGPrefsMgr.h"
#include "SyGApplication.h"

#include <JXWindow.h>
#include <JXHelpManager.h>
#include <JXTextButton.h>
#include <JXCharInput.h>
#include <JXStaticText.h>
#include <JXTextCheckbox.h>
#include <JXStringHistoryMenu.h>
#include <JXChooseSaveFile.h>

#include <JSimpleProcess.h>
#include <jAssert.h>

const JSize kHistoryLength = 20;

static const JCharacter* kDefaultViewCmd = "jcc --man";
static const JCharacter* kDefaultViewBin = "jcc";
static const JCharacter* kDefaultViewArg = " --man ";
JString SyGViewManPageDialog::itsViewCmd = kDefaultViewCmd;

// setup information

const JFileVersion kCurrentSetupVersion = 2;

	// version 1 stores itsStayOpenCB
	// version 2 stores itsViewCmd

/******************************************************************************
 Constructor

 ******************************************************************************/

SyGViewManPageDialog::SyGViewManPageDialog
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor),
	JPrefObject(SyGGetPrefsMgr(), kSViewManPagePrefID)
{
	BuildWindow();
	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SyGViewManPageDialog::~SyGViewManPageDialog()
{
	// prefs written by SyGDeleteGlobals()
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
SyGViewManPageDialog::Activate()
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
SyGViewManPageDialog::BuildWindow()
{
// begin JXLayout

    JXWindow* window = new JXWindow(this, 360,150, "");
    assert( window != NULL );
    SetWindow(window);

    itsCloseButton =
        new JXTextButton(JGetString("itsCloseButton::SyGViewManPageDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 120,120, 60,20);
    assert( itsCloseButton != NULL );
    itsCloseButton->SetShortcuts(JGetString("itsCloseButton::SyGViewManPageDialog::shortcuts::JXLayout"));

    itsViewButton =
        new JXTextButton(JGetString("itsViewButton::SyGViewManPageDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 279,119, 62,22);
    assert( itsViewButton != NULL );
    itsViewButton->SetShortcuts(JGetString("itsViewButton::SyGViewManPageDialog::shortcuts::JXLayout"));

    itsFnName =
        new JXInputField(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 20,40, 290,20);
    assert( itsFnName != NULL );

    JXStaticText* obj1_JXLayout =
        new JXStaticText(JGetString("obj1_JXLayout::SyGViewManPageDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 100,20);
    assert( obj1_JXLayout != NULL );

    JXStaticText* obj2_JXLayout =
        new JXStaticText(JGetString("obj2_JXLayout::SyGViewManPageDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,80, 90,20);
    assert( obj2_JXLayout != NULL );

    itsAproposCheckbox =
        new JXTextCheckbox(JGetString("itsAproposCheckbox::SyGViewManPageDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 210,80, 80,20);
    assert( itsAproposCheckbox != NULL );
    itsAproposCheckbox->SetShortcuts(JGetString("itsAproposCheckbox::SyGViewManPageDialog::shortcuts::JXLayout"));

    itsHelpButton =
        new JXTextButton(JGetString("itsHelpButton::SyGViewManPageDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 200,120, 60,20);
    assert( itsHelpButton != NULL );
    itsHelpButton->SetShortcuts(JGetString("itsHelpButton::SyGViewManPageDialog::shortcuts::JXLayout"));

    itsFnHistoryMenu =
        new JXStringHistoryMenu(kHistoryLength, "", window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 310,40, 30,20);
    assert( itsFnHistoryMenu != NULL );

    itsManIndex =
        new JXCharInput(window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 110,80, 30,20);
    assert( itsManIndex != NULL );

    itsStayOpenCB =
        new JXTextCheckbox(JGetString("itsStayOpenCB::SyGViewManPageDialog::JXLayout"), window,
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

	UpdateDisplay();
}

/******************************************************************************
 UpdateDisplay (private)

 ******************************************************************************/

void
SyGViewManPageDialog::UpdateDisplay()
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
SyGViewManPageDialog::Receive
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
		(GetWindow())->KillFocus();
		Deactivate();
		}
	else if (sender == itsHelpButton && message.Is(JXButton::kPushed))
		{
		(JXGetHelpManager())->ShowSection(kSyGManPageHelpName);
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
SyGViewManPageDialog::ViewManPage()
{
	if (itsFnName->InputValid() && itsManIndex->InputValid())
		{
		JCharacter manIndex = ' ';
		if (!itsManIndex->IsEmpty())
			{
			manIndex = (itsManIndex->GetText()).GetFirstCharacter();
			}

		ViewManPage(itsFnName->GetText(), manIndex,
					itsAproposCheckbox->IsChecked());
		AddToHistory(itsFnName->GetText(), manIndex,
					 itsAproposCheckbox->IsChecked());
		}
}

/******************************************************************************
 ViewManPage

 ******************************************************************************/

void
SyGViewManPageDialog::ViewManPage
	(
	const JCharacter*	item,
	const JCharacter	index,
	const JBoolean		apropos
	)
{
	JString cmd = itsViewCmd;
	cmd += " ";
	if (apropos)
		{
		cmd += "-k ";
		}
	else if (index != ' ')
		{
		#ifdef _J_MAN_SECTION_VIA_DASH_S
		if (itsViewCmd != kDefaultViewCmd)
			{
			cmd += "-s ";
			}
		#endif

		const JCharacter pageIndexStr[] = { index, ' ', '\0' };
		cmd += pageIndexStr;
		}
	cmd += JPrepArgForExec(item);

	JSimpleProcess::Create(cmd, kJTrue);
}

/******************************************************************************
 ViewManPages

 ******************************************************************************/

void
SyGViewManPageDialog::ViewManPages
	(
	const JPtrArray<JString>& list
	)
{
	if (itsViewCmd == kDefaultViewCmd)
		{
		JString cmd = kDefaultViewBin;

		const JSize count = list.GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			cmd += kDefaultViewArg;
			cmd += JPrepArgForExec(*(list.NthElement(i)));
			}

		JSimpleProcess::Create(cmd, kJTrue);
		}
	else
		{
		const JSize count = list.GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			ViewManPage(*(list.NthElement(i)));
			}
		}
}

/******************************************************************************
 SetFunction (private)

 ******************************************************************************/

void
SyGViewManPageDialog::SetFunction
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
 ReadPrefs (virtual)

 ******************************************************************************/

void
SyGViewManPageDialog::ReadPrefs
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

	itsFnHistoryMenu->ReadSetup(input);

	if (vers >= 1)
		{
		JBoolean stayOpen;
		input >> stayOpen;
		itsStayOpenCB->SetState(stayOpen);
		}

	if (vers >= 2)
		{
		input >> itsViewCmd;
		}
}

/******************************************************************************
 WritePrefs (virtual)

 ******************************************************************************/

void
SyGViewManPageDialog::WritePrefs
	(
	ostream& output
	)
	const
{
	output << kCurrentSetupVersion;

	output << ' ';
	(GetWindow())->WriteGeometry(output);

	output << ' ';
	itsFnHistoryMenu->WriteSetup(output);

	output << ' ' << itsStayOpenCB->IsChecked();
	output << ' ' << itsViewCmd;
}

/******************************************************************************
 AddToHistory

 ******************************************************************************/

void
SyGViewManPageDialog::AddToHistory
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
