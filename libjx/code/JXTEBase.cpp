/******************************************************************************
 JXTEBase.cpp

	We implement the routines required by JTextEditor that only depend on JX.

	We paste the following data types:
		XA_STRING, TEXT, text/x-jxstyled0

	We accept drops of the following data types:
		text/plain, text/x-jxstyled0

	To paste other formats, override TEGetExternalClipboard().

	To accept drops of other formats that can be pasted (and should therefore
		display an insertion caret), override TEXWillAcceptDrop() and
		TEXConvertDropData().

	To accept drops of other formats that cannot be pasted, override
		WillAcceptDrop() and -all- four HandleDND*() functions.

	BASE CLASS = JXScrollableWidget, JTextEditor

	Copyright (C) 1996-99 by John Lindal.

 ******************************************************************************/

#include "JXTEBase.h"
#include "JXSearchTextDialog.h"
#include "JXTEBlinkCaretTask.h"
#include "JXGoToLineDialog.h"
#include "JXDisplay.h"
#include "JXWindow.h"
#include "JXMenuBar.h"
#include "JXTextMenu.h"
#include "JXScrollbar.h"
#include "JXWindowPainter.h"
#include "JXPSPrinter.h"
#include "JXPTPrinter.h"
#include "JXSpellChecker.h"
#include "JXSelectionManager.h"
#include "JXDNDManager.h"
#include "JXTextSelection.h"
#include "JXColorManager.h"
#include "jXGlobals.h"
#include "jXActionDefs.h"
#include "jXKeysym.h"

#include <JFontManager.h>
#include <JRegex.h>
#include <jASCIIConstants.h>
#include <jStreamUtil.h>
#include <jFileUtil.h>
#include <jProcessUtil.h>
#include <jTime.h>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <jAssert.h>

JXTEBase::PartialWordModifier JXTEBase::thePWMod = JXTEBase::kCtrlMetaPWMod;
JBoolean JXTEBase::theWindowsHomeEndFlag         = kJTrue;
JBoolean JXTEBase::theScrollCaretFlag            = kJFalse;
JBoolean JXTEBase::theMiddleButtonPasteFlag      = kJTrue;

static const JUtf8Byte* kSelectionDataID = "JXTEBase";

struct MenuItemInfo
{
	JTextEditor::CmdIndex	cmd;
	const JUtf8Byte*		id;
};

// JError data

const JUtf8Byte* JXTEBase::kNoData            = "NoData::JXTEBase";
const JUtf8Byte* JXTEBase::kDataNotCompatible = "DataNotCompatible::JXTEBase";

static const JUtf8Byte* kDataNotCompatibleExtra = "DataNotCompatibleExtra::JXTEBase";

// Edit menu

static const JUtf8Byte* kMacEditMenuStr =
	"    Undo       %k Meta-Z       %i" kJXUndoAction
	"  | Redo       %k Meta-Shift-Z %i" kJXRedoAction
	"%l| Cut        %k Meta-X       %i" kJXCutAction
	"  | Copy       %k Meta-C       %i" kJXCopyAction
	"  | Paste      %k Meta-V       %i" kJXPasteAction
	"  | Clear                      %i" kJXClearAction
	"%l| Select all %k Meta-A       %i" kJXSelectAllAction;

static const JUtf8Byte* kWinEditMenuStr =
	"    Undo       %h uz %k Ctrl-Z       %i" kJXUndoAction
	"  | Redo       %h r  %k Ctrl-Shift-Z %i" kJXRedoAction
	"%l| Cut        %h tx %k Ctrl-X       %i" kJXCutAction
	"  | Copy       %h c  %k Ctrl-C       %i" kJXCopyAction
	"  | Paste      %h pv %k Ctrl-V       %i" kJXPasteAction
	"  | Clear      %h l                  %i" kJXClearAction
	"%l| Select all %h a  %k Ctrl-A       %i" kJXSelectAllAction;

#define kCleanRightMarginAction		"CleanRightMarginCmd::JXTEBase"
#define kCoerceRightMarginAction	"CoerceRightMarginCmd::JXTEBase"
#define kShiftSelLeftAction			"ShiftSelLeftCmd::JXTEBase"
#define kShiftSelRightAction		"ShiftSelRightCmd::JXTEBase"
#define kForceShiftSelLeftAction	"ForceShiftSelLeftCmd::JXTEBase"
#define kShowWhitespaceAction		"ShowWhitespaceCmd::JXTEBase"
#define kCleanAllWhitespaceAction	"CleanAllWhitespaceCmd::JXTEBase"
#define kCleanWhitespaceSelAction	"CleanWhitespaceSelCmd::JXTEBase"
#define kCleanAllWSAlignAction		"CleanAllWSAlignCmd::JXTEBase"
#define kCleanWSAlignSelAction		"CleanWSAlignSelCmd::JXTEBase"
#define kToggleReadOnlyAction		"ToggleReadOnlyCmd::JXTEBase"

static const JUtf8Byte* kMacCheckSpellingMenuStr =
	"  Check spelling  %i" kCheckAllSpellingAction
	"| Check selection %i" kCheckSpellingSelAction;

static const JUtf8Byte* kWinCheckSpellingMenuStr =
	"  Check spelling  %h s %i" kCheckAllSpellingAction
	"| Check selection %h k %i" kCheckSpellingSelAction;

static const JUtf8Byte* kMacAdjustMarginsMenuStr =
	"    Clean right margin  %k Meta-Return       %i" kCleanRightMarginAction
	"  | Coerce right margin %k Meta-Shift-Return %i" kCoerceRightMarginAction
	"%l| Shift left          %k Meta-[            %i" kShiftSelLeftAction
	"  | Shift right         %k Meta-]            %i" kShiftSelRightAction
	"  | Force shift left    %k Meta-{            %i" kForceShiftSelLeftAction;

static const JUtf8Byte* kWinAdjustMarginsMenuStr =
	"    Clean right margin  %h m %k Ctrl-Return       %i" kCleanRightMarginAction
	"  | Coerce right margin %h n %k Ctrl-Shift-Return %i" kCoerceRightMarginAction
	"%l| Shift left          %h e %k Ctrl-[            %i" kShiftSelLeftAction
	"  | Shift right         %h i %k Ctrl-]            %i" kShiftSelRightAction
	"  | Force shift left    %h f %k Ctrl-{            %i" kForceShiftSelLeftAction;

static const JUtf8Byte* kMacCleanWhitespaceMenuStr =
	"  Show whitespace        %b %i" kShowWhitespaceAction
	"| Clean all whitespace      %i" kCleanAllWhitespaceAction
	"| Clean selected ws         %i" kCleanWhitespaceSelAction
	"| Clean all ws & alignment  %i" kCleanAllWSAlignAction
	"| Clean selected ws & align %i" kCleanWSAlignSelAction;

static const JUtf8Byte* kWinCleanWhitespaceMenuStr =
	"  Show whitespace   %b      %i" kShowWhitespaceAction
	"| Clean all whitespace %h w %i" kCleanAllWhitespaceAction
	"| Clean selected ws         %i" kCleanWhitespaceSelAction
	"| Clean all ws & alignment  %i" kCleanAllWSAlignAction
	"| Clean selected ws & align %i" kCleanWSAlignSelAction;

static const JUtf8Byte* kMacReadOnlyMenuStr =
	"Read only %b %i" kToggleReadOnlyAction;

static const JUtf8Byte* kWinReadOnlyMenuStr =
	"Read only %b %h o %i" kToggleReadOnlyAction;

static const MenuItemInfo kEditMenuItemInfo[] =
{
	{ JTextEditor::kUndoCmd,      kJXUndoAction      },
	{ JTextEditor::kRedoCmd,      kJXRedoAction      },
	{ JTextEditor::kCutCmd,       kJXCutAction       },
	{ JTextEditor::kCopyCmd,      kJXCopyAction      },
	{ JTextEditor::kPasteCmd,     kJXPasteAction     },
	{ JTextEditor::kDeleteSelCmd, kJXClearAction     },
	{ JTextEditor::kSelectAllCmd, kJXSelectAllAction },

	{ JTextEditor::kCheckSpellingCmd,      kCheckAllSpellingAction   },
	{ JTextEditor::kCheckSpellingSelCmd,   kCheckSpellingSelAction   },
	{ JTextEditor::kCleanRightMarginCmd,   kCleanRightMarginAction   },
	{ JTextEditor::kCoerceRightMarginCmd,  kCoerceRightMarginAction  },
	{ JTextEditor::kShiftSelLeftCmd,       kShiftSelLeftAction       },
	{ JTextEditor::kShiftSelRightCmd,      kShiftSelRightAction      },
	{ JTextEditor::kForceShiftSelLeftCmd,  kForceShiftSelLeftAction  },
	{ JTextEditor::kShowWhitespaceCmd,     kShowWhitespaceAction     },
	{ JTextEditor::kCleanAllWhitespaceCmd, kCleanAllWhitespaceAction },
	{ JTextEditor::kCleanWhitespaceSelCmd, kCleanWhitespaceSelAction },
	{ JTextEditor::kCleanAllWSAlignCmd,    kCleanAllWSAlignAction    },
	{ JTextEditor::kCleanWSAlignSelCmd,    kCleanWSAlignSelAction    },
	{ JTextEditor::kToggleReadOnlyCmd,     kToggleReadOnlyAction     },
};

// used when setting images

enum
{
	kUndoIndex = 1, kRedoIndex,
	kCutIndex, kCopyIndex, kPasteIndex, kClearIndex,
	kSelectAllIndex
};

// Search menu

static const JUtf8Byte* kMacSearchMenuStr =
	"    Find...                   %k Meta-F       %i" kJXFindDialogAction
	"  | Find previous             %k Meta-Shift-G %i" kJXFindPreviousAction
	"  | Find next                 %k Meta-G       %i" kJXFindNextAction
	"%l| Enter search string       %k Meta-E       %i" kJXEnterSearchTextAction
	"  | Enter replace string      %k Meta-Shift-E %i" kJXEnterReplaceTextAction
	"%l| Find selection backwards  %k Meta-Shift-H %i" kJXFindSelectionBackwardsAction
	"  | Find selection forward    %k Meta-H       %i" kJXFindSelectionForwardAction
	"%l| Find clipboard backwards  %k Ctrl-Shift-H %i" kJXFindClipboardBackwardsAction
	"  | Find clipboard forward    %k Ctrl-H       %i" kJXFindClipboardForwardAction;

static const JUtf8Byte* kWinSearchMenuStr =
	"    Find...                   %h f %k Ctrl-F       %i" kJXFindDialogAction
	"  | Find previous             %h p %k Ctrl-Shift-G %i" kJXFindPreviousAction
	"  | Find next                 %h n %k Ctrl-G       %i" kJXFindNextAction
	"%l| Enter search string       %h s %k Ctrl-E       %i" kJXEnterSearchTextAction
	"  | Enter replace string      %h e %k Ctrl-Shift-E %i" kJXEnterReplaceTextAction
	"%l| Find selection backwards  %h b %k Ctrl-Shift-H %i" kJXFindSelectionBackwardsAction
	"  | Find selection forward    %h o %k Ctrl-H       %i" kJXFindSelectionForwardAction
	"%l| Find clipboard backwards  %h k %k Meta-Shift-H %i" kJXFindClipboardBackwardsAction
	"  | Find clipboard forward    %h w %k Meta-H       %i" kJXFindClipboardForwardAction;

