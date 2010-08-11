/******************************************************************************
 JXSearchTextDialog.cpp

	This is used as a global object.  We relay all the requests for action
	to the active text editor.  This way, all the text editors share the
	same dialog window.

	We also support the XSearch protocol so our settings can be shared with
	other programs.  (http://www.newplanetsoftware.com/xsearch/)

	Derived classes must override BuildWindow() and call SetObjects()
	at the end of their implementation.  They must also call
	JXSearchTextDialogX() after calling BuildWindow().

	To do:  support multiple displays

	BASE CLASS = JXWindowDirector

	Copyright © 1996-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXSearchTextDialog.h>
#include <JXSearchTextButton.h>
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXStaticText.h>
#include <JXInputField.h>
#include <JXTextButton.h>
#include <JXTextCheckbox.h>
#include <JXRadioGroup.h>
#include <JXTextRadioButton.h>
#include <JXStringHistoryMenu.h>
#include <JXHelpManager.h>
#include <JXTimerTask.h>
#include <JXMenu.h>
#include <JXDownRect.h>
#include <JXColormap.h>
#include <jXGlobals.h>
#include <JRegex.h>
#include <JInterpolate.h>
#include <jStreamUtil.h>
#include <sstream>
#include <jAssert.h>

static const JCharacter* kJXSearchTextHelpName = "JXSearchTextHelp";
static const JCharacter* kJXRegexHelpName      = "JXRegexHelp";
static const JCharacter* kJXRegexQRefHelpName  = "JXRegexQRef";

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
JXSearchTextDialog::Create
	(
	const JCharacter* searchTextHelpName
	)
{
	JXSearchTextDialog* dlog = new JXSearchTextDialog(searchTextHelpName);
	assert( dlog != NULL );
	dlog->BuildWindow();
	dlog->JXSearchTextDialogX();
	return dlog;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXSearchTextDialog::JXSearchTextDialog
	(
	const JCharacter* searchTextHelpName
	)
	:
	JXWindowDirector(JXGetPersistentWindowOwner())
{
	itsTE = NULL;

	itsRegex = new JRegex;
	assert( itsRegex != NULL );
	itsRegex->SetLineBegin(kJTrue);				// ^ matches beginning of line, not text
	itsRegex->SetLineEnd(kJTrue);				// $ matches end of line, not text
	(itsRegex->GetMatchInterpolator())->SetWhitespaceEscapes();

	itsUpdateTask = new JXTimerTask(kUpdatePeriod);
	assert( itsUpdateTask != NULL );
	ListenTo(itsUpdateTask);

	itsNeedXSearchBcastFlag    = kJFalse;
	itsIgnoreXSearchChangeFlag = kJTrue;			// until InitXSearch() is called

	itsVersionWindow = None;
	itsDataWindow    = None;

	JXSetSearchTextDialog(this);
	SetSearchTextHelpName(searchTextHelpName);

	(JXGetHelpManager())->RegisterSection(kJXSearchTextHelpName);
	(JXGetHelpManager())->RegisterSection(kJXRegexHelpName);
	(JXGetHelpManager())->RegisterSection(kJXRegexQRefHelpName);
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
	delete itsUpdateTask;
	delete itsRegex;
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

		(JXGetApplication())->InstallIdleTask(itsUpdateTask);
		}
}

/******************************************************************************
 Deactivate (virtual)

 ******************************************************************************/

