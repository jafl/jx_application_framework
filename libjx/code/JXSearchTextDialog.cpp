/******************************************************************************
 JXSearchTextDialog.cpp

	This is used as a global object.  We relay all the requests for action
	to the active text editor.  This way, all the text editors share the
	same dialog window.

	We also support the XSearch protocol so our settings can be shared with
	other programs.  (http://johnlindal.wixsite.com/xsearch)

	Derived classes must override BuildWindow() and call SetObjects()
	at the end of their implementation.  They must also call
	JXSearchTextDialogX() after calling BuildWindow().

	To do:  support multiple displays

	BASE CLASS = JXWindowDirector

	Copyright (C) 1996-99 by John Lindal.

 ******************************************************************************/

#include "JXSearchTextDialog.h"
#include "JXSearchTextButton.h"
#include "JXDisplay.h"
#include "JXWindow.h"
#include "JXStaticText.h"
#include "JXInputField.h"
#include "JXTextButton.h"
#include "JXTextCheckbox.h"
#include "JXRadioGroup.h"
#include "JXTextRadioButton.h"
#include "JXStringHistoryMenu.h"
#include "JXTimerTask.h"
#include "JXMenu.h"
#include "JXSearchTextDecorTask.h"
#include "JXFontManager.h"
#include "JXColorManager.h"
#include "jXGlobals.h"
#include <JWebBrowser.h>
#include <JRegex.h>
#include <JInterpolate.h>
#include <jStreamUtil.h>
#include <sstream>
#include <jAssert.h>

// JTextEditor doesn't broadcast when selection changes (would require optimization)
const JSize kUpdatePeriod = 200;	// 0.2 second (milliseconds)

// setup information

const JFileVersion kCurrentSetupVersion = 7;

	// version 1 uses JXWindow::Read/WriteGeometry()
	// version 2 stores replaceIsRegex
	// version 3 stores singleLine, matchCase
	// version 4 stores search file list, itsOnlyListFilesFlag
	// version 5 stores itsListFilesWithoutMatchFlag, itsFileSetName
	// version 6 no longer stores search direction
	// version 7 stores itsStayOpenCB, itsRetainFocusCB

/******************************************************************************
 Constructor function (static)

 ******************************************************************************/