static const MenuItemInfo kSearchMenuItemInfo[] =
{
	{ JTextEditor::kFindDialogCmd,            kJXFindDialogAction             },
	{ JTextEditor::kFindPreviousCmd,          kJXFindPreviousAction           },
	{ JTextEditor::kFindNextCmd,              kJXFindNextAction               },
	{ JTextEditor::kEnterSearchTextCmd,       kJXEnterSearchTextAction        },
	{ JTextEditor::kEnterReplaceTextCmd,      kJXEnterReplaceTextAction       },
	{ JTextEditor::kFindSelectionBackwardCmd, kJXFindSelectionBackwardsAction },
	{ JTextEditor::kFindSelectionForwardCmd,  kJXFindSelectionForwardAction   },
	{ JTextEditor::kFindClipboardBackwardCmd, kJXFindClipboardBackwardsAction },
	{ JTextEditor::kFindClipboardForwardCmd,  kJXFindClipboardForwardAction   }
};

// used when setting images

enum
{
	kSFindCmd = 1,
	kSFindPrevCmd,          kSFindNextCmd,
	kSEnterSearchStrCmd,    kSEnterReplaceStrCmd,
	kSFindSelectionBackCmd, kSFindSelectionFwdCmd,
	kSFindClipboardBackCmd, kSFindClipboardFwdCmd
};

// Search & Replace menu

static const JUtf8Byte* kMacReplaceMenuStr =
	"    Find...                   %k Meta-F       %i" kJXFindDialogAction
	"  | Find previous             %k Meta-Shift-G %i" kJXFindPreviousAction
	"  | Find next                 %k Meta-G       %i" kJXFindNextAction
	"%l| Enter search string       %k Meta-E       %i" kJXEnterSearchTextAction
	"  | Enter replace string      %k Meta-Shift-E %i" kJXEnterReplaceTextAction
	"%l| Find selection backwards  %k Meta-Shift-H %i" kJXFindSelectionBackwardsAction
	"  | Find selection forward    %k Meta-H       %i" kJXFindSelectionForwardAction
	"%l| Find clipboard backwards  %k Ctrl-Shift-H %i" kJXFindClipboardBackwardsAction
	"  | Find clipboard forward    %k Ctrl-H       %i" kJXFindClipboardForwardAction
	"%l| Replace                   %k Meta-=       %i" kJXReplaceSelectionAction
	"  | Replace and find previous %k Meta-Shift-L %i" kJXReplaceFindPrevAction
	"  | Replace and find next     %k Meta-L       %i" kJXReplaceFindNextAction
	"%l| Replace all                               %i" kJXReplaceAllAction
	"  | Replace all in selection                  %i" kJXReplaceAllInSelectionAction;

static const JUtf8Byte* kWinReplaceMenuStr =
	"    Find...                   %h f %k Ctrl-F       %i" kJXFindDialogAction
	"  | Find previous             %h p %k Ctrl-Shift-G %i" kJXFindPreviousAction
	"  | Find next                 %h n %k Ctrl-G       %i" kJXFindNextAction
	"%l| Enter search string       %h s %k Ctrl-E       %i" kJXEnterSearchTextAction
	"  | Enter replace string      %h e %k Ctrl-Shift-E %i" kJXEnterReplaceTextAction
	"%l| Find selection backwards  %h b %k Ctrl-Shift-H %i" kJXFindSelectionBackwardsAction
	"  | Find selection forward    %h o %k Ctrl-H       %i" kJXFindSelectionForwardAction
	"%l| Find clipboard backwards  %h k %k Meta-Shift-H %i" kJXFindClipboardBackwardsAction
	"  | Find clipboard forward    %h w %k Meta-H       %i" kJXFindClipboardForwardAction
	"%l| Replace                   %h r %k Ctrl-=       %i" kJXReplaceSelectionAction
	"  | Replace and find previous %h v %k Ctrl-Shift-L %i" kJXReplaceFindPrevAction
	"  | Replace and find next     %h x %k Ctrl-L       %i" kJXReplaceFindNextAction
	"%l| Replace all               %h c                 %i" kJXReplaceAllAction
	"  | Replace all in selection  %h i                 %i" kJXReplaceAllInSelectionAction;

static const MenuItemInfo kReplaceMenuItemInfo[] =
{
	{ JTextEditor::kFindDialogCmd,            kJXFindDialogAction             },
	{ JTextEditor::kFindPreviousCmd,          kJXFindPreviousAction           },
	{ JTextEditor::kFindNextCmd,              kJXFindNextAction               },
	{ JTextEditor::kEnterSearchTextCmd,       kJXEnterSearchTextAction        },
	{ JTextEditor::kEnterReplaceTextCmd,      kJXEnterReplaceTextAction       },
	{ JTextEditor::kFindSelectionBackwardCmd, kJXFindSelectionBackwardsAction },
	{ JTextEditor::kFindSelectionForwardCmd,  kJXFindSelectionForwardAction   },
	{ JTextEditor::kFindClipboardBackwardCmd, kJXFindClipboardBackwardsAction },
	{ JTextEditor::kFindClipboardForwardCmd,  kJXFindClipboardForwardAction   },
	{ JTextEditor::kReplaceSelectionCmd,      kJXReplaceSelectionAction       },
	{ JTextEditor::kReplaceFindPrevCmd,       kJXReplaceFindPrevAction        },
	{ JTextEditor::kReplaceFindNextCmd,       kJXReplaceFindNextAction        },
	{ JTextEditor::kReplaceAllCmd,            kJXReplaceAllAction             },
	{ JTextEditor::kReplaceAllInSelectionCmd, kJXReplaceAllInSelectionAction  }
};

// used when setting images

enum
{
	kSRFindCmd = 1,
	kSRFindPrevCmd,          kSRFindNextCmd,
	kSREnterSearchStrCmd,    kSREnterReplaceStrCmd,
	kSRFindSelectionBackCmd, kSRFindSelectionFwdCmd,
	kSRFindClipboardBackCmd, kSRFindClipboardFwdCmd,
	kSRReplaceCmd,
	kSRReplaceAndFindPrevCmd, kSRReplaceAndFindNextCmd,
	kSRReplaceAllCmd, kSRReplaceAllInSelectionCmd
};

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXTEBase::JXTEBase
	(
	const Type			type,
	JStyledText*		text,
	const JBoolean		ownsText,
	const JBoolean		breakCROnly,
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
	JXScrollableWidget(scrollbarSet, enclosure, hSizing, vSizing, x,y, w,h),
	JTextEditor(type, text, ownsText, GetFontManager(), breakCROnly,
				JColorManager::GetRedColor(),				// caret
				JColorManager::GetDefaultSelectionColor(),	// selection filled
				JColorManager::GetBlueColor(),				// selection outline
				JColorManager::GetGrayColor(70),			// whitespace
				GetApertureWidth()),

	itsWillPasteCustomFlag( kJFalse )
{
	itsEditMenu    = nullptr;
	itsSearchMenu  = nullptr;
	itsReplaceMenu = nullptr;

	itsCanCheckSpellingFlag   = kJFalse;
	itsCanAdjustMarginsFlag   = kJFalse;
	itsCanCleanWhitespaceFlag = kJFalse;
	itsCanToggleReadOnlyFlag  = kJFalse;

	itsPSPrinter      = nullptr;
	itsPSPrintName    = nullptr;
	itsPTPrinter      = nullptr;
	itsPTPrintName    = nullptr;

	itsGoToLineDialog = nullptr;

	itsDNDDragInfo    = nullptr;
	itsDNDDropInfo    = nullptr;

	itsBlinkTask = jnew JXTEBlinkCaretTask(this);
	assert( itsBlinkTask != nullptr );
	TECaretShouldBlink(kJTrue);

	itsMinWidth = itsMinHeight = 0;
	RecalcAll();

	itsStyledText0XAtom =
		GetDisplay()->RegisterXAtom(JXTextSelection::GetStyledText0XAtomName());

	if (type == kStaticText)
		{
		WantInput(kJFalse);
		}
	else
		{
		WantInput(kJTrue);
		SetDefaultCursor(kJXTextEditCursor);
		}

	ListenTo(GetText());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXTEBase::~JXTEBase()
{
	JXSearchTextDialog* dlog;
	if (JXGetSearchTextDialog(&dlog))
		{
		dlog->TEDeactivated(this);
		}

	jdelete itsPSPrintName;
	jdelete itsPTPrintName;
	jdelete itsBlinkTask;
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
JXTEBase::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	TEDraw(p, rect);
}

/******************************************************************************
 HandleMouseEnter

 ******************************************************************************/

void
JXTEBase::HandleMouseEnter()
{
	ShowCursor();
	itsPrevMousePt = JPoint(0,0);

	JXScrollableWidget::HandleMouseEnter();
}

/******************************************************************************
 HandleMouseHere

 ******************************************************************************/

void
JXTEBase::HandleMouseHere
	(
	const JPoint&			pt,
	const JXKeyModifiers&	modifiers
	)
{
	if (pt != itsPrevMousePt)
		{
		ShowCursor();
		}
	itsPrevMousePt = pt;

	JXScrollableWidget::HandleMouseHere(pt, modifiers);
}

/******************************************************************************
 HandleMouseLeave

 ******************************************************************************/

void
JXTEBase::HandleMouseLeave()
{
	ShowCursor();
	JXScrollableWidget::HandleMouseLeave();
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
JXTEBase::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsSearchMenu != nullptr || itsReplaceMenu != nullptr)
		{
		(JXGetSearchTextDialog())->SetActiveTE(this);
		}

	const Type type = GetType();
	if (button > kJXRightButton)
		{
		ScrollForWheel(button, modifiers);
		}
	else if (type != kStaticText)
		{
		ShowCursor();

		if (button == kJXMiddleButton && type == kFullEditor && theMiddleButtonPasteFlag)
			{
			TEHandleMouseDown(pt, 1, kJFalse, kJFalse);
			TEHandleMouseUp();
			Paste();
			}
		else if (button != kJXMiddleButton)
			{
			const JBoolean extendSelection = JI2B(
				button == kJXRightButton || modifiers.shift() );
			const JBoolean partialWord = JI2B(
				(thePWMod == kCtrlMetaPWMod &&
				 modifiers.control() && modifiers.meta()) ||
				(thePWMod != kCtrlMetaPWMod &&
				 modifiers.GetState(kJXMod2KeyIndex + thePWMod - kMod2PWMod)));
			TEHandleMouseDown(pt, clickCount, extendSelection, partialWord);
			}
		}
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
JXTEBase::HandleMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	DNDDragInfo info(&pt, &buttonStates, &modifiers);
	itsDNDDragInfo = &info;

	TEHandleMouseDrag(pt);

	itsDNDDragInfo = nullptr;
}

/******************************************************************************
 HandleMouseUp (virtual protected)

 ******************************************************************************/

void
JXTEBase::HandleMouseUp
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	TEHandleMouseUp();
}

/******************************************************************************
 HitSamePart (virtual protected)

 ******************************************************************************/

JBoolean
JXTEBase::HitSamePart
	(
	const JPoint& pt1,
	const JPoint& pt2
	)
	const
{
	return TEHitSamePart(pt1, pt2);
}

/******************************************************************************
 TEBeginDND (virtual protected)

 ******************************************************************************/

JBoolean
JXTEBase::TEBeginDND()
{
	assert( itsDNDDragInfo != nullptr );

	JXTextSelection* data = jnew JXTextSelection(this, kSelectionDataID);
	assert( data != nullptr );

	return BeginDND(*(itsDNDDragInfo->pt), *(itsDNDDragInfo->buttonStates),
					*(itsDNDDragInfo->modifiers), data);
}

/******************************************************************************
 DNDFinish (virtual protected)

 ******************************************************************************/

void
JXTEBase::DNDFinish
	(
	const JBoolean		isDrop,
	const JXContainer*	target
	)
{
	TEDNDFinished();
}

/******************************************************************************
 GetDNDAction (virtual protected)

	This is called repeatedly during the drag so the drop action can be
	changed based on the current target, buttons, and modifier keys.

 ******************************************************************************/

