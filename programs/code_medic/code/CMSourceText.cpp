/******************************************************************************
 CMSourceText.cpp

	Implements a source code viewer.

	BASE CLASS = CMTextDisplayBase

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "CMSourceText.h"
#include "CMSourceDirector.h"
#include "CMCommandDirector.h"
#include "CMCommandInput.h"

#include "CBCStyler.h"
#include "cbmUtil.h"

#include "cmGlobals.h"
#include "cmActionDefs.h"

#include <JXApplication.h>
#include <JXDisplay.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <jASCIIConstants.h>
#include <jAssert.h>

// Search menu

#include "jcc_balance_braces.xpm"

static const JUtf8Byte* kSearchMenuStr =
	"    Balance closest grouping symbols %k Meta-B     %i" kCMBalanceGroupingSymbolAction
	"%l| Go to line...                    %k Meta-comma %i" kCMGoToLineAction
	"  | Go to current line               %k Meta-<     %i" kCMGoToCurrentLineAction;

// offsets from itsFirstSearchMenuItem

enum
{
	kBalanceCmd = 0,
	kGoToLineCmd,
	kGoToCurrentLineCmd
};

/******************************************************************************
 Constructor

 ******************************************************************************/

CMSourceText::CMSourceText
	(
	CMSourceDirector*	srcDir,
	CMCommandDirector*	cmdDir,
	JXMenuBar*			menuBar,
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	CMTextDisplayBase(jnew StyledText(this, enclosure->GetFontManager()),
					  kSelectableText, true, menuBar, scrollbarSet,
					  enclosure, hSizing, vSizing, x,y, w,h),
	itsSrcDir(srcDir),
	itsCmdDir(cmdDir),
	itsStyler(nullptr)
{
	SetBackColor(CMGetPrefsManager()->GetColor(CMPrefsManager::kBackColorIndex));
	SetSingleFocusWidget();

	JXTextMenu* searchMenu = AppendSearchMenu(menuBar);
	itsFirstSearchMenuItem = searchMenu->GetItemCount() + 1;
	searchMenu->ShowSeparatorAfter(itsFirstSearchMenuItem-1);
	searchMenu->AppendMenuItems(kSearchMenuStr, "CMSourceText");
	ListenTo(searchMenu);

	searchMenu->SetItemImage(itsFirstSearchMenuItem + kBalanceCmd, jcc_balance_braces);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMSourceText::~CMSourceText()
{
}

/******************************************************************************
 HandleKeyPress

 *****************************************************************************/

void
CMSourceText::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	if (c.IsPrint() ||
		c == kJDeleteKey || c == kJForwardDeleteKey ||
		c == kJReturnKey || c == kJTabKey)
		{
		itsCmdDir->TransferKeyPressToInput(c, keySym, modifiers);
		}
	else
		{
		CMTextDisplayBase::HandleKeyPress(c, keySym, modifiers);
		}
}

/******************************************************************************
 SetFileType

 ******************************************************************************/

void
CMSourceText::SetFileType
	(
	const CBTextFileType type
	)
{
	CBGetStyler(CBGetLanguage(type), &itsStyler);

	if (GetText()->GetText().IsEmpty())
		{
		return;
		}

	if (itsStyler != nullptr)
		{
		GetText()->RestyleAll();
		}
	else
		{
		JFont font = GetText()->GetFont(1);
		font.ClearStyle();
		SelectAll();
		SetCurrentFont(font);
		SetCaretLocation(1);
		}
}

/******************************************************************************
 CMSourceText::StyledText (protected)

 ******************************************************************************/

CMSourceText::StyledText::~StyledText()
{
	jdelete itsTokenStartList;
}

void
CMSourceText::StyledText::AdjustStylesBeforeBroadcast
	(
	const JString&		text,
	JRunArray<JFont>*	styles,
	TextRange*			recalcRange,
	TextRange*			redrawRange,
	const bool		deletion
	)
{
	if (itsOwner->itsStyler != nullptr)
		{
		itsOwner->itsStyler->UpdateStyles(this, text, styles,
										  recalcRange, redrawRange,
										  deletion, itsTokenStartList);
		}
	else
		{
		itsTokenStartList->RemoveAll();
		}
}

/******************************************************************************
 BoundsResized (virtual protected)

 ******************************************************************************/

void
CMSourceText::BoundsResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	CMTextDisplayBase::BoundsResized(dw,dh);

	if (!WillBreakCROnly() && HasSelection())
		{
		TEScrollToSelection(true);
		}
	else if (!WillBreakCROnly())
		{
		const JCoordinate line = itsSrcDir->GetCurrentExecLine();
		if (line > 0)
			{
			GoToLine(line);
			}
		}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
CMSourceText::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JXMouseButton b = button;
	if (button == kJXMiddleButton && clickCount == 2)
		{
		b = kJXLeftButton;
		}
	itsLastClickCount = clickCount;

	CMTextDisplayBase::HandleMouseDown(pt, b, clickCount, buttonStates, modifiers);
}

/******************************************************************************
 HandleMouseUp (virtual protected)

 ******************************************************************************/

void
CMSourceText::HandleMouseUp
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	CMTextDisplayBase::HandleMouseUp(pt, button, buttonStates, modifiers);
	if (button == kJXMiddleButton && itsLastClickCount == 2)
		{
		JString text;
		if (GetSelection(&text) && !text.Contains("\n"))
			{
			itsCmdDir->DisplayExpression(text);
			}
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CMSourceText::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	JXTextMenu* searchMenu;
	const bool ok = GetSearchMenu(&searchMenu);
	assert( ok );

	if (sender == searchMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateCustomSearchMenuItems();
		}
	else if (sender == searchMenu && message.Is(JXMenu::kItemSelected))
		{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		if (HandleCustomSearchMenuItems(selection->GetIndex()))
			{
			return;
			}
		}

	CMTextDisplayBase::Receive(sender, message);
}

/******************************************************************************
 UpdateCustomSearchMenuItems (private)

 ******************************************************************************/

void
CMSourceText::UpdateCustomSearchMenuItems()
{
	JXTextMenu* searchMenu;
	const bool ok = GetSearchMenu(&searchMenu);
	assert( ok );

	searchMenu->EnableItem(itsFirstSearchMenuItem + kGoToLineCmd);

	if (!GetText()->IsEmpty())
		{
		searchMenu->EnableItem(itsFirstSearchMenuItem + kBalanceCmd);

		if (itsSrcDir->IsMainSourceWindow())
			{
			searchMenu->EnableItem(itsFirstSearchMenuItem + kGoToCurrentLineCmd);
			}
		}
}

/******************************************************************************
 HandleCustomSearchMenuItems (private)

 ******************************************************************************/

bool
CMSourceText::HandleCustomSearchMenuItems
	(
	const JIndex origIndex
	)
{
	const JInteger index = JInteger(origIndex) - JInteger(itsFirstSearchMenuItem);
	Focus();

	if (index == kBalanceCmd)
		{
		CBMBalanceFromSelection(this, kCBCLang);
		return true;
		}

	else if (index == kGoToLineCmd)
		{
		AskForLine();
		return true;
		}
	else if (index == kGoToCurrentLineCmd)
		{
		const JCoordinate line = itsSrcDir->GetCurrentExecLine();
		if (line > 0)
			{
			GoToLine(line);
			}
		return true;
		}

	else
		{
		return false;
		}
}