JBoolean
JXSearchTextDialog::Deactivate()
{
	if (JXWindowDirector::Deactivate())
		{
		if (itsNeedXSearchBcastFlag)
			{
			SetXSearch();
			}

		(JXGetApplication())->RemoveIdleTask(itsUpdateTask);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 SetSearchText

 ******************************************************************************/

void
JXSearchTextDialog::SetSearchText
	(
	const JCharacter* str
	)
{
	if (strcmp(str, itsSearchInput->GetText()) != 0)	// exact compare, to avoid ignoring diacritical marks
		{
		itsSearchInput->SetText(str);
		}
	itsSearchInput->Focus();
	itsSearchInput->SelectAll();
//	itsPrevSearchMenu->AddString(str);
	itsSearchIsRegexCB->SetState(kJFalse);
}

/******************************************************************************
 SetReplaceText

 ******************************************************************************/

void
JXSearchTextDialog::SetReplaceText
	(
	const JCharacter* str
	)
{
	if (strcmp(str, itsReplaceInput->GetText()) != 0)	// exact compare, to avoid ignoring diacritical marks
		{
		itsReplaceInput->SetText(str);
		}
	itsReplaceInput->Focus();
	itsReplaceInput->SelectAll();
//	itsPrevReplaceMenu->AddString(str);
	itsReplaceIsRegexCB->SetState(kJFalse);
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JXSearchTextDialog::BuildWindow()
{
// begin JXLayout

    JXWindow* window = new JXWindow(this, 450,310, "");
    assert( window != NULL );

    itsCloseButton =
        new JXTextButton(JGetString("itsCloseButton::JXSearchTextDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 320,280, 80,20);
    assert( itsCloseButton != NULL );
    itsCloseButton->SetShortcuts(JGetString("itsCloseButton::JXSearchTextDialog::shortcuts::JXLayout"));

    JXStaticText* obj1_JXLayout =
        new JXStaticText(JGetString("obj1_JXLayout::JXSearchTextDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,30, 80,20);
    assert( obj1_JXLayout != NULL );
    obj1_JXLayout->SetToLabel();

    JXStaticText* obj2_JXLayout =
        new JXStaticText(JGetString("obj2_JXLayout::JXSearchTextDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,100, 90,20);
    assert( obj2_JXLayout != NULL );
    obj2_JXLayout->SetToLabel();

    itsIgnoreCaseCB =
        new JXTextCheckbox(JGetString("itsIgnoreCaseCB::JXSearchTextDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,180, 130,20);
    assert( itsIgnoreCaseCB != NULL );
    itsIgnoreCaseCB->SetShortcuts(JGetString("itsIgnoreCaseCB::JXSearchTextDialog::shortcuts::JXLayout"));

    itsWrapSearchCB =
        new JXTextCheckbox(JGetString("itsWrapSearchCB::JXSearchTextDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,240, 130,20);
    assert( itsWrapSearchCB != NULL );
    itsWrapSearchCB->SetShortcuts(JGetString("itsWrapSearchCB::JXSearchTextDialog::shortcuts::JXLayout"));

    itsReplaceButton =
        new JXTextButton(JGetString("itsReplaceButton::JXSearchTextDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 300,50, 140,20);
    assert( itsReplaceButton != NULL );
    itsReplaceButton->SetShortcuts(JGetString("itsReplaceButton::JXSearchTextDialog::shortcuts::JXLayout"));

    itsEntireWordCB =
        new JXTextCheckbox(JGetString("itsEntireWordCB::JXSearchTextDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,210, 130,20);
    assert( itsEntireWordCB != NULL );
    itsEntireWordCB->SetShortcuts(JGetString("itsEntireWordCB::JXSearchTextDialog::shortcuts::JXLayout"));

    itsPrevReplaceMenu =
        new JXStringHistoryMenu(kHistoryLength, "", window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 250,130, 30,20);
    assert( itsPrevReplaceMenu != NULL );

    itsPrevSearchMenu =
        new JXStringHistoryMenu(kHistoryLength, "", window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 250,60, 30,20);
    assert( itsPrevSearchMenu != NULL );

    itsHelpButton =
        new JXTextButton(JGetString("itsHelpButton::JXSearchTextDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 60,280, 80,20);
    assert( itsHelpButton != NULL );
    itsHelpButton->SetShortcuts(JGetString("itsHelpButton::JXSearchTextDialog::shortcuts::JXLayout"));

    itsSearchIsRegexCB =
        new JXTextCheckbox(JGetString("itsSearchIsRegexCB::JXSearchTextDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 160,180, 150,20);
    assert( itsSearchIsRegexCB != NULL );
    itsSearchIsRegexCB->SetShortcuts(JGetString("itsSearchIsRegexCB::JXSearchTextDialog::shortcuts::JXLayout"));

    itsReplaceIsRegexCB =
        new JXTextCheckbox(JGetString("itsReplaceIsRegexCB::JXSearchTextDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 320,180, 110,20);
    assert( itsReplaceIsRegexCB != NULL );
    itsReplaceIsRegexCB->SetShortcuts(JGetString("itsReplaceIsRegexCB::JXSearchTextDialog::shortcuts::JXLayout"));

    itsSingleLineCB =
        new JXTextCheckbox(JGetString("itsSingleLineCB::JXSearchTextDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 160,210, 150,20);
    assert( itsSingleLineCB != NULL );
    itsSingleLineCB->SetShortcuts(JGetString("itsSingleLineCB::JXSearchTextDialog::shortcuts::JXLayout"));

    itsPreserveCaseCB =
        new JXTextCheckbox(JGetString("itsPreserveCaseCB::JXSearchTextDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 320,210, 110,20);
    assert( itsPreserveCaseCB != NULL );
    itsPreserveCaseCB->SetShortcuts(JGetString("itsPreserveCaseCB::JXSearchTextDialog::shortcuts::JXLayout"));

    itsQRefButton =
        new JXTextButton(JGetString("itsQRefButton::JXSearchTextDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 190,280, 80,20);
    assert( itsQRefButton != NULL );

    JXStaticText* obj3_JXLayout =
        new JXStaticText(JGetString("obj3_JXLayout::JXSearchTextDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 355,20, 40,20);
    assert( obj3_JXLayout != NULL );
    obj3_JXLayout->SetToLabel();

    itsFindBackButton =
        new JXSearchTextButton(kJFalse, window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 300,20, 20,20);
    assert( itsFindBackButton != NULL );

    itsFindFwdButton =
        new JXSearchTextButton(kJTrue, window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 420,20, 20,20);
    assert( itsFindFwdButton != NULL );

    JXStaticText* obj4_JXLayout =
        new JXStaticText(JGetString("obj4_JXLayout::JXSearchTextDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 323,80, 100,20);
    assert( obj4_JXLayout != NULL );
    obj4_JXLayout->SetToLabel();

    itsReplaceFindBackButton =
        new JXSearchTextButton(kJFalse, window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 300,80, 20,20);
    assert( itsReplaceFindBackButton != NULL );

    itsReplaceFindFwdButton =
        new JXSearchTextButton(kJTrue, window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 420,80, 20,20);
    assert( itsReplaceFindFwdButton != NULL );

    JXStaticText* obj5_JXLayout =
        new JXStaticText(JGetString("obj5_JXLayout::JXSearchTextDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 335,110, 80,20);
    assert( obj5_JXLayout != NULL );
    obj5_JXLayout->SetToLabel();

    itsReplaceAllBackButton =
        new JXSearchTextButton(kJFalse, window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 300,110, 20,20);
    assert( itsReplaceAllBackButton != NULL );

    itsReplaceAllFwdButton =
        new JXSearchTextButton(kJTrue, window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 420,110, 20,20);
    assert( itsReplaceAllFwdButton != NULL );

    itsReplaceAllInSelButton =
        new JXTextButton(JGetString("itsReplaceAllInSelButton::JXSearchTextDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 300,140, 140,20);
    assert( itsReplaceAllInSelButton != NULL );

    itsStayOpenCB =
        new JXTextCheckbox(JGetString("itsStayOpenCB::JXSearchTextDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 160,240, 150,20);
    assert( itsStayOpenCB != NULL );

    itsRetainFocusCB =
        new JXTextCheckbox(JGetString("itsRetainFocusCB::JXSearchTextDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 320,240, 110,20);
    assert( itsRetainFocusCB != NULL );

    itsSearchInput =
        new JXInputField(kJTrue, kJFalse, window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 20,50, 220,40);
    assert( itsSearchInput != NULL );

    itsReplaceInput =
        new JXInputField(kJTrue, kJFalse, window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 20,120, 220,40);
    assert( itsReplaceInput != NULL );

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
			   itsReplaceAllFwdButton, itsReplaceAllBackButton,
			   itsReplaceAllInSelButton,
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
	JXSearchTextButton*		replaceAllFwdButton,
	JXSearchTextButton*		replaceAllBackButton,
	JXTextButton*			replaceAllInSelButton,

	JXTextButton*			closeButton,
	JXTextButton*			helpButton,
	JXTextButton*			qRefButton
	)
{
	JXWindow* window = searchInput->GetWindow();
	window->SetTitle("Search text");
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();
	window->ShouldFocusWhenShow(kJTrue);
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
	itsReplaceAllFwdButton   = replaceAllFwdButton;
	itsReplaceAllBackButton  = replaceAllBackButton;
	itsReplaceAllInSelButton = replaceAllInSelButton;

	itsCloseButton = closeButton;
	itsHelpButton  = helpButton;
	itsQRefButton  = qRefButton;

	SetFont(JGetMonospaceFontName(), kJDefaultMonoFontSize);

	// decor

	const JRect wFrame  = window->GetFrame();
	const JRect soFrame = itsStayOpenCB->GetFrame();

	JXDownRect* line =
		new JXDownRect(window, JXWidget::kFixedLeft, JXWidget::kFixedTop,
					   soFrame.left, soFrame.top-6,
					   wFrame.right-10-soFrame.left, 2);
	assert( line != NULL );
	line->SetBorderWidth(1);

	// shortcuts

    itsCloseButton->SetShortcuts("#W^[");
    itsIgnoreCaseCB->SetShortcuts("#I");
    itsWrapSearchCB->SetShortcuts("#S");
    itsEntireWordCB->SetShortcuts("#E");
    itsSearchIsRegexCB->SetShortcuts("#X");
    itsSingleLineCB->SetShortcuts("#T");
    itsPreserveCaseCB->SetShortcuts("#P");

	if (JXMenu::GetDefaultStyle() == JXMenu::kMacintoshStyle)
		{
		itsFindFwdButton->SetShortcuts("#G");
		itsFindFwdButton->SetHint("Return / Meta-G");

		itsFindBackButton->SetHint("Shift-Return / Meta-Shift-G");

	    itsReplaceButton->SetShortcuts("#R#=");
		itsReplaceButton->SetHint("Meta-R / Meta-=");

		itsReplaceFindFwdButton->SetShortcuts("#L");
		itsReplaceFindFwdButton->SetHint("Meta-L");

		itsReplaceFindBackButton->SetHint("Meta-Shift-L");
		}
	else
		{
		itsFindFwdButton->SetShortcuts("^G");
		itsFindFwdButton->SetHint("Return / Ctrl-G");

		itsFindBackButton->SetHint("Shift-Return / Ctrl-Shift-G");

	    itsReplaceButton->SetShortcuts("#R^=");
		itsReplaceButton->SetHint("Meta-R / Ctrl-=");

		itsReplaceFindFwdButton->SetShortcuts("^L");
		itsReplaceFindFwdButton->SetHint("Ctrl-L");

		itsReplaceFindBackButton->SetHint("Ctrl-Shift-L");
		}

	JXKeyModifiers modifiers(GetDisplay());
	window->InstallShortcut(itsFindFwdButton, JXCtrl('M'), modifiers);

	modifiers.SetState(kJXShiftKeyIndex, kJTrue);
	window->InstallShortcut(itsFindBackButton, JXCtrl('M'), modifiers);

	modifiers.SetState(kJXMetaKeyIndex, kJTrue);
    itsReplaceIsRegexCB->SetShortcuts("#X");		// trick to underline x
	window->ClearShortcuts(itsReplaceIsRegexCB);
	window->InstallShortcut(itsReplaceIsRegexCB, 'x', modifiers);
	window->InstallShortcut(itsReplaceIsRegexCB, 'X', modifiers);

	if (JXMenu::GetDefaultStyle() != JXMenu::kMacintoshStyle)
		{
		modifiers.SetState(kJXMetaKeyIndex, kJFalse);
		modifiers.SetState(kJXControlKeyIndex, kJTrue);
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
	ListenTo(itsReplaceAllFwdButton);
	ListenTo(itsReplaceAllBackButton);
	ListenTo(itsReplaceAllInSelButton);

	ListenTo(itsHelpButton);
	ListenTo(itsQRefButton);
	ListenTo(itsCloseButton);

	ListenTo(itsPrevSearchMenu);
	ListenTo(itsPrevReplaceMenu);

	itsIgnoreCaseCB->SetState(kJTrue);
	itsWrapSearchCB->SetState(kJTrue);

	itsStayOpenCB->SetState(kJTrue);
	itsRetainFocusCB->SetState(kJTrue);

	// listen after setting value

	ListenTo(itsStayOpenCB);

	// XSearch data

	itsSearchInput->ShouldBroadcastAllTextChanged(kJTrue);
	ListenTo(itsSearchInput);
	ListenTo(itsSearchIsRegexCB);
	ListenTo(itsSingleLineCB);

	itsReplaceInput->ShouldBroadcastAllTextChanged(kJTrue);
	ListenTo(itsReplaceInput);
	ListenTo(itsReplaceIsRegexCB);
	ListenTo(itsPreserveCaseCB);

	ListenTo(itsWrapSearchCB);
	ListenTo(itsEntireWordCB);
	ListenTo(itsIgnoreCaseCB);
}

/******************************************************************************
 Set help text

 ******************************************************************************/

void
JXSearchTextDialog::SetSearchTextHelpName
	(
	const JCharacter* name
	)
{
	itsSearchTextHelpName = (JStringEmpty(name) ? kJXSearchTextHelpName : name);
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
	JBoolean found = kJFalse;

	JXWindowDirector* director = NULL;
	if (itsTE != NULL)
		{
		director = (itsTE->GetWindow())->GetDirector();
		}

	if (sender == itsFindFwdButton && message.Is(JXButton::kPushed))
		{
		if (itsTE != NULL && itsTE->SearchForward())
			{
			found = kJTrue;
			director->Activate();
			}
		}
	else if (sender == itsFindBackButton && message.Is(JXButton::kPushed))
		{
		if (itsTE != NULL && itsTE->SearchBackward())
			{
			found = kJTrue;
			director->Activate();
			}
		}

	else if (sender == itsReplaceButton && message.Is(JXButton::kPushed))
		{
		if (itsTE != NULL && itsTE->ReplaceSelection())
			{
			found = kJTrue;
			director->Activate();
			}
		}

	else if (sender == itsReplaceFindFwdButton && message.Is(JXButton::kPushed))
		{
		if (itsTE != NULL && itsTE->ReplaceAndSearchForward())
			{
			found = kJTrue;
			director->Activate();
			}
		}
	else if (sender == itsReplaceFindBackButton && message.Is(JXButton::kPushed))
		{
		if (itsTE != NULL && itsTE->ReplaceAndSearchBackward())
			{
			found = kJTrue;
			director->Activate();
			}
		}

	else if (sender == itsReplaceAllFwdButton && message.Is(JXButton::kPushed))
		{
		if (itsTE != NULL && itsTE->ReplaceAllForward())
			{
			found = kJTrue;
			director->Activate();
			}
		}
	else if (sender == itsReplaceAllBackButton && message.Is(JXButton::kPushed))
		{
		if (itsTE != NULL && itsTE->ReplaceAllBackward())
			{
			found = kJTrue;
			director->Activate();
			}
		}
	else if (sender == itsReplaceAllInSelButton && message.Is(JXButton::kPushed))
		{
		if (itsTE != NULL && itsTE->ReplaceAllInSelection())
			{
			found = kJTrue;
			director->Activate();
			}
		}

	else if (sender == itsHelpButton && message.Is(JXButton::kPushed))
		{
		(JXGetHelpManager())->ShowSection(itsSearchTextHelpName);
		}
	else if (sender == itsQRefButton && message.Is(JXButton::kPushed))
		{
		(JXGetHelpManager())->ShowSection(kJXRegexQRefHelpName);
		}

	else if (sender == itsCloseButton && message.Is(JXButton::kPushed))
		{
		Deactivate();
		}

	else if (sender == itsPrevSearchMenu && message.Is(JXMenu::kItemSelected))
		{
		// need local copy since menu is changed
		const JString str       = itsPrevSearchMenu->GetItemText(message);
		const JBoolean wasRegex = itsSearchIsRegexCB->IsChecked();
		SetSearchText(str);
		itsSearchIsRegexCB->SetState(wasRegex);
		}
	else if (sender == itsPrevReplaceMenu && message.Is(JXMenu::kItemSelected))
		{
		// need local copy since menu is changed
		const JString str       = itsPrevReplaceMenu->GetItemText(message);
		const JBoolean wasRegex = itsReplaceIsRegexCB->IsChecked();
		SetReplaceText(str);
		itsReplaceIsRegexCB->SetState(wasRegex);
		}

	else if (sender == GetWindow() && message.Is(JXWindow::kIconified))
		{
		if ((GetWindow())->IsVisible())
			{
			(JXGetApplication())->RemoveIdleTask(itsUpdateTask);
			}
		}
	else if (sender == GetWindow() && message.Is(JXWindow::kDeiconified))
		{
		if ((GetWindow())->IsVisible())
			{
			(JXGetApplication())->InstallIdleTask(itsUpdateTask);
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
			(message.Is(JTextEditor::kTextSet) ||
			 (message.Is(JXWidget::kLostFocus) && itsNeedXSearchBcastFlag)))
			{
			SetXSearch();
			}
		else if (!itsIgnoreXSearchChangeFlag && message.Is(JTextEditor::kTextChanged))
			{
			itsNeedXSearchBcastFlag = kJTrue;	// wait for them to use it
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

JBoolean
JXSearchTextDialog::HasSearchText()
	const
{
	return !itsSearchInput->IsEmpty();
}

/******************************************************************************
 HasReplaceText

 ******************************************************************************/

JBoolean
JXSearchTextDialog::HasReplaceText()
	const
{
	return !itsReplaceInput->IsEmpty();
}

/******************************************************************************
 GetSearchParameters

	Without regex, "entire word" is only possible if searchStr contains
	a single word.

	Returns kJFalse if an error occurs.  (e.g. if regex compile fails)

 ******************************************************************************/

JBoolean
JXSearchTextDialog::GetSearchParameters
	(
	JString*	searchStr,
	JBoolean*	searchIsRegex,
	JBoolean*	caseSensitive,
	JBoolean*	entireWord,
	JBoolean*	wrapSearch,

	JString*	replaceStr,
	JBoolean*	replaceIsRegex,
	JBoolean*	preserveCase,

	JRegex**	regex
	)
	const
{
	if (!HasSearchText())
		{
		(JGetUserNotification())->ReportError("You must enter something for which to search.");
		itsSearchInput->Focus();
		return kJFalse;
		}

	*wrapSearch    = itsWrapSearchCB->IsChecked();
	*caseSensitive = !itsIgnoreCaseCB->IsChecked();
	*preserveCase  = itsPreserveCaseCB->IsChecked();

	*searchStr     = itsSearchInput->GetText();
	*searchIsRegex = itsSearchIsRegexCB->IsChecked();
	if (*searchIsRegex)
		{
		itsRegex->SetCaseSensitive(*caseSensitive);
		itsRegex->SetSingleLine(itsSingleLineCB->IsChecked());
		const JError err = itsRegex->SetPattern(*searchStr);
		if (!err.OK())
			{
			itsSearchInput->Focus();
			err.ReportIfError();
			return kJFalse;
			}
		}

	*replaceStr     = itsReplaceInput->GetText();
	*replaceIsRegex = itsReplaceIsRegexCB->IsChecked();
	if (*replaceIsRegex)
		{
		itsRegex->SetMatchCase(*preserveCase);
		JIndexRange errRange;
		const JError err = itsRegex->SetReplacePattern(*replaceStr, &errRange);
		if (!err.OK())
			{
			itsReplaceInput->Focus();
			itsReplaceInput->SetSelection(errRange);
			err.ReportIfError();
			return kJFalse;
			}
		}

	if (*searchIsRegex ||
		itsSearchInput->IsEntireWord(1, itsSearchInput->GetTextLength()))
		{
		*entireWord = itsEntireWordCB->IsChecked();
		}
	else
		{
		*entireWord = kJFalse;
		}

	*regex = itsRegex;

	// use the original strings, not the substituted ones

	itsPrevSearchMenu->AddString(itsSearchInput->GetText());
	itsPrevReplaceMenu->AddString(itsReplaceInput->GetText());

	// broadcast to other programs who support Xsearch protocol

	if (itsNeedXSearchBcastFlag)
		{
		SetXSearch();
		}

	return kJTrue;
}

/******************************************************************************
 UpdateDisplay (virtual protected)

	We never need to call this because our update task triggers it regularly.

 ******************************************************************************/

void
JXSearchTextDialog::UpdateDisplay()
{
	const JBoolean hasSearchText = HasSearchText();
	const JBoolean teWritable =
		JConvertToBoolean( itsTE != NULL && itsTE->GetType() == JTextEditor::kFullEditor );

	const JBoolean canSearch = JI2B(itsTE != NULL && !itsTE->IsEmpty() && hasSearchText);
	itsFindFwdButton->SetActive(canSearch);
	itsFindBackButton->SetActive(canSearch);

	const JBoolean canReplace = JI2B(canSearch && teWritable && itsTE->HasSelection());
	itsReplaceButton->SetActive(canReplace);
	itsReplaceFindFwdButton->SetActive(canReplace);
	itsReplaceFindBackButton->SetActive(canReplace);

	const JBoolean canReplaceAll = JI2B(canSearch && teWritable);
	itsReplaceAllFwdButton->SetActive(canReplaceAll);
	itsReplaceAllBackButton->SetActive(canReplaceAll);

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
	const JCharacter*	name,
	const JSize			size
	)
{
	itsSearchInput->SetFont(name, size);
	itsPrevSearchMenu->SetDefaultFont(name, size, JFontStyle(), kJTrue);

	itsReplaceInput->SetFont(name, size);
	itsPrevReplaceMenu->SetDefaultFont(name, size, JFontStyle(), kJTrue);
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
JXSearchTextDialog::ReadSetup
	(
	istream& input
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

	JBoolean wrapSearch, ignoreCase, entireWord, searchIsRegex;
	input >> wrapSearch >> ignoreCase >> entireWord >> searchIsRegex;
	itsWrapSearchCB->SetState(wrapSearch);
	itsIgnoreCaseCB->SetState(ignoreCase);
	itsEntireWordCB->SetState(entireWord);
	itsSearchIsRegexCB->SetState(searchIsRegex);

	if (vers >= 2)
		{
		JBoolean replaceIsRegex;
		input >> replaceIsRegex;
		itsReplaceIsRegexCB->SetState(replaceIsRegex);
		}

	if (vers >= 3)
		{
		JBoolean singleLine, matchCase;
		input >> singleLine >> matchCase;
		itsSingleLineCB->SetState(singleLine);
		itsPreserveCaseCB->SetState(matchCase);
		}

	if (vers >= 7)
		{
		JBoolean stayOpen, retainFocus;
		input >> stayOpen >> retainFocus;
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
	ostream& output
	)
	const
{
	output << kCurrentSetupVersion;

	output << ' ';
	(GetWindow())->WriteGeometry(output);

	output << ' ' << itsWrapSearchCB->IsChecked();
	output << ' ' << itsIgnoreCaseCB->IsChecked();
	output << ' ' << itsEntireWordCB->IsChecked();
	output << ' ' << itsSearchIsRegexCB->IsChecked();
	output << ' ' << itsReplaceIsRegexCB->IsChecked();
	output << ' ' << itsSingleLineCB->IsChecked();
	output << ' ' << itsPreserveCaseCB->IsChecked();
	output << ' ' << itsStayOpenCB->IsChecked();
	output << ' ' << itsRetainFocusCB->IsChecked();

	output << ' ';
	itsPrevSearchMenu->WriteSetup(output);

	output << ' ';
	itsPrevReplaceMenu->WriteSetup(output);

	output << ' ';
}

/******************************************************************************
 XSearch protocol

	This protocol is described at http://www.newplanetsoftware.com/xsearch/

 ******************************************************************************/

static const JCharacter* kAtomNames[ JXSearchTextDialog::kAtomCount ] =
{
	"XsearchSelection",
	"XsearchWindows",
	"XsearchVersion",
	"XsearchDataV1"
};

static const JCharacter* kXSearchExtraTag = "JX_Application_Framework";

class JXSearchSelection : public JXSelectionData
{
public:

	JXSearchSelection(JXDisplay* display);
	virtual	~JXSearchSelection();

protected:

	virtual void		AddTypes(const Atom selectionName);
	virtual JBoolean	ConvertData(const Atom requestType, Atom* returnType,
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
	if (d2 == NULL)
		{
		return;
		}

	XSetWindowAttributes attr;
	attr.override_redirect = kJTrue;

	Visual* visual   = (GetColormap())->GetVisual();
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
	unsigned char* data = NULL;
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

		SetXSearch(kJFalse);
		}

	XFree(data);

	XSelectInput(*display, itsVersionWindow, PropertyChangeMask);
	ListenTo(GetDisplay());
	itsIgnoreXSearchChangeFlag = kJFalse;

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
	const Atom plainTextAtom = (display->GetSelectionManager())->GetMimePlainTextXAtom();

	Atom actualType;
	int actualFormat;
	unsigned long itemCount, remainingBytes;
	unsigned char* data = NULL;
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
		}

	XFree(data);
}

/******************************************************************************
 SetXSearch (private)

 ******************************************************************************/

void
JXSearchTextDialog::SetXSearch
	(
	const JBoolean grabServer
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

	JXSearchSelection* selData = new JXSearchSelection(GetDisplay());
	assert( selData != NULL );
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

	itsNeedXSearchBcastFlag = kJFalse;
}

/******************************************************************************
 ReadXSearch (private)

 ******************************************************************************/

void
JXSearchTextDialog::ReadXSearch
	(
	istream&	input,
	const Atom	vers
	)
{
	itsIgnoreXSearchChangeFlag = kJTrue;

	const JString searchText  = JReadUntil(input, '\0');
	const JString replaceText = JReadUntil(input, '\0');

	JCharacter wrap, entireWord, entirePartialWord, caseSensitive;
	input >> wrap >> entireWord >> entirePartialWord >> caseSensitive;

	JBoolean foundJX = kJFalse;
	JCharacter searchIsRegex, singleLine;
	JCharacter replaceIsRegex, preserveCase;
	if (!input.eof() && input.peek() == '\0')
		{
		input.ignore();

		const JString extraDataTag = JReadUntil(input, '\0');
		if (extraDataTag == kXSearchExtraTag)
			{
			JFileVersion vers;
			input >> vers;
			if (!input.fail() && vers == 1)
				{
				foundJX = kJTrue;

				input >> searchIsRegex >> singleLine;
				input >> replaceIsRegex >> preserveCase;
				}
			}
		}

	if (!input.fail())
		{
		JXWidget* fw;
		const JBoolean hadFocus = (GetWindow())->GetFocusWidget(&fw);

		SetSearchText(searchText);
		SetReplaceText(replaceText);
		SetStateForXSearch(itsWrapSearchCB, wrap,          kJFalse);
		SetStateForXSearch(itsEntireWordCB, entireWord,    kJFalse);
		SetStateForXSearch(itsIgnoreCaseCB, caseSensitive, kJTrue);

		if (foundJX)
			{
			SetStateForXSearch(itsSearchIsRegexCB,  searchIsRegex,  kJFalse);
			SetStateForXSearch(itsSingleLineCB,     singleLine,     kJFalse);
			SetStateForXSearch(itsReplaceIsRegexCB, replaceIsRegex, kJFalse);
			SetStateForXSearch(itsPreserveCaseCB,   preserveCase,   kJFalse);
			}

		if (hadFocus)
			{
			fw->Focus();
			}
		}

	itsIgnoreXSearchChangeFlag = kJFalse;
}

/******************************************************************************
 SetStateForXSearch (private)

 ******************************************************************************/

void
JXSearchTextDialog::SetStateForXSearch
	(
	JXTextCheckbox*		cb,
	const JCharacter	state,
	const JBoolean		negate
	)
{
	if ((!negate && state == 'T') ||
		( negate && state == 'F'))
		{
		cb->SetState(kJTrue);
		}
	else if ((!negate && state == 'F') ||
			 ( negate && state == 'T'))
		{
		cb->SetState(kJFalse);
		}
}

/******************************************************************************
 WriteXSearchV1 (private)

 ******************************************************************************/

void
JXSearchTextDialog::WriteXSearchV1
	(
	ostream& output
	)
	const
{
	(itsSearchInput->GetText()).Print(output);
	output << '\0';
	(itsReplaceInput->GetText()).Print(output);
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
		JXDisplay::XEventMessage* info =
			dynamic_cast(JXDisplay::XEventMessage*, message);
		assert( info != NULL );
		const XEvent& event = info->GetXEvent();
		if (event.type             == PropertyNotify &&
			event.xproperty.window == itsVersionWindow &&
			event.xproperty.atom   == itsAtoms[ kXSearchVersionAtomIndex ] &&
			event.xproperty.state  == PropertyNewValue)
			{
			if (!((GetDisplay())->GetSelectionManager())->
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

JBoolean
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
	*data         = NULL;
	*dataLength   = 0;
	*returnType   = None;
	*bitsPerBlock = 8;
	return kJFalse;
}