Atom
JXTEBase::GetDNDAction
	(
	const JXContainer*		target,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	const Type type = GetType();
	if (type == kFullEditor && modifiers.control())
		{
		return GetDNDManager()->GetDNDActionAskXAtom();
		}
	else if (type == kFullEditor &&
			 ((target == this && !modifiers.meta()) ||
			  (target != this &&  modifiers.meta())))
		{
		return GetDNDManager()->GetDNDActionMoveXAtom();
		}
	else
		{
		return GetDNDManager()->GetDNDActionCopyXAtom();
		}
}

/******************************************************************************
 GetDNDAskActions (virtual protected)

	This is called when the value returned by GetDNDAction() changes to
	XdndActionAsk.  If GetDNDAction() repeatedly returns XdndActionAsk,
	this function is not called again because it is assumed that the
	actions are the same within a single DND session.

	This function must place at least 2 elements in askActionList and
	askDescriptionList.

	The first element should be the default action.

 ******************************************************************************/

void
JXTEBase::GetDNDAskActions
	(
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers,
	JArray<Atom>*			askActionList,
	JPtrArray<JString>*		askDescriptionList
	)
{
	assert( GetType() == kFullEditor );

	JXDNDManager* dndMgr = GetDNDManager();
	askActionList->AppendElement(dndMgr->GetDNDActionCopyXAtom());
	askActionList->AppendElement(dndMgr->GetDNDActionMoveXAtom());

	JString* s = jnew JString("CopyDescription::JXTEBase", kJFalse);
	assert( s != nullptr );
	askDescriptionList->Append(s);

	s = jnew JString("MoveDescription::JXTEBase", kJFalse);
	assert( s != nullptr );
	askDescriptionList->Append(s);
}

/******************************************************************************
 GetSelectionData (virtual protected)

	This is called when DND is terminated by a drop or when the target
	requests the data during a drag, but only if the delayed evaluation
	constructor for JXSelectionData was used.

	id is the string passed to the JXSelectionData constructor.

 ******************************************************************************/

void
JXTEBase::GetSelectionData
	(
	JXSelectionData*	data,
	const JString&		id
	)
{
	if (id == kSelectionDataID)
		{
		JXTextSelection* textData = dynamic_cast<JXTextSelection*>(data);
		assert( textData != nullptr );

		JString* text = jnew JString;
		assert( text != nullptr );

		JRunArray<JFont>* style = jnew JRunArray<JFont>;
		assert( style != nullptr );

		const JBoolean ok = GetSelection(text, style);
		assert( ok );
		textData->SetData(text, style);

		if (GetType() == kFullEditor)
			{
			JCharacterRange r;
			const JBoolean ok = GetSelection(&r);
			assert( ok );
			textData->SetTextEditor(this, r);
			}
		}
	else
		{
		JXScrollableWidget::GetSelectionData(data, id);
		}
}

/******************************************************************************
 WillAcceptDrop (virtual protected)

 ******************************************************************************/

JBoolean
JXTEBase::WillAcceptDrop
	(
	const JArray<Atom>&	typeList,
	Atom*				action,
	const JPoint&		pt,
	const Time			time,
	const JXWidget*		source
	)
{
	itsWillPasteCustomFlag = kJFalse;

	if (GetType() != kFullEditor)
		{
		return kJFalse;
		}
	else if (source == this)
		{
		return kJTrue;
		}
	else if (TEXWillAcceptDrop(typeList, *action, time, source))
		{
		itsWillPasteCustomFlag = kJTrue;
		return kJTrue;
		}
	else
		{
		JXDNDManager* dndMgr = GetDNDManager();
		JBoolean canGetStyledText, canGetText;
		Atom textType;
		return JI2B(
			(*action == dndMgr->GetDNDActionCopyXAtom() ||
			 *action == dndMgr->GetDNDActionMoveXAtom() ||
			 *action == dndMgr->GetDNDActionAskXAtom()) &&
			GetAvailDataTypes(typeList, &canGetStyledText, &canGetText, &textType));
		}
}

/******************************************************************************
 TEXWillAcceptDrop (virtual protected)

	Derived classes can override this to accept other data types that
	can be pasted.  Returning kJTrue guarantees that TEXConvertDropData()
	will be called, and this can be used to accept a different data type
	even when one of the default types (e.g. text/plain) is available.

	Data types that cannot be pasted must be implemented by overriding
	WillAcceptDrop() and -all- four HandleDND*() functions.

	When overriding this function, derived classes must also override
	TEXConvertDropData() to process the actual data that is dropped.

	source is non-nullptr if the drag is between widgets in the same program.
	This provides a way for compound documents to identify drags between their
	various parts.

 ******************************************************************************/

JBoolean
JXTEBase::TEXWillAcceptDrop
	(
	const JArray<Atom>&	typeList,
	const Atom			action,
	const Time			time,
	const JXWidget*		source
	)
{
	return kJFalse;
}

/******************************************************************************
 HandleDNDEnter (virtual protected)

	This is called when the mouse enters the widget.

 ******************************************************************************/

void
JXTEBase::HandleDNDEnter()
{
	TEHandleDNDEnter();
}

/******************************************************************************
 HandleDNDHere (virtual protected)

	This is called while the mouse is inside the widget.

 ******************************************************************************/

void
JXTEBase::HandleDNDHere
	(
	const JPoint&	pt,
	const JXWidget*	source
	)
{
	TEHandleDNDHere(pt, JI2B(source == this));
}

/******************************************************************************
 HandleDNDLeave (virtual protected)

	This is called when the mouse leaves the widget without dropping data.

 ******************************************************************************/

void
JXTEBase::HandleDNDLeave()
{
	TEHandleDNDLeave();
}

/******************************************************************************
 HandleDNDDrop (virtual protected)

	This is called when the data is dropped.  The data is accessed via
	the selection manager, just like Paste.

 ******************************************************************************/

void
JXTEBase::HandleDNDDrop
	(
	const JPoint&		pt,
	const JArray<Atom>&	typeList,
	const Atom			action,
	const Time			time,
	const JXWidget*		source
	)
{
	DNDDropInfo info(&typeList, action, time);
	itsDNDDropInfo = &info;

	JXDNDManager* dndMgr  = GetDNDManager();
	const Atom copyAction = dndMgr->GetDNDActionCopyXAtom();
	if (!itsWillPasteCustomFlag &&
		action == dndMgr->GetDNDActionAskXAtom())
		{
		JArray<Atom> actionList;
		JPtrArray<JString> descriptionList(JPtrArrayT::kDeleteAll);
		if (!dndMgr->GetAskActions(&actionList, &descriptionList))
			{
			itsDNDDropInfo->action = copyAction;
			}
		else if (!dndMgr->ChooseDropAction(actionList, descriptionList,
										   &(itsDNDDropInfo->action)))
			{
			TEHandleDNDLeave();
			return;
			}
		}

	TEHandleDNDDrop(pt, JI2B(source == this),
					JI2B(itsDNDDropInfo->action == copyAction));

	itsDNDDropInfo = nullptr;
}

/******************************************************************************
 TEPasteDropData (virtual protected)

	Get the data that was dropped and paste it in.

 ******************************************************************************/

void
JXTEBase::TEPasteDropData()
{
	assert( itsDNDDropInfo != nullptr );

	JXDNDManager* dndMgr     = GetDNDManager();
	const Atom selectionName = dndMgr->GetDNDSelectionName();

	JString text;
	JRunArray<JFont> style;
	if (itsWillPasteCustomFlag &&
		TEXConvertDropData(*(itsDNDDropInfo->typeList), itsDNDDropInfo->action,
						   itsDNDDropInfo->time, &text, &style))
		{
		JRunArray<JFont>* s = (style.IsEmpty() ? nullptr : &style);
		Paste(text, s);
		}
	else if (!itsWillPasteCustomFlag &&
			 GetSelectionData(selectionName, *(itsDNDDropInfo->typeList),
							  itsDNDDropInfo->time, &text, &style) == kJNoError)
		{
		JRunArray<JFont>* s = (style.IsEmpty() ? nullptr : &style);
		Paste(text, s);

		if (itsDNDDropInfo->action == dndMgr->GetDNDActionMoveXAtom())
			{
			GetSelectionManager()->
				SendDeleteRequest(selectionName, itsDNDDropInfo->time);
			}
		}
}

/******************************************************************************
 TEXConvertDropData (virtual protected)

	Derived classes can override this to convert other data types into
	text and styles that can be pasted.  Return kJTrue to paste the contents
	of text and style.  To paste unstyled text, leave style empty.

	This function will only be called if the derived class implemented
	TEXWillAcceptDrop() and returned kJTrue.

 ******************************************************************************/

