/******************************************************************************
 SyGViewManPageDialog.cpp

	BASE CLASS = JXWindowDirector, JPrefObject

	Copyright (C) 1996-98 by John Lindal.

 ******************************************************************************/

#include "SyGViewManPageDialog.h"
#include "SyGPrefsMgr.h"
#include "SyGGlobals.h"

#include <JXWindow.h>
#include <JXHelpManager.h>
#include <JXTextButton.h>
#include <JXCharInput.h>
#include <JXStaticText.h>
#include <JXTextCheckbox.h>
#include <JXStringHistoryMenu.h>
#include <JXChooseSaveFile.h>

#include <JSimpleProcess.h>
#include <JStringIterator.h>
#include <jAssert.h>

const JSize kHistoryLength = 20;

static const JString kDefaultViewCmd("jcc --man", JString::kNoCopy);
static const JString kDefaultViewBin("jcc", JString::kNoCopy);
static const JString kDefaultViewArg(" --man ", JString::kNoCopy);

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

	auto* window = jnew JXWindow(this, 360,150, JString::empty);
	assert( window != nullptr );

	itsCloseButton =
		jnew JXTextButton(JGetString("itsCloseButton::SyGViewManPageDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 120,120, 60,20);
	assert( itsCloseButton != nullptr );
	itsCloseButton->SetShortcuts(JGetString("itsCloseButton::SyGViewManPageDialog::shortcuts::JXLayout"));

	itsViewButton =
		jnew JXTextButton(JGetString("itsViewButton::SyGViewManPageDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 280,120, 60,20);
	assert( itsViewButton != nullptr );
	itsViewButton->SetShortcuts(JGetString("itsViewButton::SyGViewManPageDialog::shortcuts::JXLayout"));

	itsFnName =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,40, 290,20);
	assert( itsFnName != nullptr );

	auto* fnNameLabel =
		jnew JXStaticText(JGetString("fnNameLabel::SyGViewManPageDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 100,20);
	assert( fnNameLabel != nullptr );
	fnNameLabel->SetToLabel();

	auto* sectionLabel =
		jnew JXStaticText(JGetString("sectionLabel::SyGViewManPageDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,80, 90,20);
	assert( sectionLabel != nullptr );
	sectionLabel->SetToLabel();

	itsAproposCheckbox =
		jnew JXTextCheckbox(JGetString("itsAproposCheckbox::SyGViewManPageDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 210,80, 80,20);
	assert( itsAproposCheckbox != nullptr );
	itsAproposCheckbox->SetShortcuts(JGetString("itsAproposCheckbox::SyGViewManPageDialog::shortcuts::JXLayout"));

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::SyGViewManPageDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 200,120, 60,20);
	assert( itsHelpButton != nullptr );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::SyGViewManPageDialog::shortcuts::JXLayout"));

	itsFnHistoryMenu =
		jnew JXStringHistoryMenu(kHistoryLength, JString::empty, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 310,40, 30,20);
	assert( itsFnHistoryMenu != nullptr );

	itsManIndex =
		jnew JXCharInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 110,80, 30,20);
	assert( itsManIndex != nullptr );

	itsStayOpenCB =
		jnew JXTextCheckbox(JGetString("itsStayOpenCB::SyGViewManPageDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,120, 90,20);
	assert( itsStayOpenCB != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::SyGViewManPageDialog"));
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();
	window->ShouldFocusWhenShow(true);

	ListenTo(itsViewButton);
	ListenTo(itsCloseButton);
	ListenTo(itsHelpButton);
	ListenTo(itsFnHistoryMenu);

	itsFnName->GetText()->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);
	ListenTo(itsFnName);

	itsManIndex->SetIsRequired(false);
	itsStayOpenCB->SetState(true);

	UpdateDisplay();
}

/******************************************************************************
 UpdateDisplay (private)

 ******************************************************************************/

void
SyGViewManPageDialog::UpdateDisplay()
{
	if (itsFnName->GetText()->IsEmpty())
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
		GetWindow()->KillFocus();
		Deactivate();
		}
	else if (sender == itsHelpButton && message.Is(JXButton::kPushed))
		{
		(JXGetHelpManager())->ShowSection("SyGViewManHelp");
		}

	else if (sender == itsFnHistoryMenu && message.Is(JXMenu::kItemSelected))
		{
		SetFunction(itsFnHistoryMenu->GetItemText(message));
		itsFnName->Focus();
		}

	else if (sender == itsFnName &&
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
 ViewManPage (private)

 ******************************************************************************/

void
SyGViewManPageDialog::ViewManPage()
{
	if (itsFnName->InputValid() && itsManIndex->InputValid())
		{
		JUtf8Character manIndex(' ');
		if (!itsManIndex->GetText()->IsEmpty())
			{
			manIndex = (itsManIndex->GetText()->GetText()).GetFirstCharacter();
			}

		ViewManPage(itsFnName->GetText()->GetText(), manIndex,
					itsAproposCheckbox->IsChecked());
		AddToHistory(itsFnName->GetText()->GetText(), manIndex,
					 itsAproposCheckbox->IsChecked());
		}
}

/******************************************************************************
 ViewManPage

 ******************************************************************************/

void
SyGViewManPageDialog::ViewManPage
	(
	const JString&			item,
	const JUtf8Character&	index,
	const bool			apropos
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
		cmd += index;
		cmd += " ";
		}
	cmd += JPrepArgForExec(item);

	JSimpleProcess::Create(cmd, true);
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
			cmd += JPrepArgForExec(*(list.GetElement(i)));
			}

		JSimpleProcess::Create(cmd, true);
		}
	else
		{
		const JSize count = list.GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			ViewManPage(*(list.GetElement(i)));
			}
		}
}

/******************************************************************************
 SetFunction (private)

 ******************************************************************************/

void
SyGViewManPageDialog::SetFunction
	(
	const JString& historyStr
	)
{
	JString fnName = historyStr;

	JUtf8Character manIndex;
	if (fnName.GetLastCharacter() == ')')
		{
		JStringIterator iter(&fnName, kJIteratorStartAtEnd);
		iter.SkipPrev();

		const bool ok = iter.Prev(&manIndex);
		assert( ok );

		iter.SkipPrev(2);
		iter.RemoveAllNext();
		}

	if (manIndex == '*')
		{
		manIndex = '\0';
		itsAproposCheckbox->SetState(true);
		}
	else
		{
		itsAproposCheckbox->SetState(false);
		}

	itsFnName->GetText()->SetText(fnName);
	itsManIndex->GetText()->SetText(JString(manIndex));
}

/******************************************************************************
 ReadPrefs (virtual)

 ******************************************************************************/

void
SyGViewManPageDialog::ReadPrefs
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
		bool stayOpen;
		input >> JBoolFromString(stayOpen);
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
	std::ostream& output
	)
	const
{
	output << kCurrentSetupVersion;

	output << ' ';
	GetWindow()->WriteGeometry(output);

	output << ' ';
	itsFnHistoryMenu->WriteSetup(output);

	output << ' ' << JBoolToString(itsStayOpenCB->IsChecked());
	output << ' ' << itsViewCmd;
}

/******************************************************************************
 AddToHistory

 ******************************************************************************/

void
SyGViewManPageDialog::AddToHistory
	(
	const JString&			pageName,
	const JUtf8Character&	pageIndex,
	const bool			apropos
	)
{
	JString historyStr = pageName;
	if (apropos)
		{
		historyStr += " (*)";
		}
	else if (pageIndex != ' ')
		{
		historyStr.Append(" ");
		historyStr.Append("(");
		historyStr.Append(pageIndex);
		historyStr.Append(")");
		}

	itsFnHistoryMenu->AddString(historyStr);
}