JXSearchTextDialog*
JXSearchTextDialog::Create()
{
	auto* dlog = jnew JXSearchTextDialog();
	assert( dlog != nullptr );
	dlog->BuildWindow();
	dlog->JXSearchTextDialogX();
	return dlog;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXSearchTextDialog::JXSearchTextDialog()
	:
	JXWindowDirector(JXGetPersistentWindowOwner())
{
	itsTE = nullptr;

	itsRegex = jnew JRegex;
	assert( itsRegex != nullptr );
	itsRegex->SetLineBegin(true);				// ^ matches beginning of line, not text
	itsRegex->SetLineEnd(true);				// $ matches end of line, not text

	itsInterpolator = jnew JInterpolate;
	assert( itsInterpolator != nullptr );
	itsInterpolator->SetWhitespaceEscapes();

	itsUpdateTask = jnew JXTimerTask(kUpdatePeriod);
	assert( itsUpdateTask != nullptr );
	ListenTo(itsUpdateTask);

	itsNeedXSearchBcastFlag    = false;
	itsIgnoreXSearchChangeFlag = true;			// until InitXSearch() is called

	itsVersionWindow = None;
	itsDataWindow    = None;

	JXSetSearchTextDialog(this);
}

// protected -- must be done after prefs are loaded

void
JXSearchTextDialog::JXSearchTextDialogX()
{
	InitXSearch();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXSearchTextDialog::~JXSearchTextDialog()
{
	jdelete itsUpdateTask;
	jdelete itsRegex;
	jdelete itsInterpolator;
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
JXSearchTextDialog::Activate()
{
	JXWindowDirector::Activate();

	if (IsActive())
		{
		itsSearchInput->Focus();
		itsSearchInput->SelectAll();

		itsUpdateTask->Start();
		}
}

/******************************************************************************
 Deactivate (virtual)

 ******************************************************************************/

bool
JXSearchTextDialog::Deactivate()
{
	if (JXWindowDirector::Deactivate())
		{
		if (itsNeedXSearchBcastFlag)
			{
			SetXSearch();
			}

		itsUpdateTask->Stop();
		return true;
		}
	else
		{
		return false;
		}
}

/******************************************************************************
 GetSearchText

 ******************************************************************************/

const JString&
JXSearchTextDialog::GetSearchText()
	const
{
	return itsSearchInput->GetText()->GetText();
}

/******************************************************************************
 SetSearchText

 ******************************************************************************/

void
JXSearchTextDialog::SetSearchText
	(
	const JString& str
	)
{
	if (str != itsSearchInput->GetText()->GetText())
		{
		itsSearchInput->GetText()->SetText(str);
		}
	itsSearchInput->Focus();
	itsSearchInput->SelectAll();
//	itsPrevSearchMenu->AddString(str);
	itsSearchIsRegexCB->SetState(false);
}

/******************************************************************************
 SetRegexSearch

 ******************************************************************************/

void
JXSearchTextDialog::SetRegexSearch
	(
	const bool regex
	)
{
	itsSearchIsRegexCB->SetState(regex);
}

/******************************************************************************
 SetReplaceText

 ******************************************************************************/

void
JXSearchTextDialog::SetReplaceText
	(
	const JString& str
	)
{
	if (str != itsReplaceInput->GetText()->GetText())
		{
		itsReplaceInput->GetText()->SetText(str);
		}
	itsReplaceInput->Focus();
	itsReplaceInput->SelectAll();
//	itsPrevReplaceMenu->AddString(str);
	itsReplaceIsRegexCB->SetState(false);
}

/******************************************************************************
 SetRegexReplace

 ******************************************************************************/

void
JXSearchTextDialog::SetRegexReplace
	(
	const bool regex
	)
{
	itsReplaceIsRegexCB->SetState(regex);
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JXSearchTextDialog::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 450,300, JString::empty);
	assert( window != nullptr );

	itsCloseButton =
		jnew JXTextButton(JGetString("itsCloseButton::JXSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 320,270, 80,20);
	assert( itsCloseButton != nullptr );
	itsCloseButton->SetShortcuts(JGetString("itsCloseButton::JXSearchTextDialog::shortcuts::JXLayout"));

	auto* searchInputLabel =
		jnew JXStaticText(JGetString("searchInputLabel::JXSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,9, 220,20);
	assert( searchInputLabel != nullptr );
	searchInputLabel->SetToLabel();

	auto* replaceInputLabel =
		jnew JXStaticText(JGetString("replaceInputLabel::JXSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,85, 220,20);
	assert( replaceInputLabel != nullptr );
	replaceInputLabel->SetToLabel();

	itsIgnoreCaseCB =
		jnew JXTextCheckbox(JGetString("itsIgnoreCaseCB::JXSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,170, 130,20);
	assert( itsIgnoreCaseCB != nullptr );
	itsIgnoreCaseCB->SetShortcuts(JGetString("itsIgnoreCaseCB::JXSearchTextDialog::shortcuts::JXLayout"));

	itsWrapSearchCB =
		jnew JXTextCheckbox(JGetString("itsWrapSearchCB::JXSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,230, 130,20);
	assert( itsWrapSearchCB != nullptr );
	itsWrapSearchCB->SetShortcuts(JGetString("itsWrapSearchCB::JXSearchTextDialog::shortcuts::JXLayout"));

	itsReplaceButton =
		jnew JXTextButton(JGetString("itsReplaceButton::JXSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,50, 140,20);
	assert( itsReplaceButton != nullptr );
	itsReplaceButton->SetShortcuts(JGetString("itsReplaceButton::JXSearchTextDialog::shortcuts::JXLayout"));

	itsEntireWordCB =
		jnew JXTextCheckbox(JGetString("itsEntireWordCB::JXSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,200, 130,20);
	assert( itsEntireWordCB != nullptr );
	itsEntireWordCB->SetShortcuts(JGetString("itsEntireWordCB::JXSearchTextDialog::shortcuts::JXLayout"));

	itsPrevReplaceMenu =
		jnew JXStringHistoryMenu(kHistoryLength, JString::empty, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,115, 30,20);
	assert( itsPrevReplaceMenu != nullptr );

	itsPrevSearchMenu =
		jnew JXStringHistoryMenu(kHistoryLength, JString::empty, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,41, 30,20);
	assert( itsPrevSearchMenu != nullptr );

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::JXSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 60,270, 80,20);
	assert( itsHelpButton != nullptr );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::JXSearchTextDialog::shortcuts::JXLayout"));

	itsSearchIsRegexCB =
		jnew JXTextCheckbox(JGetString("itsSearchIsRegexCB::JXSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 160,170, 150,20);
	assert( itsSearchIsRegexCB != nullptr );
	itsSearchIsRegexCB->SetShortcuts(JGetString("itsSearchIsRegexCB::JXSearchTextDialog::shortcuts::JXLayout"));

	itsReplaceIsRegexCB =
		jnew JXTextCheckbox(JGetString("itsReplaceIsRegexCB::JXSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 320,170, 110,20);
	assert( itsReplaceIsRegexCB != nullptr );
	itsReplaceIsRegexCB->SetShortcuts(JGetString("itsReplaceIsRegexCB::JXSearchTextDialog::shortcuts::JXLayout"));

	itsSingleLineCB =
		jnew JXTextCheckbox(JGetString("itsSingleLineCB::JXSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 160,200, 150,20);
	assert( itsSingleLineCB != nullptr );
	itsSingleLineCB->SetShortcuts(JGetString("itsSingleLineCB::JXSearchTextDialog::shortcuts::JXLayout"));

	itsPreserveCaseCB =
		jnew JXTextCheckbox(JGetString("itsPreserveCaseCB::JXSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 320,200, 110,20);
	assert( itsPreserveCaseCB != nullptr );
	itsPreserveCaseCB->SetShortcuts(JGetString("itsPreserveCaseCB::JXSearchTextDialog::shortcuts::JXLayout"));

	itsQRefButton =
		jnew JXTextButton(JGetString("itsQRefButton::JXSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 190,270, 80,20);
	assert( itsQRefButton != nullptr );

	auto* findLabel =
		jnew JXStaticText(JGetString("findLabel::JXSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 320,20, 100,20);
	assert( findLabel != nullptr );
	findLabel->SetToLabel(true);

	itsFindBackButton =
		jnew JXSearchTextButton(false, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,20, 20,20);
	assert( itsFindBackButton != nullptr );

	itsFindFwdButton =
		jnew JXSearchTextButton(true, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 420,20, 20,20);
	assert( itsFindFwdButton != nullptr );

	auto* replaceFindLabel =
		jnew JXStaticText(JGetString("replaceFindLabel::JXSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 320,80, 100,20);
	assert( replaceFindLabel != nullptr );
	replaceFindLabel->SetToLabel(true);

	itsReplaceFindBackButton =
		jnew JXSearchTextButton(false, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,80, 20,20);
	assert( itsReplaceFindBackButton != nullptr );

	itsReplaceFindFwdButton =
		jnew JXSearchTextButton(true, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 420,80, 20,20);
	assert( itsReplaceFindFwdButton != nullptr );

	itsReplaceAllInSelButton =
		jnew JXTextButton(JGetString("itsReplaceAllInSelButton::JXSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 370,110, 70,20);
	assert( itsReplaceAllInSelButton != nullptr );

	itsStayOpenCB =
		jnew JXTextCheckbox(JGetString("itsStayOpenCB::JXSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 160,230, 150,20);
	assert( itsStayOpenCB != nullptr );

	itsRetainFocusCB =
		jnew JXTextCheckbox(JGetString("itsRetainFocusCB::JXSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 320,230, 110,20);
	assert( itsRetainFocusCB != nullptr );

	itsSearchInput =
		jnew JXInputField(true, false, window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,29, 220,45);
	assert( itsSearchInput != nullptr );

	itsReplaceInput =
		jnew JXInputField(true, false, window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,105, 220,45);
	assert( itsReplaceInput != nullptr );

	itsReplaceAllButton =
		jnew JXTextButton(JGetString("itsReplaceAllButton::JXSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,110, 70,20);
	assert( itsReplaceAllButton != nullptr );

// end JXLayout

	SetObjects(itsSearchInput, itsPrevSearchMenu,
			   itsIgnoreCaseCB, itsEntireWordCB, itsWrapSearchCB,
			   itsSearchIsRegexCB, itsSingleLineCB,
			   itsReplaceInput, itsPrevReplaceMenu,
			   itsReplaceIsRegexCB, itsPreserveCaseCB,
			   itsStayOpenCB, itsRetainFocusCB,
			   itsFindFwdButton, itsFindBackButton,
			   itsReplaceButton,
			   itsReplaceFindFwdButton, itsReplaceFindBackButton,
			   itsReplaceAllButton, itsReplaceAllInSelButton,
			   itsCloseButton, itsHelpButton, itsQRefButton);
}

/******************************************************************************
 SetObjects (protected)

 ******************************************************************************/

void
JXSearchTextDialog::SetObjects
	(
	JXInputField*			searchInput,
	JXStringHistoryMenu*	prevSearchMenu,
	JXTextCheckbox*			ignoreCaseCB,
	JXTextCheckbox*			entireWordCB,
	JXTextCheckbox*			wrapSearchCB,
	JXTextCheckbox*			searchIsRegexCB,
	JXTextCheckbox*			singleLineCB,

	JXInputField*			replaceInput,
	JXStringHistoryMenu*	prevReplaceMenu,
	JXTextCheckbox*			replaceIsRegexCB,
	JXTextCheckbox*			preserveCaseCB,

	JXTextCheckbox*			stayOpenCB,
	JXTextCheckbox*			retainFocusCB,

	JXSearchTextButton*		findFwdButton,
	JXSearchTextButton*		findBackButton,
	JXTextButton*			replaceButton,
	JXSearchTextButton*		replaceFindFwdButton,
	JXSearchTextButton*		replaceFindBackButton,
	JXTextButton*			replaceAllButton,
	JXTextButton*			replaceAllInSelButton,

	JXTextButton*			closeButton,
	JXTextButton*			helpButton,
	JXTextButton*			qRefButton
	)
{
	JXWindow* window = searchInput->GetWindow();
	window->SetTitle(JGetString("WindowTitle::JXSearchTextDialog"));
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();
	window->ShouldFocusWhenShow(true);
	ListenTo(window);

	itsSearchInput     = searchInput;
	itsPrevSearchMenu  = prevSearchMenu;
	itsIgnoreCaseCB    = ignoreCaseCB;
	itsEntireWordCB    = entireWordCB;
	itsWrapSearchCB    = wrapSearchCB;
	itsSearchIsRegexCB = searchIsRegexCB;
	itsSingleLineCB    = singleLineCB;

	itsReplaceInput     = replaceInput;
	itsPrevReplaceMenu  = prevReplaceMenu;
	itsReplaceIsRegexCB = replaceIsRegexCB;
	itsPreserveCaseCB   = preserveCaseCB;

	itsStayOpenCB    = stayOpenCB;
	itsRetainFocusCB = retainFocusCB;

	itsFindFwdButton         = findFwdButton;
	itsFindBackButton        = findBackButton;
	itsReplaceButton         = replaceButton;
	itsReplaceFindFwdButton  = replaceFindFwdButton;
	itsReplaceFindBackButton = replaceFindBackButton;
	itsReplaceAllButton      = replaceAllButton;
	itsReplaceAllInSelButton = replaceAllInSelButton;

	itsCloseButton = closeButton;
	itsHelpButton  = helpButton;
	itsQRefButton  = qRefButton;

	SetFont(JFontManager::GetDefaultMonospaceFont());

	// decor

	JXUrgentTask* decorTask = jnew JXSearchTextDecorTask(window, itsStayOpenCB, itsRetainFocusCB);
	assert( decorTask != nullptr );
	decorTask->Go();

	// shortcuts

	itsCloseButton->SetShortcuts(JGetString("CloseShortcut::JXSearchTextDialog"));
	itsIgnoreCaseCB->SetShortcuts(JGetString("IgnoreCaseShortcut::JXSearchTextDialog"));
	itsWrapSearchCB->SetShortcuts(JGetString("WrapSearchShortcut::JXSearchTextDialog"));
	itsEntireWordCB->SetShortcuts(JGetString("EntireWordShortcut::JXSearchTextDialog"));
	itsSearchIsRegexCB->SetShortcuts(JGetString("SearchIsRegexShortcut::JXSearchTextDialog"));
	itsSingleLineCB->SetShortcuts(JGetString("SingleLineShortcut::JXSearchTextDialog"));
	itsPreserveCaseCB->SetShortcuts(JGetString("PreserveCaseShortcut::JXSearchTextDialog"));

	if (JXMenu::GetDefaultStyle() == JXMenu::kMacintoshStyle)
		{
		itsFindFwdButton->SetShortcuts(JGetString("FindForwardMacShortcut::JXSearchTextDialog"));
		itsFindFwdButton->SetHint(JGetString("FindForwardMacHint::JXSearchTextDialog"));

		itsFindBackButton->SetHint(JGetString("FindBackMacHint::JXSearchTextDialog"));

		itsReplaceButton->SetShortcuts(JGetString("ReplaceMacShortcut::JXSearchTextDialog"));
		itsReplaceButton->SetHint(JGetString("ReplaceMacHint::JXSearchTextDialog"));

		itsReplaceFindFwdButton->SetShortcuts(JGetString("ReplaceFindForwardMacShortcut::JXSearchTextDialog"));
		itsReplaceFindFwdButton->SetHint(JGetString("ReplaceFindForwardMacHint::JXSearchTextDialog"));

		itsReplaceFindBackButton->SetHint(JGetString("ReplaceFindBackMacHint::JXSearchTextDialog"));
		}
	else
		{
		itsFindFwdButton->SetShortcuts(JGetString("FindForwardWinShortcut::JXSearchTextDialog"));
		itsFindFwdButton->SetHint(JGetString("FindForwardWinHint::JXSearchTextDialog"));

		itsFindBackButton->SetHint(JGetString("FindBackWinHint::JXSearchTextDialog"));

		itsReplaceButton->SetShortcuts(JGetString("ReplaceWinShortcut::JXSearchTextDialog"));
		itsReplaceButton->SetHint(JGetString("ReplaceWinHint::JXSearchTextDialog"));

		itsReplaceFindFwdButton->SetShortcuts(JGetString("ReplaceFindForwardWinShortcut::JXSearchTextDialog"));
		itsReplaceFindFwdButton->SetHint(JGetString("ReplaceFindForwardWinHint::JXSearchTextDialog"));

		itsReplaceFindBackButton->SetHint(JGetString("ReplaceFindBackWinHint::JXSearchTextDialog"));
		}

	JXKeyModifiers modifiers(GetDisplay());
	window->InstallShortcut(itsFindFwdButton, JXCtrl('M'), modifiers);

	modifiers.SetState(kJXShiftKeyIndex, true);
	window->InstallShortcut(itsFindBackButton, JXCtrl('M'), modifiers);

	modifiers.SetState(kJXMetaKeyIndex, true);
	itsReplaceIsRegexCB->SetShortcuts(JGetString("ReplaceIsRegexShorcut::JXSearchTextDialog"));
	window->ClearShortcuts(itsReplaceIsRegexCB);
	window->InstallShortcut(itsReplaceIsRegexCB, 'x', modifiers);
	window->InstallShortcut(itsReplaceIsRegexCB, 'X', modifiers);

	if (JXMenu::GetDefaultStyle() != JXMenu::kMacintoshStyle)
		{
		modifiers.SetState(kJXMetaKeyIndex, false);
		modifiers.SetState(kJXControlKeyIndex, true);
		}

	window->InstallShortcut(itsFindBackButton, 'g', modifiers);
	window->InstallShortcut(itsFindBackButton, 'G', modifiers);

	window->InstallShortcut(itsReplaceFindBackButton, 'l', modifiers);
	window->InstallShortcut(itsReplaceFindBackButton, 'L', modifiers);

	// broadcasters

	ListenTo(itsFindFwdButton);
	ListenTo(itsFindBackButton);
	ListenTo(itsReplaceButton);
	ListenTo(itsReplaceFindFwdButton);
	ListenTo(itsReplaceFindBackButton);
	ListenTo(itsReplaceAllButton);
	ListenTo(itsReplaceAllInSelButton);

	ListenTo(itsHelpButton);
	ListenTo(itsQRefButton);
	ListenTo(itsCloseButton);

	ListenTo(itsPrevSearchMenu);
	ListenTo(itsPrevReplaceMenu);

	itsIgnoreCaseCB->SetState(true);
	itsWrapSearchCB->SetState(true);

	itsStayOpenCB->SetState(true);
	itsRetainFocusCB->SetState(true);

	// listen after setting value

	ListenTo(itsStayOpenCB);

	// XSearch data

	ListenTo(itsSearchInput);
	ListenTo(itsSearchIsRegexCB);
	ListenTo(itsSingleLineCB);

	ListenTo(itsReplaceInput);
	ListenTo(itsReplaceIsRegexCB);
	ListenTo(itsPreserveCaseCB);

	ListenTo(itsWrapSearchCB);
	ListenTo(itsEntireWordCB);
	ListenTo(itsIgnoreCaseCB);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXSearchTextDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	bool found = false;

	JXWindowDirector* director = nullptr;
	if (itsTE != nullptr)
		{
		director = (itsTE->GetWindow())->GetDirector();
		}

	if (sender == itsFindFwdButton && message.Is(JXButton::kPushed))
		{
		if (itsTE != nullptr && itsTE->SearchForward())
			{
			found = true;
			director->Activate();
			}
		}
	else if (sender == itsFindBackButton && message.Is(JXButton::kPushed))
		{
		if (itsTE != nullptr && itsTE->SearchBackward())
			{
			found = true;
			director->Activate();
			}
		}

	else if (sender == itsReplaceButton && message.Is(JXButton::kPushed))
		{
		if (itsTE != nullptr && itsTE->ReplaceSelection())
			{
			found = true;
			director->Activate();
			}
		}

	else if (sender == itsReplaceFindFwdButton && message.Is(JXButton::kPushed))
		{
		if (itsTE != nullptr && itsTE->ReplaceAndSearchForward())
			{
			found = true;
			director->Activate();
			}
		}
	else if (sender == itsReplaceFindBackButton && message.Is(JXButton::kPushed))
		{
		if (itsTE != nullptr && itsTE->ReplaceAndSearchBackward())
			{
			found = true;
			director->Activate();
			}
		}

	else if ((sender == itsReplaceAllButton || sender == itsReplaceAllInSelButton) &&
			 message.Is(JXButton::kPushed))
		{
		if (itsTE != nullptr && itsTE->ReplaceAll(sender == itsReplaceAllInSelButton))
			{
			found = true;
			director->Activate();
			}
		}

	else if (sender == itsHelpButton && message.Is(JXButton::kPushed))
		{
		JGetWebBrowser()->ShowURL(JGetString("HelpURL::JXSearchTextDialog"));
		}
	else if (sender == itsQRefButton && message.Is(JXButton::kPushed))
		{
		JGetWebBrowser()->ShowURL(JGetString("QuickRefURL::JXSearchTextDialog"));
		}

	else if (sender == itsCloseButton && message.Is(JXButton::kPushed))
		{
		Deactivate();
		}

	else if (sender == itsPrevSearchMenu && message.Is(JXMenu::kItemSelected))
		{
		// need local copy since menu is changed
		const JString str       = itsPrevSearchMenu->GetItemText(message);
		const bool wasRegex = itsSearchIsRegexCB->IsChecked();
		SetSearchText(str);
		itsSearchIsRegexCB->SetState(wasRegex);
		}
	else if (sender == itsPrevReplaceMenu && message.Is(JXMenu::kItemSelected))
		{
		// need local copy since menu is changed
		const JString str       = itsPrevReplaceMenu->GetItemText(message);
		const bool wasRegex = itsReplaceIsRegexCB->IsChecked();
		SetReplaceText(str);
		itsReplaceIsRegexCB->SetState(wasRegex);
		}

	else if (sender == GetWindow() && message.Is(JXWindow::kIconified))
		{
		if (GetWindow()->IsVisible())
			{
			itsUpdateTask->Stop();
			}
		}
	else if (sender == GetWindow() && message.Is(JXWindow::kDeiconified))
		{
		if (GetWindow()->IsVisible())
			{
			itsUpdateTask->Start();
			}
		}
	else if (sender == itsUpdateTask && message.Is(JXTimerTask::kTimerWentOff))
		{
		UpdateDisplay();
		}

	else if (sender == itsSearchInput || sender == itsReplaceInput)
		{
		UpdateDisplay();

		if (!itsIgnoreXSearchChangeFlag &&
			(message.Is(JStyledText::kTextSet) ||
			 (message.Is(JXWidget::kLostFocus) && itsNeedXSearchBcastFlag)))
			{
			SetXSearch();
			}
		else if (!itsIgnoreXSearchChangeFlag && message.Is(JStyledText::kTextChanged))
			{
			itsNeedXSearchBcastFlag = true;	// wait for them to use it
			}
		}

	else if (sender == itsWrapSearchCB ||
			 sender == itsEntireWordCB ||
			 sender == itsIgnoreCaseCB ||
			 sender == itsSearchIsRegexCB ||
			 sender == itsSingleLineCB ||
			 sender == itsReplaceIsRegexCB ||
			 sender == itsPreserveCaseCB)
		{
		UpdateDisplay();
		if (!itsIgnoreXSearchChangeFlag)
			{
			SetXSearch();
			}
		}

	else if (sender == itsStayOpenCB)
		{
		UpdateDisplay();
		}

	else
		{
		JXWindowDirector::Receive(sender, message);
		}

	if (found && !itsStayOpenCB->IsChecked())
		{
		Deactivate();
		}
	else if (found && itsRetainFocusCB->IsChecked())
		{
		Activate();
		}
}

/******************************************************************************
 HasSearchText

 ******************************************************************************/

bool
JXSearchTextDialog::HasSearchText()
	const
{
	return !itsSearchInput->GetText()->IsEmpty();
}

/******************************************************************************
 HasReplaceText

 ******************************************************************************/

bool
JXSearchTextDialog::HasReplaceText()
	const
{
	return !itsReplaceInput->GetText()->IsEmpty();
}

/******************************************************************************
 GetSearchParameters

	Without regex, "entire word" is only possible if searchStr contains
	a single word.

	Returns false if an error occurs.  (e.g. if regex compile fails)

 ******************************************************************************/

bool
JXSearchTextDialog::GetSearchParameters
	(
	JRegex**		searchRegex,
	bool*		entireWord,
	bool*		wrapSearch,

	JString*		replaceStr,
	JInterpolate**	interpolator,
	bool*		preserveCase
	)
	const
{
	if (!HasSearchText())
		{
		JGetUserNotification()->ReportError(JGetString("EmptySearchText::JXSearchTextDialog"));
		itsSearchInput->Focus();
		return false;
		}

	*searchRegex = itsRegex;
	*wrapSearch  = itsWrapSearchCB->IsChecked();

	*entireWord = itsEntireWordCB->IsChecked() &&
		(itsSearchIsRegexCB->IsChecked() ||
		 itsSearchInput->GetText()->IsEntireWord(itsSearchInput->GetText()->SelectAll()));

	itsRegex->SetCaseSensitive(!itsIgnoreCaseCB->IsChecked());

	if (itsSearchIsRegexCB->IsChecked())
		{
		itsRegex->SetSingleLine(itsSingleLineCB->IsChecked());
		const JError err = itsRegex->SetPattern(itsSearchInput->GetText()->GetText());
		if (!err.OK())
			{
			itsSearchInput->Focus();
			err.ReportIfError();
			return false;
			}
		}
	else
		{
		itsRegex->SetSingleLine(true);
		const JError err = itsRegex->SetPattern(
			JRegex::BackslashForLiteral(itsSearchInput->GetText()->GetText()));
		assert_ok( err );
		}

	*replaceStr   = itsReplaceInput->GetText()->GetText();
	*interpolator = nullptr;
	*preserveCase = itsPreserveCaseCB->IsChecked();

	if (itsReplaceIsRegexCB->IsChecked())
		{
		JCharacterRange errRange;
		const JError err = itsInterpolator->ContainsError(*replaceStr, &errRange);
		if (!err.OK())
			{
			itsReplaceInput->Focus();
			itsReplaceInput->SetSelection(errRange);
			err.ReportIfError();
			return false;
			}

		*interpolator = itsInterpolator;
		}

	// use the original strings, not the substituted ones

	itsPrevSearchMenu->AddString(itsSearchInput->GetText()->GetText());
	itsPrevReplaceMenu->AddString(itsReplaceInput->GetText()->GetText());

	// broadcast to other programs who support Xsearch protocol

	if (itsNeedXSearchBcastFlag)
		{
		SetXSearch();
		}

	return true;
}

/******************************************************************************
 UpdateDisplay (virtual protected)

	We never need to call this because our update task triggers it regularly.

 ******************************************************************************/

void
JXSearchTextDialog::UpdateDisplay()
{
	const bool hasSearchText = HasSearchText();
	const bool teWritable =
		itsTE != nullptr && itsTE->GetType() == JTextEditor::kFullEditor;

	const bool canSearch = itsTE != nullptr && !itsTE->GetText()->IsEmpty() && hasSearchText;
	itsFindFwdButton->SetActive(canSearch);
	itsFindBackButton->SetActive(canSearch);

	const bool canReplace = canSearch && teWritable && itsTE->HasSelection();
	itsReplaceButton->SetActive(canReplace);
	itsReplaceFindFwdButton->SetActive(canReplace);
	itsReplaceFindBackButton->SetActive(canReplace);

	itsReplaceAllButton->SetActive(canSearch && teWritable);
	itsReplaceAllInSelButton->SetActive(canReplace);

	itsSingleLineCB->SetActive(itsSearchIsRegexCB->IsChecked());

	itsRetainFocusCB->SetActive(itsStayOpenCB->IsChecked());
}

/******************************************************************************
 SetFont

 ******************************************************************************/

void
JXSearchTextDialog::SetFont
	(
	const JFont& font
	)
{
	itsSearchInput->SetFont(font);
	itsPrevSearchMenu->SetDefaultFont(font, true);

	itsReplaceInput->SetFont(font);
	itsPrevReplaceMenu->SetDefaultFont(font, true);
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
JXSearchTextDialog::ReadSetup
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	assert( vers <= kCurrentSetupVersion );

	JXWindow* window = GetWindow();
	if (vers == 0)
		{
		JPoint pt;
		input >> pt;
		window->Place(pt.x, pt.y);
		}
	else
		{
		window->ReadGeometry(input);
		}
	window->Deiconify();

	if (vers < 6)
		{
		JIndex searchDirection;
		input >> searchDirection;
		}

	bool wrapSearch, ignoreCase, entireWord, searchIsRegex;
	input >> JBoolFromString(wrapSearch)
		  >> JBoolFromString(ignoreCase)
		  >> JBoolFromString(entireWord)
		  >> JBoolFromString(searchIsRegex);
	itsWrapSearchCB->SetState(wrapSearch);
	itsIgnoreCaseCB->SetState(ignoreCase);
	itsEntireWordCB->SetState(entireWord);
	itsSearchIsRegexCB->SetState(searchIsRegex);

	if (vers >= 2)
		{
		bool replaceIsRegex;
		input >> JBoolFromString(replaceIsRegex);
		itsReplaceIsRegexCB->SetState(replaceIsRegex);
		}

	if (vers >= 3)
		{
		bool singleLine, matchCase;
		input >> JBoolFromString(singleLine)
			  >> JBoolFromString(matchCase);
		itsSingleLineCB->SetState(singleLine);
		itsPreserveCaseCB->SetState(matchCase);
		}

	if (vers >= 7)
		{
		bool stayOpen, retainFocus;
		input >> JBoolFromString(stayOpen)
			  >> JBoolFromString(retainFocus);
		itsStayOpenCB->SetState(stayOpen);
		itsRetainFocusCB->SetState(retainFocus);
		}

	itsPrevSearchMenu->ReadSetup(input);
	itsPrevReplaceMenu->ReadSetup(input);
}

/******************************************************************************
 WriteSetup

 ******************************************************************************/

void
JXSearchTextDialog::WriteSetup
	(
	std::ostream& output
	)
	const
{
	output << kCurrentSetupVersion;

	output << ' ';
	GetWindow()->WriteGeometry(output);

	output << ' ' << JBoolToString(itsWrapSearchCB->IsChecked())
				  << JBoolToString(itsIgnoreCaseCB->IsChecked())
				  << JBoolToString(itsEntireWordCB->IsChecked())
				  << JBoolToString(itsSearchIsRegexCB->IsChecked())
				  << JBoolToString(itsReplaceIsRegexCB->IsChecked())
				  << JBoolToString(itsSingleLineCB->IsChecked())
				  << JBoolToString(itsPreserveCaseCB->IsChecked())
				  << JBoolToString(itsStayOpenCB->IsChecked())
				  << JBoolToString(itsRetainFocusCB->IsChecked());

	output << ' ';
	itsPrevSearchMenu->WriteSetup(output);

	output << ' ';
	itsPrevReplaceMenu->WriteSetup(output);

	output << ' ';
}

/******************************************************************************
 XSearch protocol

	This protocol is described at http://johnlindal.wixsite.com/xsearch

 ******************************************************************************/

static const JUtf8Byte* kAtomNames[ JXSearchTextDialog::kAtomCount ] =
{
	"XsearchSelection",
	"XsearchWindows",
	"XsearchVersion",
	"XsearchDataV1"
};

static const JUtf8Byte* kXSearchExtraTag = "JX_Application_Framework";

class JXSearchSelection : public JXSelectionData
{
public:

	JXSearchSelection(JXDisplay* display);
	virtual	~JXSearchSelection();

protected:

	virtual void		AddTypes(const Atom selectionName);
	virtual bool	ConvertData(const Atom requestType, Atom* returnType,
									unsigned char** data, JSize* dataLength,
									JSize* bitsPerBlock) const;
};

/******************************************************************************
 InitXSearch (private)

 ******************************************************************************/

void
JXSearchTextDialog::InitXSearch()
{
	JXDisplay* display      = GetDisplay();
	const Window rootWindow = display->GetRootWindow();

	display->RegisterXAtoms(kAtomCount, kAtomNames, itsAtoms);

	itsVersionWindow = None;
	itsDataWindow    = None;

	Display* d2 = XOpenDisplay(DisplayString(display->GetXDisplay()));
	if (d2 == nullptr)
		{
		return;
		}

	XSetWindowAttributes attr;
	attr.override_redirect = true;

	Visual* visual   = display->GetColorManager()->GetVisual();
	itsVersionWindow = XCreateWindow(d2, rootWindow, 0,0, 10,10,
									 0, CopyFromParent, InputOutput, visual,
									 CWOverrideRedirect, &attr);
	itsDataWindow    = XCreateWindow(d2, rootWindow, 0,0, 10,10,
									 0, CopyFromParent, InputOutput, visual,
									 CWOverrideRedirect, &attr);

	XSetCloseDownMode(d2, RetainPermanent);
	XCloseDisplay(d2);

	XGrabServer(*display);

	Atom actualType;
	int actualFormat;
	unsigned long itemCount, remainingBytes;
	unsigned char* data = nullptr;
	XGetWindowProperty(*display, rootWindow, itsAtoms[ kXSearchWindowsAtomIndex ],
					   0, 2, False, XA_WINDOW,
					   &actualType, &actualFormat,
					   &itemCount, &remainingBytes, &data);

	if (actualType == XA_WINDOW && actualFormat == 32 && itemCount == 2)
		{
		// destroy all resources so X server doesn't run out of client slots
		XKillClient(*display, itsVersionWindow);

		itsVersionWindow = *(reinterpret_cast<Window*>(data));
		itsDataWindow    = *(reinterpret_cast<Window*>(data) + 1);
		GetXSearch();
		}
	else
		{
		const Window newData[2] = { itsVersionWindow, itsDataWindow };
		XChangeProperty(*display, rootWindow,
						itsAtoms[ kXSearchWindowsAtomIndex ], XA_WINDOW, 32,
						PropModeReplace,
						(unsigned char*) newData, 2);

		SetXSearch(false);
		}

	XFree(data);

	XSelectInput(*display, itsVersionWindow, PropertyChangeMask);
	ListenTo(GetDisplay());
	itsIgnoreXSearchChangeFlag = false;

	XUngrabServer(*display);
}

/******************************************************************************
 GetXSearch (private)

 ******************************************************************************/

void
JXSearchTextDialog::GetXSearch()
{
	if (itsVersionWindow == None || itsDataWindow == None)
		{
		return;
		}

	JXDisplay* display       = GetDisplay();
	const Atom plainTextAtom = display->GetSelectionManager()->GetMimePlainTextXAtom();
	const Atom utf8Atom      = display->GetSelectionManager()->GetMimePlainTextUTF8XAtom();

	Atom actualType;
	int actualFormat;
	unsigned long itemCount, remainingBytes;
	unsigned char* data = nullptr;
	XGetWindowProperty(*display, itsVersionWindow, itsAtoms[ kXSearchVersionAtomIndex ],
					   0, 1, False, XA_ATOM,
					   &actualType, &actualFormat,
					   &itemCount, &remainingBytes, &data);

	if (actualType == XA_ATOM && actualFormat == 32 && itemCount == 1)
		{
		const Atom maxSourceVers = *(reinterpret_cast<Atom*>(data));
		const Atom vers          = JMin(maxSourceVers, (Atom) kCurrentXSearchVersion);
		XFree(data);

		const Atom requestType = itsAtoms[ kXSearchDataV1AtomIndex + (vers-1) ];
		XGetWindowProperty(*display, itsDataWindow, requestType,
						   0, LONG_MAX, False, plainTextAtom,
						   &actualType, &actualFormat,
						   &itemCount, &remainingBytes, &data);

		if (actualType == plainTextAtom && actualFormat == 8 && itemCount > 0)
			{
			const std::string s(reinterpret_cast<char*>(data), itemCount);
			std::istringstream input(s);
			ReadXSearch(input, vers);
			}
		else if (actualType == utf8Atom)
			{
			XGetWindowProperty(*display, itsDataWindow, requestType,
							   0, LONG_MAX, False, utf8Atom,
							   &actualType, &actualFormat,
							   &itemCount, &remainingBytes, &data);

			if (actualType == utf8Atom && actualFormat == 8 && itemCount > 0)
				{
				const std::string s(reinterpret_cast<char*>(data), itemCount);
				std::istringstream input(s);
				ReadXSearch(input, vers);
				}
			}
		}

	XFree(data);
}

/******************************************************************************
 SetXSearch (private)

 ******************************************************************************/

void
JXSearchTextDialog::SetXSearch
	(
	const bool grabServer
	)
	const
{
	if (itsVersionWindow == None || itsDataWindow == None)
		{
		return;
		}

	JXDisplay* display         = GetDisplay();
	JXSelectionManager* selMgr = display->GetSelectionManager();
	const Atom plainTextAtom   = selMgr->GetMimePlainTextXAtom();

	if (grabServer)
		{
		XGrabServer(*display);
		}

	// grab ownership of the selection
	// (We have to create a new object every time in case there is a
	//  SelectionClear event in the queue.)

	auto* selData = jnew JXSearchSelection(GetDisplay());
	assert( selData != nullptr );
	if (!selMgr->SetData(itsAtoms[ kXSearchSelectionAtomIndex ], selData))
		{
		if (grabServer)
			{
			XUngrabServer(*display);
			}
		return;
		}

	// version 1

	std::ostringstream dataV1;
	WriteXSearchV1(dataV1);
	const std::string s = dataV1.str();

	XChangeProperty(*display, itsDataWindow,
					itsAtoms[ kXSearchDataV1AtomIndex ], plainTextAtom, 8,
					PropModeReplace,
					(unsigned char*) s.c_str(), s.length());

	// broadcast change

	const Atom vers = kCurrentXSearchVersion;
	XChangeProperty(*display, itsVersionWindow,
					itsAtoms[ kXSearchVersionAtomIndex ], XA_ATOM, 32,
					PropModeReplace,
					(unsigned char*) &vers, 1);

	if (grabServer)
		{
		XUngrabServer(*display);
		}

	itsNeedXSearchBcastFlag = false;
}

/******************************************************************************
 ReadXSearch (private)

 ******************************************************************************/

void
JXSearchTextDialog::ReadXSearch
	(
	std::istream&	input,
	const Atom		vers
	)
{
	itsIgnoreXSearchChangeFlag = true;

	const JString searchText  = JReadUntil(input, '\0');
	const JString replaceText = JReadUntil(input, '\0');

	JUtf8Byte wrap, entireWord, entirePartialWord, caseSensitive;
	input >> wrap >> entireWord >> entirePartialWord >> caseSensitive;

	bool foundJX = false;
	JUtf8Byte searchIsRegex, singleLine;
	JUtf8Byte replaceIsRegex, preserveCase;
	if (!input.eof() && input.peek() == '\0')
		{
		input.ignore();

		const JString extraDataTag = JReadUntil(input, '\0');
		if (extraDataTag == kXSearchExtraTag)
			{
			JFileVersion vers1;
			input >> vers1;
			if (!input.fail() && vers1 == 1)
				{
				foundJX = true;

				input >> searchIsRegex >> singleLine;
				input >> replaceIsRegex >> preserveCase;
				}
			}
		}

	if (!input.fail())
		{
		JXWidget* fw;
		const bool hadFocus = GetWindow()->GetFocusWidget(&fw);

		SetSearchText(searchText);
		SetReplaceText(replaceText);
		SetStateForXSearch(itsWrapSearchCB, wrap,          false);
		SetStateForXSearch(itsEntireWordCB, entireWord,    false);
		SetStateForXSearch(itsIgnoreCaseCB, caseSensitive, true);

		if (foundJX)
			{
			SetStateForXSearch(itsSearchIsRegexCB,  searchIsRegex,  false);
			SetStateForXSearch(itsSingleLineCB,     singleLine,     false);
			SetStateForXSearch(itsReplaceIsRegexCB, replaceIsRegex, false);
			SetStateForXSearch(itsPreserveCaseCB,   preserveCase,   false);
			}

		if (hadFocus)
			{
			fw->Focus();
			}
		}

	itsIgnoreXSearchChangeFlag = false;
}

/******************************************************************************
 SetStateForXSearch (private)

 ******************************************************************************/

void
JXSearchTextDialog::SetStateForXSearch
	(
	JXTextCheckbox*	cb,
	const JUtf8Byte	state,
	const bool	negate
	)
{
	if ((!negate && state == 'T') ||
		( negate && state == 'F'))
		{
		cb->SetState(true);
		}
	else if ((!negate && state == 'F') ||
			 ( negate && state == 'T'))
		{
		cb->SetState(false);
		}
}

/******************************************************************************
 WriteXSearchV1 (private)

 ******************************************************************************/

void
JXSearchTextDialog::WriteXSearchV1
	(
	std::ostream& output
	)
	const
{
	(itsSearchInput->GetText()->GetText()).Print(output);
	output << '\0';
	(itsReplaceInput->GetText()->GetText()).Print(output);
	output << '\0';
	output << (itsWrapSearchCB->IsChecked() ? 'T' : 'F');
	output << (itsEntireWordCB->IsChecked() ? 'T' : 'F');
	output << 'X';	// entire partial word
	output << (itsIgnoreCaseCB->IsChecked() ? 'F' : 'T');
	output << '\0';
	output << kXSearchExtraTag;
	output << '\0';
	output << 1;
	output << (itsSearchIsRegexCB->IsChecked()  ? 'T' : 'F');
	output << (itsSingleLineCB->IsChecked()     ? 'T' : 'F');
	output << (itsReplaceIsRegexCB->IsChecked() ? 'T' : 'F');
	output << (itsPreserveCaseCB->IsChecked()   ? 'T' : 'F');
}

/******************************************************************************
 ReceiveWithFeedback (virtual protected)

 ******************************************************************************/

void
JXSearchTextDialog::ReceiveWithFeedback
	(
	JBroadcaster*	sender,
	Message*		message
	)
{
	if (sender == GetDisplay() && message->Is(JXDisplay::kXEventMessage))
		{
		auto* info =
			dynamic_cast<JXDisplay::XEventMessage*>(message);
		assert( info != nullptr );
		const XEvent& event = info->GetXEvent();
		if (event.type             == PropertyNotify &&
			event.xproperty.window == itsVersionWindow &&
			event.xproperty.atom   == itsAtoms[ kXSearchVersionAtomIndex ] &&
			event.xproperty.state  == PropertyNewValue)
			{
			if (!(GetDisplay()->GetSelectionManager())->
					OwnsSelection(itsAtoms[ kXSearchSelectionAtomIndex ]))
				{
				GetXSearch();
				}
			info->SetProcessed();
			return;
			}
		}

	JXWindowDirector::ReceiveWithFeedback(sender, message);
}

/******************************************************************************
 JXSearchSelection

	This class allows us to grab ownership of XsearchSelection.

 ******************************************************************************/

/******************************************************************************
 Constructor

 ******************************************************************************/

JXSearchSelection::JXSearchSelection
	(
	JXDisplay* display
	)
	:
	JXSelectionData(display)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXSearchSelection::~JXSearchSelection()
{
}

/******************************************************************************
 AddTypes (virtual protected)

 ******************************************************************************/

void
JXSearchSelection::AddTypes
	(
	const Atom selectionName
	)
{
}

/******************************************************************************
 ConvertData (virtual protected)

 ******************************************************************************/

bool
JXSearchSelection::ConvertData
	(
	const Atom		requestType,
	Atom*			returnType,
	unsigned char**	data,
	JSize*			dataLength,
	JSize*			bitsPerBlock
	)
	const
{
	*data         = nullptr;
	*dataLength   = 0;
	*returnType   = None;
	*bitsPerBlock = 8;
	return false;
}