JBoolean
JXTEBase::TEXConvertDropData
	(
	const JArray<Atom>&	typeList,
	const Atom			action,
	const Time			time,
	JString*			text,
	JRunArray<JFont>*	style
	)
{
	return kJFalse;
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
JXTEBase::Activate()
{
	JXScrollableWidget::Activate();
	PrivateActivate();
}

/******************************************************************************
 Deactivate (virtual)

 ******************************************************************************/

void
JXTEBase::Deactivate()
{
	JXScrollableWidget::Deactivate();
	PrivateDeactivate();
}

/******************************************************************************
 Suspend (virtual)

 ******************************************************************************/

void
JXTEBase::Suspend()
{
	JXScrollableWidget::Suspend();
	PrivateDeactivate();
}

/******************************************************************************
 Resume (virtual)

 ******************************************************************************/

void
JXTEBase::Resume()
{
	JXScrollableWidget::Resume();
	PrivateActivate();
}

/******************************************************************************
 PrivateActivate (private)

 ******************************************************************************/

void
JXTEBase::PrivateActivate()
{
	if (IsActive() && HasFocus())
		{
		TEActivate();
		if (GetWindow()->HasFocus())
			{
			itsBlinkTask->Reset();
			itsBlinkTask->Start();
			TEActivateSelection();
			}
		}
}

/******************************************************************************
 PrivateDeactivate (private)

 ******************************************************************************/

void
JXTEBase::PrivateDeactivate()
{
	if (!IsActive())
		{
		TEDeactivate();
		itsBlinkTask->Stop();
		TEDeactivateSelection();
		}
}

/******************************************************************************
 HandleFocusEvent (virtual protected)

 ******************************************************************************/

void
JXTEBase::HandleFocusEvent()
{
	JXScrollableWidget::HandleFocusEvent();
	TEActivate();
	if (IsActive() && GetWindow()->HasFocus())
		{
		itsBlinkTask->Reset();
		itsBlinkTask->Start();
		TEActivateSelection();

		if (itsSearchMenu != nullptr || itsReplaceMenu != nullptr)
			{
			(JXGetSearchTextDialog())->SetActiveTE(this);
			}
		}
}

/******************************************************************************
 HandleUnfocusEvent (virtual protected)

 ******************************************************************************/

void
JXTEBase::HandleUnfocusEvent()
{
	JXScrollableWidget::HandleUnfocusEvent();
	TEDeactivate();
	itsBlinkTask->Stop();
	TEDeactivateSelection();

	if (itsSearchMenu != nullptr || itsReplaceMenu != nullptr)
		{
		(JXGetSearchTextDialog())->TEDeactivated(this);
		}
}

/******************************************************************************
 HandleWindowFocusEvent (virtual protected)

 ******************************************************************************/

void
JXTEBase::HandleWindowFocusEvent()
{
	JXScrollableWidget::HandleWindowFocusEvent();
	TEActivateSelection();
	if (IsActive() && HasFocus())
		{
		itsBlinkTask->Reset();
		itsBlinkTask->Start();
		}
}

/******************************************************************************
 HandleWindowUnfocusEvent (virtual protected)

 ******************************************************************************/

void
JXTEBase::HandleWindowUnfocusEvent()
{
	JXScrollableWidget::HandleWindowUnfocusEvent();
	TEHideCaret();
	TEDeactivateSelection();
	itsBlinkTask->Stop();
}

/******************************************************************************
 TECaretShouldBlink (virtual protected)

 ******************************************************************************/

void
JXTEBase::TECaretShouldBlink
	(
	const JBoolean blink
	)
{
	itsBlinkTask->ShouldBlink(blink);
}

/******************************************************************************
 HandleKeyPress (virtual)

	We handle all the edit shortcuts here because we won't always have
	an Edit menu.

 ******************************************************************************/

void
JXTEBase::HandleKeyPress
	(
	const int				origKey,
	const JXKeyModifiers&	origModifiers
	)
{
	if (itsSearchMenu != nullptr || itsReplaceMenu != nullptr)
		{
		(JXGetSearchTextDialog())->SetActiveTE(this);
		}

	int key                  = origKey;
	JXKeyModifiers modifiers = origModifiers;
	if (theWindowsHomeEndFlag)
		{
		RemapWindowsHomeEnd(&key, &modifiers);
		}

	const JBoolean controlOn = modifiers.control();
	const JBoolean metaOn    = modifiers.meta();
	const JBoolean shiftOn   = modifiers.shift();

	const Type type = GetType();
	if (type == kFullEditor && !controlOn && !metaOn &&
		(GetDisplay()->GetLatestButtonStates()).AllOff())
		{
		HideCursor();
		}

	JBoolean processed = kJFalse;
	char typedKey;

	if (type == kFullEditor &&
		(((key == 'z' || key == 'Z') && !controlOn &&  metaOn && !shiftOn) ||
		 (key == JXCtrl('Z')         &&  controlOn && !metaOn && !shiftOn)))
		{
		GetText()->Undo();
		processed = kJTrue;
		}
	else if (type == kFullEditor &&
			 (((key == 'x' || key == 'X') && !controlOn &&  metaOn && !shiftOn) ||
			  (key == JXCtrl('X')         &&  controlOn && !metaOn && !shiftOn)))
		{
		Cut();
		processed = kJTrue;
		}
	else if (((key == 'c' || key == 'C') && !controlOn &&  metaOn && !shiftOn) ||
			 (key == JXCtrl('C')         &&  controlOn && !metaOn && !shiftOn))
		{
		Copy();
		processed = kJTrue;
		}
	else if (type == kFullEditor &&
			 (((key == 'v' || key == 'V') && !controlOn &&  metaOn && !shiftOn) ||
			  (key == JXCtrl('V')         &&  controlOn && !metaOn && !shiftOn)))
		{
		Paste();
		processed = kJTrue;
		}
	else if (((key == 'a' || key == 'A') && !controlOn &&  metaOn && !shiftOn) ||
			 (key == JXCtrl('A')         &&  controlOn && !metaOn && !shiftOn))
		{
		SelectAll();
		processed = kJTrue;
		}

	else if (key == kJLeftArrow || key == kJRightArrow ||
			 key == kJUpArrow   || key == kJDownArrow)
		{
		CaretMotion motion = kMoveByCharacter;
		if ((thePWMod == kCtrlMetaPWMod && controlOn && metaOn) ||
			(thePWMod != kCtrlMetaPWMod &&
			 modifiers.GetState(kJXMod2KeyIndex + thePWMod - kMod2PWMod)))
			{
			motion = kMoveByPartialWord;
			}
		else if (controlOn)
			{
			motion = kMoveByWord;
			}
		else if (metaOn)
			{
			motion = kMoveByLine;
			}

		if (type == kFullEditor || shiftOn || motion != kMoveByCharacter)
			{
			processed = TEHandleKeyPress(key, shiftOn, motion, kJFalse);
			}
		else
			{
			processed = kJFalse;
			}
		}

	else if (type == kFullEditor &&
			 !((controlOn || metaOn) && '1' <= key && key <= '9') &&
			 OKToPassToJTE(key, &typedKey))
		{
		JBoolean deleteToTabStop = GetText()->TabInsertsSpaces();
		if (shiftOn)
			{
			deleteToTabStop = !deleteToTabStop;
			}

		processed = TEHandleKeyPress(typedKey, shiftOn, kMoveByCharacter, deleteToTabStop);
		}

	if (!processed)
		{
		if (theScrollCaretFlag)
			{
			// move caret to top/bottom if already scrolled there

			const JRect ap = GetAperture();
			const JRect b  = GetBounds();

			if ((key == XK_Page_Up || key == XK_KP_Page_Up) &&
				 ap.top == b.top)
				{
				SetCaretLocation(1);
				}
			else if ((key == XK_Page_Down || key == XK_KP_Page_Down) &&
					  ap.bottom == b.bottom)
				{
				SetCaretLocation(GetText()->GetText().GetCharacterCount()+1);
				}
			}

		int k = key;
		if (type != kFullEditor && k == ' ')
			{
			k = XK_Page_Down;
			}
		else if (type != kFullEditor && k == kJDeleteKey)
			{
			k = XK_Page_Up;
			}

		JXScrollableWidget::HandleKeyPress(k, modifiers);

		if (theScrollCaretFlag)
			{
			// move caret to top/bottom regardless of where it was

			if (key == XK_Home || key == XK_KP_Home)
				{
				SetCaretLocation(1);
				}
			else if (key == XK_End || key == XK_KP_End)
				{
				SetCaretLocation(GetText()->GetText().GetCharacterCount()+1);
				}
			}
		}
}

/******************************************************************************
 OKToPassToJTE (private)

	Map Latin 1,2,3,4,9 to 8-bit ASCII.

 ******************************************************************************/

JBoolean
JXTEBase::OKToPassToJTE
	(
	const int	key,
	char*		c
	)
	const
{
	const int flags = 0xFF00 & key;
	*c              = 0x00FF & key;
	return JI2B(*c != 0 &&
				(flags == 0x0000 || flags == 0x0100 || flags == 0x0200 ||
				 flags == 0x0300 || flags == 0x1300));
}

/******************************************************************************
 RemapWindowsHomeEnd (private)

	Home/End		=> beginning/end of line
	Ctrl-Home/End	=> scroll to top/bottom

 ******************************************************************************/

void
JXTEBase::RemapWindowsHomeEnd
	(
	int*			key,
	JXKeyModifiers*	modifiers
	)
	const
{
	if ((*key == XK_Home || *key == XK_KP_Home ||
		 *key == XK_End  || *key == XK_KP_End) &&
		modifiers->control())
		{
		modifiers->SetState(kJXControlKeyIndex, kJFalse);
		}
	else if (*key == XK_Home || *key == XK_KP_Home)
		{
		*key = kJLeftArrow;
		modifiers->SetState(kJXMetaKeyIndex, kJTrue);
		}
	else if (*key == XK_End || *key == XK_KP_End)
		{
		*key = kJRightArrow;
		modifiers->SetState(kJXMetaKeyIndex, kJTrue);
		}
}

/******************************************************************************
 MoveCaretToEdge

	key can be either kJUpArrow or kJDownArrow.

 ******************************************************************************/

void
JXTEBase::MoveCaretToEdge
	(
	const int key
	)
{
	const JSize h = (3 * (GetText()->GetDefaultFont()).GetLineHeight(GetFontManager())) / 4;

	JPoint pt;
	if (key == kJUpArrow)
		{
		pt    = (GetAperture()).topLeft();
		pt.y += h;
		}
	else if (key == kJDownArrow)
		{
		pt    = (GetAperture()).bottomLeft();
		pt.y -= h;
		}
	else
		{
		return;
		}

	SetCaretLocation(pt);
}

/******************************************************************************
 HandleShortcut (virtual)

 ******************************************************************************/

void
JXTEBase::HandleShortcut
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsSearchMenu != nullptr || itsReplaceMenu != nullptr)
		{
		(JXGetSearchTextDialog())->SetActiveTE(this);
		}
	JXScrollableWidget::HandleShortcut(key, modifiers);
}

/******************************************************************************
 AdjustCursor (virtual protected)

	Show the default cursor during drag-and-drop.

 ******************************************************************************/

void
JXTEBase::AdjustCursor
	(
	const JPoint&			pt,
	const JXKeyModifiers&	modifiers
	)
{
	if (TEWillDragAndDrop(pt, kJFalse, modifiers.meta()))
		{
		DisplayCursor(kJXDefaultCursor);
		}
	else
		{
		JXScrollableWidget::AdjustCursor(pt, modifiers);
		}
}

/******************************************************************************
 BoundsMoved (virtual protected)

 ******************************************************************************/

void
JXTEBase::BoundsMoved
	(
	const JCoordinate dx,
	const JCoordinate dy
	)
{
	JXScrollableWidget::BoundsMoved(dx, dy);

	if (theScrollCaretFlag && dy > 0 &&
		GetLineTop(GetLineForChar(GetInsertionIndex().charIndex)) > (GetAperture()).bottom)
		{
		MoveCaretToEdge(kJDownArrow);
		}
	else if (theScrollCaretFlag && dy < 0 &&
			 GetLineBottom(GetLineForChar(GetInsertionIndex().charIndex)) < (GetAperture()).top)
		{
		MoveCaretToEdge(kJUpArrow);
		}
}

/******************************************************************************
 ApertureResized (virtual protected)

 ******************************************************************************/

void
JXTEBase::ApertureResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXScrollableWidget::ApertureResized(dw,dh);
	TESetBoundsWidth(GetApertureWidth());
	TESetGUIBounds(itsMinWidth, itsMinHeight, -1);
}

/******************************************************************************
 TERefresh (virtual protected)

	Not inline because it is virtual.

 ******************************************************************************/

void
JXTEBase::TERefresh()
{
	Refresh();
}

/******************************************************************************
 TERefreshRect (virtual protected)

	Not inline because it is virtual.

 ******************************************************************************/

void
JXTEBase::TERefreshRect
	(
	const JRect& rect
	)
{
	RefreshRect(rect);
}

/******************************************************************************
 TERedraw (virtual protected)

	Not inline because it is virtual.

 ******************************************************************************/

void
JXTEBase::TERedraw()
{
	Redraw();
}

/******************************************************************************
 TESetGUIBounds (virtual protected)

	Keep the bounds at least as large as the aperture.

 ******************************************************************************/

void
JXTEBase::TESetGUIBounds
	(
	const JCoordinate w,
	const JCoordinate h,
	const JCoordinate changeY
	)
{
	itsMinWidth  = w;
	itsMinHeight = h;

	const JRect ap = GetApertureGlobal();

	JCoordinate width = ap.width();
	if (width < w)
		{
		width = w;
		}

	JCoordinate height = ap.height();
	if (height < h)
		{
		height = h;
		}

	JXScrollbar *hScrollbar, *vScrollbar;
	if (changeY >= 0 && GetScrollbars(&hScrollbar, &vScrollbar))
		{
		const JCoordinate origH = GetBoundsHeight();
		if (height < origH)
			{
			vScrollbar->PrepareForLowerMaxValue(changeY, origH - height);
			}
		else if (height > origH)
			{
			vScrollbar->PrepareForHigherMaxValue(changeY, height - origH);
			}
		}

	SetBounds(width, height);
}

/******************************************************************************
 TEWidthIsBeyondDisplayCapacity (virtual protected)

 ******************************************************************************/

JBoolean
JXTEBase::TEWidthIsBeyondDisplayCapacity
	(
	const JSize width
	)
	const
{
	return JI2B( width > 16000 );	// X uses 2 bytes for coordinate value
}

/******************************************************************************
 TEScrollToRect (virtual protected)

 ******************************************************************************/

JBoolean
JXTEBase::TEScrollToRect
	(
	const JRect&	rect,
	const JBoolean	centerInDisplay
	)
{
	if (centerInDisplay && rect.right <= GetApertureWidth())
		{
		JRect r = rect;
		r.left  = 0;
		return ScrollToRectCentered(r, kJFalse);
		}
	else if (centerInDisplay)
		{
		return ScrollToRectCentered(rect, kJFalse);
		}
	else
		{
		return ScrollToRect(rect);
		}
}

/******************************************************************************
 TEScrollForDrag (virtual protected)

	Not inline because it is virtual.

 ******************************************************************************/

JBoolean
JXTEBase::TEScrollForDrag
	(
	const JPoint& pt
	)
{
	return ScrollForDrag(pt);
}

/******************************************************************************
 TEScrollForDND (virtual protected)

	Not inline because it is virtual.

 ******************************************************************************/

JBoolean
JXTEBase::TEScrollForDND
	(
	const JPoint& pt
	)
{
	return kJFalse;		// JXContainer does it for us
}

/******************************************************************************
 TESetVertScrollStep (virtual protected)

 ******************************************************************************/

void
JXTEBase::TESetVertScrollStep
	(
	const JCoordinate vStep
	)
{
	SetVertStepSize(vStep);
	SetVertPageStepContext(vStep);
}

/******************************************************************************
 TEUpdateClipboard (virtual protected)

 ******************************************************************************/

void
JXTEBase::TEUpdateClipboard
	(
	const JString&			text,
	const JRunArray<JFont>&	style
	)
	const
{
	JXTextSelection* data = jnew JXTextSelection(GetDisplay(), text, &style);
	assert( data != nullptr );

	if (!GetSelectionManager()->SetData(kJXClipboardName, data))
		{
		(JGetUserNotification())->ReportError(JGetString("UnableToCopy::JXTEBase"));
		}
}

/******************************************************************************
 TEGetClipboard (virtual protected)

	Returns kJTrue if there is something pasteable on the system clipboard.

 ******************************************************************************/

JBoolean
JXTEBase::TEGetClipboard
	(
	JString*			text,
	JRunArray<JFont>*	style
	)
	const
{
	const JError err = GetSelectionData(kJXClipboardName, CurrentTime, text, style);
	if (err.OK())
		{
		return kJTrue;
		}
	else
		{
		if (err != kNoData)
			{
			err.ReportIfError();
			}
		return kJFalse;
		}
}

/******************************************************************************
 GetAvailDataTypes (private)

	Returns kJTrue if it can find a data type that we understand.

 ******************************************************************************/

JBoolean
JXTEBase::GetAvailDataTypes
	(
	const JArray<Atom>&	typeList,
	JBoolean*			canGetStyledText,
	JBoolean*			canGetText,
	Atom*				textType
	)
	const
{
	*canGetStyledText = kJFalse;
	*canGetText       = kJFalse;
	*textType         = None;

	JXSelectionManager* selMgr = GetSelectionManager();

	JBoolean utf8 = kJFalse;
	for (const Atom type : typeList)
		{
		if (type == selMgr->GetUtf8StringXAtom() ||
			type == selMgr->GetMimePlainTextUTF8XAtom())
			{
			*canGetText = kJTrue;
			*textType   = type;
			utf8        = kJTrue;
			}
		else if (!utf8 &&
				 (type == XA_STRING ||
				  type == selMgr->GetMimePlainTextXAtom()))
			{
			*canGetText = kJTrue;
			*textType   = type;
			}

		// By checking WillPasteStyledText(), we avoid wasting time
		// parsing style information.

		else if (type == itsStyledText0XAtom && GetText().WillPasteStyledText())
			{
			*canGetStyledText = kJTrue;
			}
		}

	return JI2B( *canGetStyledText || *canGetText );
}

/******************************************************************************
 GetSelectionData (private)

	Returns kJTrue if there is something pasteable in the given selection.

 ******************************************************************************/

JError
JXTEBase::GetSelectionData
	(
	const Atom			selectionName,
	const Time			time,
	JString*			text,
	JRunArray<JFont>*	style
	)
	const
{
	JArray<Atom> typeList;
	if (GetSelectionManager()->
			GetAvailableTypes(selectionName, time, &typeList))
		{
		return GetSelectionData(selectionName, typeList, time, text, style);
		}
	else
		{
		return NoData();
		}
}

/******************************************************************************
 GetSelectionData (private)

	Returns kJTrue if there is something pasteable in the given selection.

 ******************************************************************************/

JError
JXTEBase::GetSelectionData
	(
	const Atom			selectionName,
	const JArray<Atom>&	typeList,
	const Time			time,
	JString*			text,
	JRunArray<JFont>*	style
	)
	const
{
	text->Clear();
	style->RemoveAll();

	JBoolean canGetStyledText, canGetText;
	Atom textType;
	if (GetAvailDataTypes(typeList, &canGetStyledText, &canGetText, &textType))
		{
		JXSelectionManager* selMgr = GetSelectionManager();

		JBoolean gotData = kJFalse;
		JError err       = JNoError();

		Atom returnType, textReturnType;
		unsigned char* data = nullptr;
		JSize dataLength;
		JXSelectionManager::DeleteMethod delMethod;

		if (!gotData && canGetStyledText &&
			selMgr->GetData(selectionName, time, itsStyledText0XAtom,
							&returnType, &data, &dataLength, &delMethod))
			{
			if (returnType == itsStyledText0XAtom)
				{
				gotData = kJTrue;
				const std::string s(reinterpret_cast<char*>(data), dataLength);
				std::istringstream input(s);
				if (!JStyledText::ReadPrivateFormat(input, text, style))
					{
					err = DataNotCompatible();
					}
				}
			selMgr->DeleteData(&data, delMethod);
			}

		if (!gotData && canGetText &&
			selMgr->GetData(selectionName, time, textType,
							&textReturnType, &data, &dataLength, &delMethod))
			{
			if (textReturnType == XA_STRING ||
				textReturnType == selMgr->GetUtf8StringXAtom() ||
				textReturnType == selMgr->GetMimePlainTextXAtom() ||
				textReturnType == selMgr->GetMimePlainTextUTF8XAtom())
				{
				gotData = kJTrue;
				text->Set(reinterpret_cast<JUtf8Byte*>(data), dataLength);
				}
			selMgr->DeleteData(&data, delMethod);
			}

		if (!gotData)
			{
			err = DataNotCompatible(canGetText ? textReturnType : None, GetDisplay());
			}

		return err;
		}
	else
		{
		return NoData();
		}
}

/******************************************************************************
 DataNotCompatible::DataNotCompatible

 ******************************************************************************/

JXTEBase::DataNotCompatible::DataNotCompatible
	(
	const Atom	type,
	JXDisplay*	display
	)
	:
	JError(kDataNotCompatible)
{
	if (type != None)
		{
		assert( display != nullptr );

		const JUtf8Byte* map[] =
			{
			"atom", XGetAtomName(*display, type)
			};
		JString msg = JGetString(kDataNotCompatible);
		msg += JGetString(kDataNotCompatibleExtra, map, sizeof(map));
		SetMessage(msg);
		}
}

/******************************************************************************
 AppendEditMenu

	Call this to let us create the Edit menu for text editing.

 ******************************************************************************/

#include <jx_edit_undo.xpm>
#include <jx_edit_redo.xpm>
#include <jx_edit_cut.xpm>
#include <jx_edit_copy.xpm>
#include <jx_edit_paste.xpm>
#include <jx_edit_clear.xpm>
#include <jx_spell_check.xpm>
#include <jx_spell_check_selection.xpm>
#include <jx_edit_clean_right_margin.xpm>
#include <jx_edit_coerce_right_margin.xpm>
#include <jx_edit_shift_left.xpm>
#include <jx_edit_shift_right.xpm>
#include <jx_edit_force_shift_left.xpm>
#include <jx_show_whitespace.xpm>
#include <jx_edit_read_only.xpm>

JXTextMenu*
JXTEBase::AppendEditMenu
	(
	JXMenuBar*		menuBar,
	const JBoolean	showCheckSpellingCmds,
	const JBoolean	allowCheckSpelling,
	const JBoolean	showAdjustMarginsCmds,
	const JBoolean	allowAdjustMargins,
	const JBoolean	showCleanWhitespaceCmds,
	const JBoolean	allowCleanWhitespace,
	const JBoolean	showToggleReadOnly,
	const JBoolean	allowToggleReadOnly
	)
{
	JXTextMenu* editMenu =
		StaticAppendEditMenu(menuBar, showCheckSpellingCmds, allowCheckSpelling,
							 showAdjustMarginsCmds, allowAdjustMargins,
							 showCleanWhitespaceCmds, allowCleanWhitespace,
							 showToggleReadOnly, allowToggleReadOnly);

	ShareEditMenu(editMenu, allowCheckSpelling, allowAdjustMargins,
				  allowCleanWhitespace, allowToggleReadOnly);
	return editMenu;
}

JXTextMenu*
JXTEBase::StaticAppendEditMenu
	(
	JXMenuBar*		menuBar,
	const JBoolean	showCheckSpellingCmds,
	const JBoolean	allowCheckSpelling,
	const JBoolean	showAdjustMarginsCmds,
	const JBoolean	allowAdjustMargins,
	const JBoolean	showCleanWhitespaceCmds,
	const JBoolean	allowCleanWhitespace,
	const JBoolean	showToggleReadOnly,
	const JBoolean	allowToggleReadOnly
	)
{
	// create basic menu

	JXTextMenu* editMenu = menuBar->AppendTextMenu(JGetString("EditMenuTitle::JXTEBase"));
	if (JXMenu::GetDefaultStyle() == JXMenu::kMacintoshStyle)
		{
		editMenu->SetMenuItems(kMacEditMenuStr, "JXTEBase");
		}
	else
		{
		editMenu->SetShortcuts(JGetString("EditMenuShortcut::JXTEBase"));
		editMenu->SetMenuItems(kWinEditMenuStr, "JXTEBase");
		}

	editMenu->SetItemImage(kUndoIndex,  jx_edit_undo);
	editMenu->SetItemImage(kRedoIndex,  jx_edit_redo);
	editMenu->SetItemImage(kCutIndex,   jx_edit_cut);
	editMenu->SetItemImage(kCopyIndex,  jx_edit_copy);
	editMenu->SetItemImage(kPasteIndex, jx_edit_paste);
	editMenu->SetItemImage(kClearIndex, jx_edit_clear);

	editMenu->SetUpdateAction(JXMenu::kDisableAll);

	// append check spelling commands

	if (showCheckSpellingCmds)
		{
		const JSize itemCount = editMenu->GetItemCount();
		editMenu->ShowSeparatorAfter(itemCount);

		if (JXMenu::GetDefaultStyle() == JXMenu::kMacintoshStyle)
			{
			editMenu->AppendMenuItems(kMacCheckSpellingMenuStr, "JXTEBase");
			}
		else
			{
			editMenu->AppendMenuItems(kWinCheckSpellingMenuStr, "JXTEBase");
			}

		editMenu->SetItemImage(itemCount+1, jx_spell_check);
		editMenu->SetItemImage(itemCount+2, jx_spell_check_selection);
		}

	// append margin adjustment commands

	if (showAdjustMarginsCmds)
		{
		const JSize itemCount = editMenu->GetItemCount();
		editMenu->ShowSeparatorAfter(itemCount);

		if (JXMenu::GetDefaultStyle() == JXMenu::kMacintoshStyle)
			{
			editMenu->AppendMenuItems(kMacAdjustMarginsMenuStr, "JXTEBase");
			}
		else
			{
			editMenu->AppendMenuItems(kWinAdjustMarginsMenuStr, "JXTEBase");
			}

		editMenu->SetItemImage(itemCount+1, jx_edit_clean_right_margin);
		editMenu->SetItemImage(itemCount+2, jx_edit_coerce_right_margin);
		editMenu->SetItemImage(itemCount+3, jx_edit_shift_left);
		editMenu->SetItemImage(itemCount+4, jx_edit_shift_right);
		editMenu->SetItemImage(itemCount+5, jx_edit_force_shift_left);
		}

	// append clean whitespace commands

	if (showCleanWhitespaceCmds)
		{
		const JSize itemCount = editMenu->GetItemCount();
		editMenu->ShowSeparatorAfter(itemCount);

		if (JXMenu::GetDefaultStyle() == JXMenu::kMacintoshStyle)
			{
			editMenu->AppendMenuItems(kMacCleanWhitespaceMenuStr, "JXTEBase");
			}
		else
			{
			editMenu->AppendMenuItems(kWinCleanWhitespaceMenuStr, "JXTEBase");
			}

		editMenu->SetItemImage(itemCount+1, jx_show_whitespace);
		}

	// append "read only" checkbox

	if (showToggleReadOnly)
		{
		const JSize itemCount = editMenu->GetItemCount();
		editMenu->ShowSeparatorAfter(itemCount);

		if (JXMenu::GetDefaultStyle() == JXMenu::kMacintoshStyle)
			{
			editMenu->AppendMenuItems(kMacReadOnlyMenuStr, "JXTEBase");
			}
		else
			{
			editMenu->AppendMenuItems(kWinReadOnlyMenuStr, "JXTEBase");
			}

		editMenu->SetItemImage(itemCount+1, jx_edit_read_only);
		}

	return editMenu;
}

/******************************************************************************
 ShareEditMenu

	Call this to let us share the Edit menu with other objects.

	The JXTextMenu passed to the second version must have ID's
	assigned to the standard items:
		Undo, Redo, Cut, Copy, Paste, Clear, Select All

 ******************************************************************************/

JXTextMenu*
JXTEBase::ShareEditMenu
	(
	JXTEBase*		te,
	const JBoolean	allowCheckSpelling,
	const JBoolean	allowAdjustMargins,
	const JBoolean	allowCleanWhitespace,
	const JBoolean	allowToggleReadOnly
	)
{
	ShareEditMenu(te->itsEditMenu, allowCheckSpelling, allowAdjustMargins,
				  allowCleanWhitespace, allowToggleReadOnly);
	return itsEditMenu;
}

void
JXTEBase::ShareEditMenu
	(
	JXTextMenu*		menu,
	const JBoolean	allowCheckSpelling,
	const JBoolean	allowAdjustMargins,
	const JBoolean	allowCleanWhitespace,
	const JBoolean	allowToggleReadOnly
	)
{
	assert( itsEditMenu == nullptr && menu != nullptr );

	itsEditMenu               = menu;
	itsCanCheckSpellingFlag   = allowCheckSpelling;
	itsCanAdjustMarginsFlag   = allowAdjustMargins;
	itsCanCleanWhitespaceFlag = allowCleanWhitespace;
	itsCanToggleReadOnlyFlag  = allowToggleReadOnly;

	ListenTo(itsEditMenu);
}

/******************************************************************************
 AppendSearchMenu

	Call this to let us create the Search menu for text editing.

	*** This cannot be used if a Search & Replace menu already exists.

 ******************************************************************************/

#include <jx_find.xpm>
#include <jx_find_previous.xpm>
#include <jx_find_next.xpm>
#include <jx_find_selection_backwards.xpm>
#include <jx_find_selection_forward.xpm>
#include <jx_replace_selection.xpm>

JXTextMenu*
JXTEBase::AppendSearchMenu
	(
	JXMenuBar* menuBar
	)
{
	assert( itsSearchMenu == nullptr && itsReplaceMenu == nullptr );

	itsSearchMenu = menuBar->AppendTextMenu(JGetString("SearchMenuTitle::JXTEBase"));
	if (JXMenu::GetDefaultStyle() == JXMenu::kMacintoshStyle)
		{
		itsSearchMenu->SetMenuItems(kMacSearchMenuStr, "JXTEBase");
		}
	else
		{
		itsSearchMenu->SetShortcuts(JGetString("SearchMenuShortcut::JXTEBase"));
		itsSearchMenu->SetMenuItems(kWinSearchMenuStr, "JXTEBase");
		}

	itsSearchMenu->SetItemImage(kSFindCmd,              jx_find);
	itsSearchMenu->SetItemImage(kSFindPrevCmd,          jx_find_previous);
	itsSearchMenu->SetItemImage(kSFindNextCmd,          jx_find_next);
	itsSearchMenu->SetItemImage(kSFindSelectionBackCmd, jx_find_selection_backwards);
	itsSearchMenu->SetItemImage(kSFindSelectionFwdCmd,  jx_find_selection_forward);

	itsSearchMenu->SetUpdateAction(JXMenu::kDisableAll);
	ListenTo(itsSearchMenu);

	return itsSearchMenu;
}

/******************************************************************************
 AppendSearchReplaceMenu

	Call this to let us create the Search & Replace menu for text editing.

	*** This cannot be used if a Search menu already exists.

 ******************************************************************************/

JXTextMenu*
JXTEBase::AppendSearchReplaceMenu
	(
	JXMenuBar* menuBar
	)
{
	assert( itsSearchMenu == nullptr && itsReplaceMenu == nullptr );

	itsReplaceMenu = menuBar->AppendTextMenu(JGetString("SearchMenuTitle::JXTEBase"));
	if (JXMenu::GetDefaultStyle() == JXMenu::kMacintoshStyle)
		{
		itsReplaceMenu->SetMenuItems(kMacReplaceMenuStr, "JXTEBase");
		}
	else
		{
		itsReplaceMenu->SetShortcuts(JGetString("SearchMenuShortcut::JXTEBase"));
		itsReplaceMenu->SetMenuItems(kWinReplaceMenuStr, "JXTEBase");
		}

	itsReplaceMenu->SetItemImage(kSRFindCmd,              jx_find);
	itsReplaceMenu->SetItemImage(kSRFindPrevCmd,          jx_find_previous);
	itsReplaceMenu->SetItemImage(kSRFindNextCmd,          jx_find_next);
	itsReplaceMenu->SetItemImage(kSRFindSelectionBackCmd, jx_find_selection_backwards);
	itsReplaceMenu->SetItemImage(kSRFindSelectionFwdCmd,  jx_find_selection_forward);
	itsReplaceMenu->SetItemImage(kSRReplaceCmd,           jx_replace_selection);

	itsReplaceMenu->SetUpdateAction(JXMenu::kDisableAll);
	ListenTo(itsReplaceMenu);

	return itsReplaceMenu;
}

/******************************************************************************
 ShareSearchMenu

	Call this to let us share the Search or Search & Replace menu
	with other objects.

	The JXTextMenu passed to the versions taking JXTextMenu* must have ID's
	assigned to the standard items.

 ******************************************************************************/

JXTextMenu*
JXTEBase::ShareSearchMenu
	(
	JXTEBase* te
	)
{
	assert( itsSearchMenu == nullptr && itsReplaceMenu == nullptr &&
			(te->itsSearchMenu != nullptr || te->itsReplaceMenu != nullptr) );

	itsSearchMenu  = te->itsSearchMenu;
	itsReplaceMenu = te->itsReplaceMenu;

	if (itsSearchMenu != nullptr)
		{
		ListenTo(itsSearchMenu);
		return itsSearchMenu;
		}
	else if (itsReplaceMenu != nullptr)
		{
		ListenTo(itsReplaceMenu);
		return itsReplaceMenu;
		}
	else
		{
		assert_msg( 0, "should never happen" );
		return nullptr;
		}
}

void
JXTEBase::ShareSearchMenu
	(
	JXTextMenu* menu
	)
{
	assert( itsSearchMenu == nullptr && itsReplaceMenu == nullptr && menu != nullptr );

	itsSearchMenu = menu;
	ListenTo(itsSearchMenu);
}

void
JXTEBase::ShareSearchReplaceMenu
	(
	JXTextMenu* menu
	)
{
	assert( itsSearchMenu == nullptr && itsReplaceMenu == nullptr && menu != nullptr );

	itsReplaceMenu = menu;
	ListenTo(itsReplaceMenu);
}

/******************************************************************************
 Receive (virtual protected)

	Listen for menu update requests and menu selections.

 ******************************************************************************/

void
JXTEBase::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsEditMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		if (HasFocus())
			{
			UpdateEditMenu();
			}
		}
	else if (sender == itsEditMenu && message.Is(JXMenu::kItemSelected))
		{
		if (HasFocus())
			{
			const JXMenu::ItemSelected* selection =
				dynamic_cast<const JXMenu::ItemSelected*>(&message);
			assert( selection != nullptr );
			HandleEditMenu(selection->GetIndex());
			}
		}

	else if (sender == itsSearchMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		if (HasFocus())
			{
			UpdateSearchMenu();
			}
		}
	else if (sender == itsSearchMenu && message.Is(JXMenu::kItemSelected))
		{
		if (HasFocus())
			{
			const JXMenu::ItemSelected* selection =
				dynamic_cast<const JXMenu::ItemSelected*>(&message);
			assert( selection != nullptr );
			HandleSearchMenu(selection->GetIndex());
			}
		}

	else if (sender == itsReplaceMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		if (HasFocus())
			{
			UpdateReplaceMenu();
			}
		}
	else if (sender == itsReplaceMenu && message.Is(JXMenu::kItemSelected))
		{
		if (HasFocus())
			{
			const JXMenu::ItemSelected* selection =
				dynamic_cast<const JXMenu::ItemSelected*>(&message);
			assert( selection != nullptr );
			HandleReplaceMenu(selection->GetIndex());
			}
		}

	else if (sender == itsPSPrinter &&
			 message.Is(JPrinter::kPrintSetupFinished))
		{
		const JPrinter::PrintSetupFinished* info =
			dynamic_cast<const JPrinter::PrintSetupFinished*>(&message);
		assert( info != nullptr );
		if (info->Successful())
			{
			SetPSPrintFileName(itsPSPrinter->GetFileName());
			Print(*itsPSPrinter);
			}
		StopListening(itsPSPrinter);
		}

	else if (sender == itsPTPrinter &&
			 message.Is(JPrinter::kPrintSetupFinished))
		{
		const JPrinter::PrintSetupFinished* info =
			dynamic_cast<const JPrinter::PrintSetupFinished*>(&message);
		assert( info != nullptr );
		if (info->Successful())
			{
			SetPTPrintFileName(itsPTPrinter->GetFileName());
			itsPTPrinter->Print(GetText()->GetText());
			}
		StopListening(itsPTPrinter);
		}

	else if (sender == itsGoToLineDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
			{
			JBoolean physicalLineIndexFlag;
			JIndex lineIndex = itsGoToLineDialog->GetLineIndex(&physicalLineIndexFlag);
			if (physicalLineIndexFlag)
				{
				lineIndex = CRLineIndexToVisualLineIndex(lineIndex);
				}
			GoToLine(lineIndex);
			}
		itsGoToLineDialog = nullptr;
		}

	else if (sender == GetText() && message.Is(JStyledText::kWillBeBusy))
		{
		(JXGetApplication())->DisplayBusyCursor();
		}

	else
		{
		JXScrollableWidget::Receive(sender, message);
		JTextEditor::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateEditMenu (private)

 ******************************************************************************/

void
JXTEBase::UpdateEditMenu()
{
	assert( itsEditMenu != nullptr );

	JString crmActionText, crm2ActionText;
	JBoolean isReadOnly;
	const JArray<JBoolean> enableFlags =
		GetCmdStatus(&crmActionText, &crm2ActionText, &isReadOnly);

	const JSize count = itsEditMenu->GetItemCount();
	for (JIndex i=1; i<=count; i++)
		{
		CmdIndex cmd;
		if (EditMenuIndexToCmd(i, &cmd))
			{
			JBoolean enable = kJTrue;
			if (cmd == kCheckSpellingCmd ||
				cmd == kCheckSpellingSelCmd)
				{
				enable = JI2B(itsCanCheckSpellingFlag && (JXGetSpellChecker())->IsAvailable());
				}
			else if (cmd == kCleanRightMarginCmd)
				{
				itsEditMenu->SetItemText(i, crmActionText);
				enable = itsCanAdjustMarginsFlag;
				}
			else if (cmd == kCoerceRightMarginCmd)
				{
				itsEditMenu->SetItemText(i, crm2ActionText);
				enable = itsCanAdjustMarginsFlag;
				}
			else if (cmd == kShiftSelLeftCmd  ||
					 cmd == kShiftSelRightCmd ||
					 cmd == kForceShiftSelLeftCmd)
				{
				enable = itsCanAdjustMarginsFlag;
				}
			else if (cmd == kShowWhitespaceCmd)
				{
				enable = itsCanCleanWhitespaceFlag;
				if (enable && WillShowWhitespace())
					{
					itsEditMenu->CheckItem(i);
					}
				}
			else if (cmd == kCleanAllWhitespaceCmd ||
					 cmd == kCleanWhitespaceSelCmd ||
					 cmd == kCleanAllWSAlignCmd    ||
					 cmd == kCleanWSAlignSelCmd)
				{
				enable = itsCanCleanWhitespaceFlag;
				}
			else if (cmd == kToggleReadOnlyCmd)
				{
				if (isReadOnly)
					{
					itsEditMenu->CheckItem(i);
					}
				enable = itsCanToggleReadOnlyFlag;
				}

			itsEditMenu->SetItemEnable(i, JI2B(enable && enableFlags.GetElement(cmd)));
			}
		}
}

/******************************************************************************
 HandleEditMenu (private)

 ******************************************************************************/

void
JXTEBase::HandleEditMenu
	(
	const JIndex index
	)
{
	assert( itsEditMenu != nullptr );

	CmdIndex cmd;
	if (!EditMenuIndexToCmd(index, &cmd))
		{
		return;
		}

	if (itsSearchMenu != nullptr || itsReplaceMenu != nullptr)
		{
		(JXGetSearchTextDialog())->SetActiveTE(this);
		}

	if (cmd == kUndoCmd)
		{
		GetText()->Undo();
		}
	else if (cmd == kRedoCmd)
		{
		GetText()->Redo();
		}

	else if (cmd == kCutCmd)
		{
		Cut();
		}
	else if (cmd == kCopyCmd)
		{
		Copy();
		}
	else if (cmd == kPasteCmd)
		{
		Paste();
		}
	else if (cmd == kDeleteSelCmd)
		{
		DeleteSelection();
		}

	else if (cmd == kSelectAllCmd)
		{
		SelectAll();
		}

	else if (cmd == kCheckSpellingCmd)
		{
		(JXGetSpellChecker())->Check(this);
		}
	else if (cmd == kCheckSpellingSelCmd)
		{
		(JXGetSpellChecker())->CheckSelection(this);
		}

	else if (cmd == kCleanRightMarginCmd)
		{
		JCharacterRange range;
		GetText()->CleanRightMargin(kJFalse, &range);
		}
	else if (cmd == kCoerceRightMarginCmd)
		{
		JCharacterRange range;
		GetText()->CleanRightMargin(kJTrue, &range);
		}
	else if (cmd == kShiftSelLeftCmd)
		{
		TabSelectionLeft();
		}
	else if (cmd == kShiftSelRightCmd)
		{
		TabSelectionRight();
		}
	else if (cmd == kForceShiftSelLeftCmd)
		{
		TabSelectionLeft(1, kJTrue);
		}

	else if (cmd == kShowWhitespaceCmd)
		{
		ShouldShowWhitespace(!WillShowWhitespace());
		}
	else if (cmd == kCleanAllWhitespaceCmd)
		{
		CleanAllWhitespace(kJFalse);
		}
	else if (cmd == kCleanWhitespaceSelCmd)
		{
		CleanSelectedWhitespace(kJFalse);
		}
	else if (cmd == kCleanAllWSAlignCmd)
		{
		CleanAllWhitespace(kJTrue);
		}
	else if (cmd == kCleanWSAlignSelCmd)
		{
		CleanSelectedWhitespace(kJTrue);
		}

	else if (cmd == kToggleReadOnlyCmd)
		{
		const Type type = GetType();
		if (type == kFullEditor)
			{
			SetType(kSelectableText);
			}
		else if (type == kSelectableText)
			{
			SetType(kFullEditor);
			}
		// don't change kStaticText
		}
}

/******************************************************************************
 Edit menu index <-> cmd

 ******************************************************************************/

#define ClassName    JXTEBase
#define IndexToCmdFn EditMenuIndexToCmd
#define CmdToIndexFn EditMenuCmdToIndex
#define MenuVar      itsEditMenu
#define CmdType      MenuItemInfo
#define CmdIDList    kEditMenuItemInfo
#include "JXMenuItemIDUtil.th"
#undef ClassName
#undef IndexToCmdFn
#undef CmdToIndexFn
#undef MenuVar
#undef CmdType
#undef CmdIDList

/******************************************************************************
 UpdateSearchMenu (private)

 ******************************************************************************/

void
JXTEBase::UpdateSearchMenu()
{
	assert( itsSearchMenu != nullptr );

	JString crmActionText, crm2ActionText;
	JBoolean isReadOnly;
	JArray<JBoolean> enableFlags =
		GetCmdStatus(&crmActionText, &crm2ActionText, &isReadOnly);

	if (!GetText()->IsEmpty())
		{
		enableFlags.SetElement(kFindSelectionBackwardCmd, kJTrue);
		enableFlags.SetElement(kFindSelectionForwardCmd,  kJTrue);
		}

	const JSize count = itsSearchMenu->GetItemCount();
	for (JIndex i=1; i<=count; i++)
		{
		CmdIndex cmd;
		if (SearchMenuIndexToCmd(i, &cmd))
			{
			itsSearchMenu->SetItemEnable(i, enableFlags.GetElement(cmd));
			}
		}
}

/******************************************************************************
 HandleSearchMenu (private)

 ******************************************************************************/

void
JXTEBase::HandleSearchMenu
	(
	const JIndex index
	)
{
	assert( itsSearchMenu != nullptr );

	CmdIndex cmd;
	if (SearchMenuIndexToCmd(index, &cmd))
		{
		HandleSearchReplaceCmd(cmd);
		}
}

/******************************************************************************
 Search menu index <-> cmd

 ******************************************************************************/

#define ClassName    JXTEBase
#define IndexToCmdFn SearchMenuIndexToCmd
#define CmdToIndexFn SearchMenuCmdToIndex
#define MenuVar      itsSearchMenu
#define CmdType      MenuItemInfo
#define CmdIDList    kSearchMenuItemInfo
#include "JXMenuItemIDUtil.th"
#undef ClassName
#undef IndexToCmdFn
#undef CmdToIndexFn
#undef MenuVar
#undef CmdType
#undef CmdIDList

/******************************************************************************
 UpdateReplaceMenu (private)

 ******************************************************************************/

void
JXTEBase::UpdateReplaceMenu()
{
	assert( itsReplaceMenu != nullptr );

	JString crmActionText, crm2ActionText;
	JBoolean isReadOnly;
	JArray<JBoolean> enableFlags =
		GetCmdStatus(&crmActionText, &crm2ActionText, &isReadOnly);

	if (!GetText()->IsEmpty())
		{
		enableFlags.SetElement(kFindSelectionBackwardCmd, kJTrue);
		enableFlags.SetElement(kFindSelectionForwardCmd,  kJTrue);
		}

	const JSize count = itsReplaceMenu->GetItemCount();
	for (JIndex i=1; i<=count; i++)
		{
		CmdIndex cmd;
		if (SearchReplaceMenuIndexToCmd(i, &cmd))
			{
			itsReplaceMenu->SetItemEnable(i, enableFlags.GetElement(cmd));
			}
		}
}

/******************************************************************************
 HandleReplaceMenu (private)

 ******************************************************************************/

void
JXTEBase::HandleReplaceMenu
	(
	const JIndex index
	)
{
	assert( itsReplaceMenu != nullptr );

	CmdIndex cmd;
	if (SearchReplaceMenuIndexToCmd(index, &cmd))
		{
		HandleSearchReplaceCmd(cmd);
		}
}

/******************************************************************************
 HandleSearchReplaceCmd (private)

 ******************************************************************************/

void
JXTEBase::HandleSearchReplaceCmd
	(
	const CmdIndex cmd
	)
{
	(JXGetSearchTextDialog())->SetActiveTE(this);

	if (cmd == kFindDialogCmd)
		{
		(JXGetSearchTextDialog())->Activate();
		}
	else if (cmd == kFindPreviousCmd)
		{
		SearchBackward();
		}
	else if (cmd == kFindNextCmd)
		{
		SearchForward();
		}
	else if (cmd == kEnterSearchTextCmd)
		{
		EnterSearchSelection();
		}
	else if (cmd == kEnterReplaceTextCmd)
		{
		EnterReplaceSelection();
		}
	else if (cmd == kFindSelectionBackwardCmd)
		{
		if (!HasSelection())
			{
			JStyledText::TextRange r;
			TEGetDoubleClickSelection(GetInsertionIndex(), kJFalse, kJFalse, &r);
			SetSelection(r);
			}
		SearchSelectionBackward();
		}
	else if (cmd == kFindSelectionForwardCmd)
		{
		if (!HasSelection())
			{
			JStyledText::TextRange r;
			TEGetDoubleClickSelection(GetInsertionIndex(), kJFalse, kJFalse, &r);
			SetSelection(r);
			}
		SearchSelectionForward();
		}
	else if (cmd == kFindClipboardBackwardCmd)
		{
		SearchClipboardBackward();
		}
	else if (cmd == kFindClipboardForwardCmd)
		{
		SearchClipboardForward();
		}

	else if (cmd == kReplaceSelectionCmd)
		{
		ReplaceSelection();
		}
	else if (cmd == kReplaceFindPrevCmd)
		{
		ReplaceAndSearchBackward();
		}
	else if (cmd == kReplaceFindNextCmd)
		{
		ReplaceAndSearchForward();
		}
	else if (cmd == kReplaceAllCmd)
		{
		ReplaceAll(kJFalse);
		}
	else if (cmd == kReplaceAllInSelectionCmd)
		{
		ReplaceAll(kJTrue);
		}
}

/******************************************************************************
 Search menu index <-> cmd

 ******************************************************************************/

#define ClassName    JXTEBase
#define IndexToCmdFn SearchReplaceMenuIndexToCmd
#define CmdToIndexFn SearchReplaceMenuCmdToIndex
#define MenuVar      itsReplaceMenu
#define CmdType      MenuItemInfo
#define CmdIDList    kReplaceMenuItemInfo
#include "JXMenuItemIDUtil.th"
#undef ClassName
#undef IndexToCmdFn
#undef CmdToIndexFn
#undef MenuVar
#undef CmdType
#undef CmdIDList

/******************************************************************************
 TEHasSearchText (virtual)

 ******************************************************************************/

JBoolean
JXTEBase::TEHasSearchText()
	const
{
	return (JXGetSearchTextDialog())->HasSearchText();
}

/******************************************************************************
 SearchForward

	Get the search parameters from the search dialog and look for the
	next match beyond the current position.  We beep if we can't find a match.

 ******************************************************************************/

JBoolean
JXTEBase::SearchForward
	(
	const JBoolean reportNotFound
	)
{
	JRegex* searchRegex;
	JString replaceStr;
	JInterpolate* interpolator;
	JBoolean entireWord, wrapSearch, preserveCase;
	if (!(JXGetSearchTextDialog())->GetSearchParameters(
			&searchRegex, &entireWord, &wrapSearch,
			&replaceStr, &interpolator, &preserveCase))
		{
		return kJFalse;
		}

	if (GetType() != kFullEditor && !HasSelection())	// caret not visible
		{
		SetCaretLocation(1);
		}

	JBoolean wrapped;
	const JStringMatch m = JTextEditor::SearchForward(*searchRegex, entireWord, wrapSearch, &wrapped);

	if (!m.IsEmpty())
		{
		TEScrollToSelection(kJTrue);
		}

	if ((m.IsEmpty() && reportNotFound) || wrapped)
		{
		GetDisplay()->Beep();
		}

	return !m.IsEmpty();
}

/******************************************************************************
 SearchBackward

	Get the search parameters from the search dialog and look for the
	match before the current position.  We beep if we can't find a match.

 ******************************************************************************/

JBoolean
JXTEBase::SearchBackward()
{
	JRegex* searchRegex;
	JString replaceStr;
	JInterpolate* interpolator;
	JBoolean entireWord, wrapSearch, preserveCase;
	if (!(JXGetSearchTextDialog())->GetSearchParameters(
			&searchRegex, &entireWord, &wrapSearch,
			&replaceStr, &interpolator, &preserveCase))
		{
		return kJFalse;
		}

	if (GetType() != kFullEditor && !HasSelection())	// caret not visible
		{
		SetCaretLocation(GetText()->GetText().GetCharacterCount()+1);
		}

	JBoolean wrapped;
	const JStringMatch m = JTextEditor::SearchBackward(*searchRegex, entireWord, wrapSearch, &wrapped);

	if (!m.IsEmpty())
		{
		TEScrollToSelection(kJTrue);
		}

	if (m.IsEmpty() || wrapped)
		{
		GetDisplay()->Beep();
		}

	return !m.IsEmpty();
}

/******************************************************************************
 Search for clipboard

 ******************************************************************************/

JBoolean
JXTEBase::SearchClipboardForward()
{
	JString text;
	JRunArray<JFont> style;
	if (TEGetClipboard(&text, &style))
		{
		(JXGetSearchTextDialog())->SetSearchText(text);
		return SearchForward();
		}
	else
		{
		return kJFalse;
		}
}

JBoolean
JXTEBase::SearchClipboardBackward()
{
	JString text;
	JRunArray<JFont> style;
	if (TEGetClipboard(&text, &style))
		{
		(JXGetSearchTextDialog())->SetSearchText(text);
		return SearchBackward();
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 EnterSearchSelection

	Copy the selection into the search field of the SearchTextDialog.
	Returns kJTrue if there was something selected.

 ******************************************************************************/

JBoolean
JXTEBase::EnterSearchSelection()
{
	JString selText;
	if (GetSelection(&selText))
		{
		(JXGetSearchTextDialog())->SetSearchText(selText);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 EnterReplaceSelection

	Copy the selection into the replace field of the SearchTextDialog.
	Returns kJTrue if there was something selected.

 ******************************************************************************/

JBoolean
JXTEBase::EnterReplaceSelection()
{
	JString selText;
	if (GetSelection(&selText))
		{
		(JXGetSearchTextDialog())->SetReplaceText(selText);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 ReplaceSelection

	Replace the current selection with the replace string from the
	SearchTextDialog.  Returns kJFalse if nothing is selected or if the
	current selection doesn't match the search string.

 ******************************************************************************/

JBoolean
JXTEBase::ReplaceSelection()
{
	JRegex* searchRegex;
	JString replaceStr;
	JInterpolate* interpolator;
	JBoolean entireWord, wrapSearch, preserveCase;
	if (GetType() != kFullEditor ||
		!(JXGetSearchTextDialog())->GetSearchParameters(
			&searchRegex, &entireWord, &wrapSearch,
			&replaceStr, &interpolator, &preserveCase))
		{
		return kJFalse;
		}

	const JStringMatch m = SelectionMatches(*searchRegex, entireWord);

	if (!m.IsEmpty())
		{
		JTextEditor::ReplaceSelection(m, replaceStr, interpolator, preserveCase);
		return kJTrue;
		}
	else
		{
		GetDisplay()->Beep();
		return kJFalse;
		}
}

/******************************************************************************
 ReplaceAll

	Replace every occurrence of the search string with the replace string.
	Returns kJTrue if it replaced anything.

 ******************************************************************************/

JBoolean
JXTEBase::ReplaceAll
	(
	const JBoolean restrictToSelection
	)
{
	JRegex* searchRegex;
	JString replaceStr;
	JInterpolate* interpolator;
	JBoolean entireWord, wrapSearch, preserveCase;
	if (GetType() == kFullEditor &&
		(JXGetSearchTextDialog())->GetSearchParameters(
			&searchRegex, &entireWord, &wrapSearch,
			&replaceStr, &interpolator, &preserveCase))
		{
		(JXGetApplication())->DisplayBusyCursor();

		return JTextEditor::ReplaceAll(*searchRegex, entireWord,
									   replaceStr, interpolator,
									   preserveCase, restrictToSelection);
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 SetPSPrinter

	Call this to provide a JXPSPrinter object for this text.  This object
	does *not* take ownership of the printer object.

 ******************************************************************************/

void
JXTEBase::SetPSPrinter
	(
	JXPSPrinter* p
	)
{
	if (itsPSPrinter != nullptr)
		{
		StopListening(itsPSPrinter);
		}
	itsPSPrinter = p;
}

/******************************************************************************
 GetPSPrintFileName

	Call this to get the file name used in the Print Setup dialog.

 ******************************************************************************/

const JString&
JXTEBase::GetPSPrintFileName()
	const
{
	if (itsPSPrintName == nullptr)
		{
		JXTEBase* me = const_cast<JXTEBase*>(this);
		me->itsPSPrintName = jnew JString;
		assert( itsPSPrintName != nullptr );
		}

	return *itsPSPrintName;
}

/******************************************************************************
 SetPSPrintFileName

	Call this to set the file name used in the Print Setup dialog.

 ******************************************************************************/

void
JXTEBase::SetPSPrintFileName
	(
	const JString& fileName
	)
{
	GetPSPrintFileName();		// create itsPSPrintName
	*itsPSPrintName = fileName;
}

/******************************************************************************
 HandlePSPageSetup

	You must call SetPSPrinter() before using this routine.

 ******************************************************************************/

void
JXTEBase::HandlePSPageSetup()
{
	assert( itsPSPrinter != nullptr );

	itsPSPrinter->BeginUserPageSetup();
}

/******************************************************************************
 PrintPS

	You must call SetPSPrinter() before using this routine.

 ******************************************************************************/

void
JXTEBase::PrintPS()
{
	assert( itsPSPrinter != nullptr );

	itsPSPrinter->SetFileName(GetPSPrintFileName());
	itsPSPrinter->BeginUserPrintSetup();
	ListenTo(itsPSPrinter);
}

/******************************************************************************
 Print footer (virtual protected)

	Overrides of JTextEditor functions.

 ******************************************************************************/

JCoordinate
JXTEBase::GetPrintFooterHeight
	(
	JPagePrinter& p
	)
	const
{
	return JRound(1.5 * p.GetLineHeight());
}

void
JXTEBase::DrawPrintFooter
	(
	JPagePrinter&		p,
	const JCoordinate	footerHeight
	)
{
	JRect pageRect = p.GetPageRect();
	pageRect.top   = pageRect.bottom - footerHeight;

	JString pageNumberStr(p.GetPageIndex(), 0);
	pageNumberStr.Prepend("Page ");

	p.String(pageRect, pageNumberStr,
			 JPainter::kHAlignCenter, JPainter::kVAlignBottom);
}

/******************************************************************************
 SetPTPrinter

	Call this to provide a JXPTPrinter object for this text.  This object
	does *not* take ownership of the printer object.

 ******************************************************************************/

void
JXTEBase::SetPTPrinter
	(
	JXPTPrinter* p
	)
{
	if (itsPTPrinter != nullptr)
		{
		StopListening(itsPTPrinter);
		}
	itsPTPrinter = p;
}

/******************************************************************************
 GetPTPrintFileName

	Call this to get the file name used in the Print Setup dialog.

 ******************************************************************************/

const JString&
JXTEBase::GetPTPrintFileName()
	const
{
	if (itsPTPrintName == nullptr)
		{
		JXTEBase* me = const_cast<JXTEBase*>(this);
		me->itsPTPrintName = jnew JString;
		assert( itsPTPrintName != nullptr );
		}

	return *itsPTPrintName;
}

/******************************************************************************
 SetPTPrintFileName

	Call this to set the file name used in the Print Setup dialog.

 ******************************************************************************/

void
JXTEBase::SetPTPrintFileName
	(
	const JString& fileName
	)
{
	GetPTPrintFileName();		// create itsPTPrintName
	*itsPTPrintName = fileName;
}

/******************************************************************************
 HandlePTPageSetup

	You must call SetPTPrinter() before using this routine.

 ******************************************************************************/

void
JXTEBase::HandlePTPageSetup()
{
	assert( itsPTPrinter != nullptr );

	itsPTPrinter->BeginUserPageSetup();
}

/******************************************************************************
 PrintPT

	You must call SetPTPrinter() before using this routine.

 ******************************************************************************/

void
JXTEBase::PrintPT()
{
	assert( itsPTPrinter != nullptr );

	itsPTPrinter->SetFileName(GetPTPrintFileName());
	itsPTPrinter->BeginUserPrintSetup();
	ListenTo(itsPTPrinter);
}

/******************************************************************************
 AskForLine

	Opens dialog window to ask user which line to go to.

 ******************************************************************************/

void
JXTEBase::AskForLine()
{
	assert( itsGoToLineDialog == nullptr );

	const JIndex lineIndex = GetLineForChar(GetInsertionIndex().charIndex);
	const JSize lineCount  = GetLineCount();

	JXDirector* sup = GetWindow()->GetDirector();
	itsGoToLineDialog = jnew JXGoToLineDialog(sup, lineIndex, lineCount);
	assert( itsGoToLineDialog != nullptr );
	itsGoToLineDialog->BeginDialog();
	ListenTo(itsGoToLineDialog);
}

/******************************************************************************
 SaveDisplayState

	Save the selection and scrollbar positions.

	Cannot be virtual because derived class must use layered type.

 ******************************************************************************/

JXTEBase::DisplayState
JXTEBase::SaveDisplayState()
	const
{
	DisplayState state;
	state.scroll       = JXScrollableWidget::SaveDisplayState();
	state.hadSelection = GetSelection(&state.range);
	return state;
}

/******************************************************************************
 RestoreDisplayState

	Restore the selection and scrollbar positions.

	Cannot be virtual because derived class must use layered type.

 ******************************************************************************/

void
JXTEBase::RestoreDisplayState
	(
	const DisplayState& state
	)
{
	// restore the scrollbar positions

	JXScrollableWidget::RestoreDisplayState(state.scroll);

	// restore the selection

	if (state.hadSelection)
		{
		SetSelection(state.range);
		}
}

/******************************************************************************
 JXTEBase::PartialWordModifier

 ******************************************************************************/

std::istream&
operator>>
	(
	std::istream&					input,
	JXTEBase::PartialWordModifier&	pwMod
	)
{
	long temp;
	input >> temp;
	pwMod = (JXTEBase::PartialWordModifier) temp;
	assert( pwMod == JXTEBase::kCtrlMetaPWMod ||
			pwMod == JXTEBase::kMod2PWMod || pwMod == JXTEBase::kMod3PWMod ||
			pwMod == JXTEBase::kMod4PWMod || pwMod == JXTEBase::kMod5PWMod );
	return input;
}

std::ostream&
operator<<
	(
	std::ostream&						output,
	const JXTEBase::PartialWordModifier	pwMod
	)
{
	output << (long) pwMod;
	return output;
}
