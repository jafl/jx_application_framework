/******************************************************************************
 JXExprEditor.cpp

	Maintains a pane that displays the result of rendering a JFunction.
	Also allows editing of the JFunction.

	Since JX uses Tab to shift focus, we yield the tab key whenever
	there are no UIF's in the expression.  Since the number of UIF's are
	only changed by menu items and key presses, we only check for it there.

	The second constructor allows multiple JXExprEditors to share the same
	menus.  Only the JXExprEditor with focus actually uses the menus.
	The safest policy in this case is to create the JXMenuBar independent
	of all ExprExitorSets so the menus don't get thrown out when a JXExprEditor
	is deleted.

	BASE CLASS = JXScrollableWidget, JExprEditor

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXExprEditor.h"
#include "JFunction.h"
#include "JExprRectList.h"
#include "JUserInputFunction.h"

#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXWindowPainter.h>
#include <jx-af/jx/JXSelectionManager.h>
#include <jx-af/jx/JXTextSelection.h>
#include <jx-af/jx/JXEPSPrinter.h>
#include <jx-af/jx/jXEventUtil.h>
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jx/jXGlobals.h>
#include <jx-af/jx/jXActionDefs.h>

#include <jx-af/jcore/JMinMax.h>
#include <jx-af/jcore/jASCIIConstants.h>
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/JRect.h>
#include <jx-af/jcore/jMath.h>
#include <jx-af/jcore/jAssert.h>

// Edit menu

static const JUtf8Byte* kMacEditMenuStr =
	"    Undo       %k Meta-Z %i" kJXUndoAction
	"%l| Cut        %k Meta-X %i" kJXCutAction
	"  | Copy       %k Meta-C %i" kJXCopyAction
	"  | Paste      %k Meta-V %i" kJXPasteAction
	"  | Clear                %i" kJXClearAction
	"%l| Select all %k Meta-A %i" kJXSelectAllAction;

static const JUtf8Byte* kWinEditMenuStr =
	"    Undo       %h uz %k Ctrl-Z %i" kJXUndoAction
	"%l| Cut        %h tx %k Ctrl-X %i" kJXCutAction
	"  | Copy       %h c  %k Ctrl-C %i" kJXCopyAction
	"  | Paste      %h pv %k Ctrl-V %i" kJXPasteAction
	"  | Clear      %h l            %i" kJXClearAction
	"%l| Select all %h a  %k Ctrl-A %i" kJXSelectAllAction;

struct EditMenuItemInfo
{
	JExprEditor::CmdIndex	cmd;
	const JUtf8Byte*		id;
};

static const EditMenuItemInfo kEditMenuItemInfo[] =
{
{ JExprEditor::kUndoCmd,      kJXUndoAction      },
{ JExprEditor::kCutCmd,       kJXCutAction       },
{ JExprEditor::kCopyCmd,      kJXCopyAction      },
{ JExprEditor::kPasteCmd,     kJXPasteAction     },
{ JExprEditor::kDeleteSelCmd, kJXClearAction     },
{ JExprEditor::kSelectAllCmd, kJXSelectAllAction }
};

// used when setting images

enum
{
	kUndoIndex = 1,
	kCutIndex, kCopyIndex, kPasteIndex, kClearIndex,
	kSelectAllIndex
};

// Math menu

static const JUtf8Byte* kMacMathMenuStr =
	"    Evaluate        %k Meta-="
	"  | Print to EPS..."
	"%l| Negate"
	"  | Apply function"
	"%l| Add argument"
	"  | Move left       %k Meta-<"
	"  | Move right      %k Meta->"
	"%l| Group left      %k Meta-("
	"  | Group right     %k Meta-)"
	"  | Ungroup";

static const JUtf8Byte* kWinMathMenuStr =
	"    Evaluate        %h e %k Ctrl-="
	"  | Print to EPS... %h p"
	"%l| Negate          %h n"
	"  | Apply function  %h f"
	"%l| Add argument    %h a"
	"  | Move left       %h < %k Ctrl-<"
	"  | Move right      %h > %k Ctrl->"
	"%l| Group left      %h ( %k Ctrl-("
	"  | Group right     %h ) %k Ctrl-)"
	"  | Ungroup         %h u";

const JIndex kEvaluateItemIndex = 1;
const JIndex kApplyFnToSelIndex = 4;

static const JExprEditor::CmdIndex kMathMenuItemToCmd[] =
{
	JExprEditor::kEvaluateSelCmd, JExprEditor::kPrintEPSCmd,
	JExprEditor::kNegateSelCmd, JExprEditor::kApplyFnToSelCmd,
	JExprEditor::kAddArgCmd, JExprEditor::kMoveArgLeftCmd,
	JExprEditor::kMoveArgRightCmd,
	JExprEditor::kGroupLeftCmd, JExprEditor::kGroupRightCmd,
	JExprEditor::kUngroupCmd
};
const JSize kMathMenuItemCount = sizeof(kMathMenuItemToCmd)/sizeof(JExprEditor::CmdIndex);

// Function menu

static const JUtf8Byte* kFunctionMenuStr =
	"    absolute value               %k abs"
	"  | square root                  %k sqrt"
	"%l| logarithm (any base)         %k log"
	"  | natural logarithm            %k ln"
	"%l| sine                         %k sin"
	"  | cosine                       %k cos"
	"  | tangent                      %k tan"
	"%l| inverse sine                 %k arcsin"
	"  | inverse cosine               %k arccos"
	"  | inverse tangent              %k arctan"
	"  | inverse tangent(y, x)        %k arctan2"
	"%l| hyperbolic sine              %k sinh"
	"  | hyperbolic cosine            %k cosh"
	"  | hyperbolic tangent           %k tanh"
	"%l| inverse hyperbolic sine      %k arcsinh"
	"  | inverse hyperbolic cosine    %k arccosh"
	"  | inverse hyperbolic tangent   %k arctanh"
	"%l| real part                    %k re"
	"  | imaginary part               %k im"
	"  | phase angle                  %k arg"
	"  | complex conjugate            %k conjugate"
	"  | rotate(z, angle)             %k rotate"
	"%l| minimum                      %k min"
	"  | maximum                      %k max"
	"  | parallel                     %k parallel"
	"%l| algebraic sign               %k sign"
	"  | round to integer             %k round"
	"  | truncate to integer          %k truncate";

// Font menu

static const JUtf8Byte* kMacFontMenuStr = "Normal      %r | Greek      %r";
static const JUtf8Byte* kWinFontMenuStr = "Normal %h n %r | Greek %h g %r";

static const JExprEditor::CmdIndex kFontMenuItemToCmd[] =
{
	JExprEditor::kSetNormalFontCmd,
	JExprEditor::kSetGreekFontCmd
};
const JSize kFontMenuItemCount = sizeof(kFontMenuItemToCmd)/sizeof(JExprEditor::CmdIndex);

/******************************************************************************
 Constructor

 ******************************************************************************/

JXExprEditor::JXExprEditor
	(
	const JVariableList*	varList,
	JXMenuBar*				menuBar,
	JXScrollbarSet*			scrollbarSet,
	JXContainer*			enclosure,
	const HSizingOption		hSizing,
	const VSizingOption		vSizing,
	const JCoordinate		x,
	const JCoordinate		y,
	const JCoordinate		w,
	const JCoordinate		h
	)
	:
	JXScrollableWidget(scrollbarSet, enclosure, hSizing, vSizing, x,y, w,h),
	JExprEditor(varList, JXScrollableWidget::GetFontManager()),
	itsIsSharingEditMenuFlag(false),
	itsIsSharingOtherMenusFlag(false)
{
	CreateMenus(menuBar, nullptr);
	JXExprEditorX();
}

JXExprEditor::JXExprEditor
	(
	const JVariableList*	varList,
	JXMenuBar*				menuBar,
	JXTextMenu*				editMenu,
	JXScrollbarSet*			scrollbarSet,
	JXContainer*			enclosure,
	const HSizingOption		hSizing,
	const VSizingOption		vSizing,
	const JCoordinate		x,
	const JCoordinate		y,
	const JCoordinate		w,
	const JCoordinate		h
	)
	:
	JXScrollableWidget(scrollbarSet, enclosure, hSizing, vSizing, x,y, w,h),
	JExprEditor(varList, JXScrollableWidget::GetFontManager()),
	itsIsSharingEditMenuFlag(true),
	itsIsSharingOtherMenusFlag(false)
{
	CreateMenus(menuBar, editMenu);
	JXExprEditorX();
}

JXExprEditor::JXExprEditor
	(
	const JVariableList*	varList,
	JXExprEditor*			menuProvider,
	JXScrollbarSet*			scrollbarSet,
	JXContainer*			enclosure,
	const HSizingOption		hSizing,
	const VSizingOption		vSizing,
	const JCoordinate		x,
	const JCoordinate		y,
	const JCoordinate		w,
	const JCoordinate		h
	)
	:
	JXScrollableWidget(scrollbarSet, enclosure, hSizing, vSizing, x,y, w,h),
	JExprEditor(varList, JXScrollableWidget::GetFontManager()),
	itsIsSharingEditMenuFlag(true),
	itsIsSharingOtherMenusFlag(true),
	itsEditMenu(menuProvider->itsEditMenu),
	itsMathMenu(menuProvider->itsMathMenu),
	itsFunctionMenu(menuProvider->itsFunctionMenu),
	itsFontMenu(menuProvider->itsFontMenu)
{
	JXExprEditorX();
	menuProvider->itsIsSharingEditMenuFlag = true;
	menuProvider->itsIsSharingOtherMenusFlag = true;
}

// private

void
JXExprEditor::JXExprEditorX()
{
	itsEPSPrinter = jnew JXEPSPrinter(GetDisplay());
	assert( itsEPSPrinter != nullptr );

	// ensure that we have a JFunction to display
	// (also calls EIPBoundsChanged and EIPAdjustNeedTab)

	ClearFunction();

	itsEditMenu->AttachHandlers(this,
		&JXExprEditor::UpdateEditMenu,
		&JXExprEditor::HandleEditMenu);

	itsMathMenu->AttachHandlers(this,
		&JXExprEditor::UpdateMathMenu,
		&JXExprEditor::HandleMathMenu);

	itsFunctionMenu->AttachHandler(this, &JXExprEditor::HandleFunctionMenu);

	itsFontMenu->AttachHandlers(this,
		&JXExprEditor::UpdateFontMenu,
		&JXExprEditor::HandleFontMenu);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXExprEditor::~JXExprEditor()
{
	jdelete itsEPSPrinter;
}

/******************************************************************************
 CreateMenus (private)

 ******************************************************************************/

#include <jx-af/image/jx/jx_edit_undo.xpm>
#include <jx-af/image/jx/jx_edit_cut.xpm>
#include <jx-af/image/jx/jx_edit_copy.xpm>
#include <jx-af/image/jx/jx_edit_paste.xpm>
#include <jx-af/image/jx/jx_edit_clear.xpm>

void
JXExprEditor::CreateMenus
	(
	JXMenuBar*	menuBar,
	JXTextMenu*	editMenu
	)
{
	if (editMenu != nullptr)
	{
		itsEditMenu = editMenu;
	}
	else
	{
		itsEditMenu = menuBar->AppendTextMenu(JGetString("EditMenuTitle::JXGlobal"));
		if (JXMenu::GetDefaultStyle() == JXMenu::kMacintoshStyle)
		{
			itsEditMenu->SetMenuItems(kMacEditMenuStr);
		}
		else
		{
			itsEditMenu->SetShortcuts(JGetString("EditMenuShortcut::JXGlobal"));
			itsEditMenu->SetMenuItems(kWinEditMenuStr);
		}

		itsEditMenu->SetItemImage(kUndoIndex,  jx_edit_undo);
		itsEditMenu->SetItemImage(kCutIndex,   jx_edit_cut);
		itsEditMenu->SetItemImage(kCopyIndex,  jx_edit_copy);
		itsEditMenu->SetItemImage(kPasteIndex, jx_edit_paste);
		itsEditMenu->SetItemImage(kClearIndex, jx_edit_clear);
	}

	itsMathMenu = menuBar->AppendTextMenu(JGetString("MathMenuTitle::JXExprEditor"));
	if (JXMenu::GetDefaultStyle() == JXMenu::kMacintoshStyle)
	{
		itsMathMenu->SetMenuItems(kMacMathMenuStr);
	}
	else
	{
		itsMathMenu->SetShortcuts(JGetString("MathMenuShortcut::JXExprEditor"));
		itsMathMenu->SetMenuItems(kWinMathMenuStr);
	}
	itsMathMenu->SetUpdateAction(JXMenu::kDisableAll);

	itsFunctionMenu = jnew JXTextMenu(itsMathMenu, kApplyFnToSelIndex, menuBar);
	itsFunctionMenu->SetMenuItems(kFunctionMenuStr);
	itsFunctionMenu->SetUpdateAction(JXMenu::kDisableNone);

	itsFontMenu = menuBar->AppendTextMenu(JGetString("FontMenuTitle::JXExprEditor"));
	if (JXMenu::GetDefaultStyle() == JXMenu::kMacintoshStyle)
	{
		itsFontMenu->SetMenuItems(kMacFontMenuStr);
	}
	else
	{
		itsFontMenu->SetShortcuts(JGetString("FontMenuShortcut::JXExprEditor"));
		itsFontMenu->SetMenuItems(kWinFontMenuStr);
	}
}

/******************************************************************************
 EIPRefresh (virtual protected)

 ******************************************************************************/

void
JXExprEditor::EIPRefresh()
{
	Refresh();
}

/******************************************************************************
 EIPRedraw (virtual protected)

 ******************************************************************************/

void
JXExprEditor::EIPRedraw()
{
	Redraw();
}

/******************************************************************************
 EIPBoundsChanged (virtual protected)

	Adjust our bounds to fit the expression.

 ******************************************************************************/

void
JXExprEditor::EIPBoundsChanged()
{
	const JRect newBounds = GetRectList()->GetBoundsRect();
	const JRect apG       = GetApertureGlobal();
	const JCoordinate w   = JMax(newBounds.width(),  apG.width());
	const JCoordinate h   = JMax(newBounds.height(), apG.height());
	SetBounds(w,h);
}

/******************************************************************************
 ApertureResized (virtual protected)

	Adjust our Bounds so the expression remains centered within Aperture.

 ******************************************************************************/

void
JXExprEditor::ApertureResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXScrollableWidget::ApertureResized(dw,dh);
	EIPBoundsChanged();
}

/******************************************************************************
 EIPScrollToRect (virtual protected)

	Scroll the pane to make the given rectangle visible.  Return true
	if scrolling was necessary.

 ******************************************************************************/

bool
JXExprEditor::EIPScrollToRect
	(
	const JRect& r
	)
{
	JPoint delta;
	GetDrawingOffset(&delta);
	JRect r1 = r;
	r1.Shift(delta);
	return ScrollToRect(r1);
}

/******************************************************************************
 EIPScrollForDrag (virtual protected)

 ******************************************************************************/

bool
JXExprEditor::EIPScrollForDrag
	(
	const JPoint& pt
	)
{
	return ScrollForDrag(RendererToBounds(pt));
}

/******************************************************************************
 EIPAdjustNeedTab (virtual protected)

 ******************************************************************************/

void
JXExprEditor::EIPAdjustNeedTab
	(
	const bool needTab
	)
{
	WantInput(true, needTab);
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
JXExprEditor::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	// adjust the origin so the expression is centered in Bounds

	JPoint delta;
	GetDrawingOffset(&delta);
	p.ShiftOrigin(delta);

	// draw the function

	EIPDraw(p);
}

/******************************************************************************
 GetDrawingOffset (private)

 ******************************************************************************/

void
JXExprEditor::GetDrawingOffset
	(
	JPoint* delta
	)
	const
{
	const JRect bounds = GetBounds();

	const JExprRectList* rectList = GetRectList();
	const JRect exprBounds = rectList->GetBoundsRect();

	delta->x = bounds.xcenter() - exprBounds.xcenter();
	delta->y = bounds.ycenter() - exprBounds.ycenter();
}

/******************************************************************************
 Bounds coords <-> Renderer coords (private)

 ******************************************************************************/

JPoint
JXExprEditor::BoundsToRenderer
	(
	const JPoint& pt
	)
	const
{
	JPoint origin;
	GetDrawingOffset(&origin);
	return (pt-origin);
}

JPoint
JXExprEditor::RendererToBounds
	(
	const JPoint& pt
	)
	const
{
	JPoint origin;
	GetDrawingOffset(&origin);
	return (pt+origin);
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
JXExprEditor::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (button == kJXMiddleButton)
	{
		Paste();
	}
	else if (button == kJXLeftButton || button == kJXRightButton)
	{
		EIPHandleMouseDown(BoundsToRenderer(pt),
						   button == kJXRightButton);
	}
	else
	{
		ScrollForWheel(button, modifiers);
	}
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
JXExprEditor::HandleMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	ScrollForDrag(pt);
	EIPHandleMouseDrag(BoundsToRenderer(pt));
}

/******************************************************************************
 HandleMouseUp (virtual protected)

 ******************************************************************************/

void
JXExprEditor::HandleMouseUp
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	EIPHandleMouseUp();
}

/******************************************************************************
 HandleFocusEvent (virtual protected)

 ******************************************************************************/

void
JXExprEditor::HandleFocusEvent()
{
	JXScrollableWidget::HandleFocusEvent();
	EIPActivate();
}

/******************************************************************************
 OKToUnfocus (virtual protected)

 ******************************************************************************/

bool
JXExprEditor::OKToUnfocus()
{
	return JXScrollableWidget::OKToUnfocus() && EndEditing();
}

/******************************************************************************
 HandleUnfocusEvent (virtual protected)

 ******************************************************************************/

void
JXExprEditor::HandleUnfocusEvent()
{
	JXScrollableWidget::HandleUnfocusEvent();
	EIPDeactivate();
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

void
JXExprEditor::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	// we need the delete key, which isn't printable

	if ((c.IsAscii() || c.IsPrint()) && !modifiers.meta() && !modifiers.control())
	{
		EIPHandleKeyPress(c);
	}
	else
	{
		JXScrollableWidget::HandleKeyPress(c, keySym, modifiers);
	}
}

/******************************************************************************
 AdjustCursor (virtual protected)

	Show the IBeam cursor when editing something.

 ******************************************************************************/

void
JXExprEditor::AdjustCursor
	(
	const JPoint&			pt,
	const JXKeyModifiers&	modifiers
	)
{
	if (MouseOnActiveUIF(BoundsToRenderer(pt)))
	{
		DisplayCursor(kJXTextEditCursor);
	}
	else
	{
		JXScrollableWidget::AdjustCursor(pt, modifiers);
	}
}

/******************************************************************************
 UpdateEditMenu (private)

 ******************************************************************************/

void
JXExprEditor::UpdateEditMenu()
{
	if (itsIsSharingEditMenuFlag && !HasFocus())
	{
		return;
	}

	const JArray<bool> enableFlags = GetCmdStatus(nullptr);

	const JSize count = itsEditMenu->GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		CmdIndex cmd;
		if (EditMenuIndexToCmd(i, &cmd) && enableFlags.GetItem(cmd))
		{
			itsEditMenu->EnableItem(i);
		}
	}
}

/******************************************************************************
 HandleEditMenu (private)

 ******************************************************************************/

void
JXExprEditor::HandleEditMenu
	(
	const JIndex item
	)
{
	if (itsIsSharingEditMenuFlag && !HasFocus())
	{
		return;
	}

	CmdIndex cmd;
	if (!EditMenuIndexToCmd(item, &cmd))
	{
		return;
	}

	if (cmd == kUndoCmd)
	{
		Undo();
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
}

/******************************************************************************
 Edit menu index <-> cmd

 ******************************************************************************/

#define ClassName    JXExprEditor
#define IndexToCmdFn EditMenuIndexToCmd
#define CmdToIndexFn EditMenuCmdToIndex
#define MenuVar      itsEditMenu
#define CmdType      EditMenuItemInfo
#define CmdIDList    kEditMenuItemInfo
#include <jx-af/jx/JXMenuItemIDUtil.th>
#undef ClassName
#undef IndexToCmdFn
#undef CmdToIndexFn
#undef MenuVar
#undef CmdCount
#undef CmdIDList

/******************************************************************************
 UpdateMathMenu (private)

 ******************************************************************************/

void
JXExprEditor::UpdateMathMenu()
{
	if (itsIsSharingOtherMenusFlag && !HasFocus())
	{
		return;
	}

	JString evalStr;

	const JArray<bool> enableFlags = GetCmdStatus(&evalStr);
	for (JIndex i=1; i<=kMathMenuItemCount; i++)
	{
		if (enableFlags.GetItem(kMathMenuItemToCmd[i-1]))
		{
			itsMathMenu->EnableItem(i);
		}
	}

	itsMathMenu->SetItemText(kEvaluateItemIndex, evalStr);
}

/******************************************************************************
 HandleMathMenu (private)

 ******************************************************************************/

void
JXExprEditor::HandleMathMenu
	(
	const JIndex item
	)
{
	if ((itsIsSharingOtherMenusFlag && !HasFocus()) ||
		item > kMathMenuItemCount)
	{
		return;
	}

	const JExprEditor::CmdIndex cmd = kMathMenuItemToCmd[ item-1 ];
	if (cmd == kEvaluateSelCmd)
	{
		if (EndEditing())
		{
			DisplaySelectionValue();
		}
	}
	else if (cmd == kPrintEPSCmd)
	{
		if (itsEPSPrinter->ConfirmUserPrintSetup())
		{
			Print(*itsEPSPrinter);
		}
	}
	else if (cmd == kNegateSelCmd)
	{
		NegateSelection();
	}
	else if (cmd == kAddArgCmd)
	{
		AddArgument();
	}
	else if (cmd == kMoveArgLeftCmd)
	{
		MoveArgument(-1);
	}
	else if (cmd == kMoveArgRightCmd)
	{
		MoveArgument(+1);
	}
	else if (cmd == kGroupLeftCmd)
	{
		GroupArguments(-1);
	}
	else if (cmd == kGroupRightCmd)
	{
		GroupArguments(+1);
	}
	else if (cmd == kUngroupCmd)
	{
		UngroupArguments();
	}
}

/******************************************************************************
 HandleFunctionMenu (private)

 ******************************************************************************/

void
JXExprEditor::HandleFunctionMenu
	(
	const JIndex item
	)
{
	if (itsIsSharingOtherMenusFlag && !HasFocus())
	{
		return;
	}

	JString fnName;
	if (itsFunctionMenu->GetItemNMShortcut(item, &fnName))
	{
		ApplyFunctionToSelection(fnName);
	}
}

/******************************************************************************
 UpdateFontMenu (private)

	We select the cmd that cannot be performed, because that is the
	current font.

	If there is more than one such cmd, then no JUIF is active, so
	we leave everything disabled.

 ******************************************************************************/

void
JXExprEditor::UpdateFontMenu()
{
	if (itsIsSharingOtherMenusFlag && !HasFocus())
	{
		return;
	}

	const JArray<bool> enableFlags = GetCmdStatus(nullptr);
	JIndex activeIndex = 0;
	for (JIndex i=1; i<=kFontMenuItemCount; i++)
	{
		const bool enableFlag =
			enableFlags.GetItem(kFontMenuItemToCmd[i-1]);
		if (enableFlag && activeIndex == 0)
		{
			activeIndex = i;
		}
	}

	if (activeIndex != 0)
	{
		itsFontMenu->CheckItem(activeIndex);
		itsFontMenu->EnableAll();
	}
}

/******************************************************************************
 HandleFontMenu (private)

 ******************************************************************************/

void
JXExprEditor::HandleFontMenu
	(
	const JIndex item
	)
{
	if ((itsIsSharingOtherMenusFlag && !HasFocus()) ||
		item > kFontMenuItemCount)
	{
		return;
	}

	const long i = item-1;
	if (kFontMenuItemToCmd[i] == kSetNormalFontCmd)
	{
		SetNormalFont();
	}
	else if (kFontMenuItemToCmd[i] == kSetGreekFontCmd)
	{
		SetGreekFont();
	}
}

/******************************************************************************
 Receive (protected)

	Listen for menu update requests and menu selections.

 ******************************************************************************/

void
JXExprEditor::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	JXScrollableWidget::Receive(sender, message);
	JExprEditor::Receive(sender, message);
}

/******************************************************************************
 EIPClipboardChanged (virtual protected)

 ******************************************************************************/

void
JXExprEditor::EIPClipboardChanged()
{
	const JFunction* f;
	if (GetClipboard(&f))
	{
		const JString text = f->Print();

		auto* data = jnew JXTextSelection(GetDisplay(), text);
		assert( data != nullptr );

		GetSelectionManager()->SetData(kJXClipboardName, data);
	}
}

/******************************************************************************
 EIPOwnsClipboard (virtual protected)

	We can't know if the current data is ours.

 ******************************************************************************/

bool
JXExprEditor::EIPOwnsClipboard()
{
	return false;
}

/******************************************************************************
 EIPGetExternalClipboard (virtual protected)

	Returns true if there is something pasteable on the system clipboard.

 ******************************************************************************/

bool
JXExprEditor::EIPGetExternalClipboard
	(
	JString* text
	)
{
	text->Clear();

	bool gotData = false;
	JXSelectionManager* selManager = GetSelectionManager();

	JArray<Atom> typeList;
	if (selManager->GetAvailableTypes(kJXClipboardName, CurrentTime, &typeList))
	{
		bool canGetText = false;
		Atom textType       = None;

		const JSize typeCount = typeList.GetItemCount();
		for (JIndex i=1; i<=typeCount; i++)
		{
			Atom type = typeList.GetItem(i);
			if (type == XA_STRING ||
				(!canGetText && type == selManager->GetUtf8StringXAtom()))
			{
				canGetText = true;
				textType   = type;
				break;
			}
		}

		Atom returnType;
		unsigned char* data = nullptr;
		JSize dataLength;
		JXSelectionManager::DeleteMethod delMethod;
		if (canGetText &&
			selManager->GetData(kJXClipboardName, CurrentTime, textType,
								&returnType, &data, &dataLength, &delMethod))
		{
			if (returnType == XA_STRING ||
				returnType == selManager->GetUtf8StringXAtom())
			{
				text->Set(reinterpret_cast<JUtf8Byte*>(data), dataLength);
				gotData = true;
			}
			selManager->DeleteData(&data, delMethod);
		}
		else
		{
			JGetUserNotification()->ReportError(JGetString("UnableToPaste::JXExprEditor"));
		}
	}
	else
	{
		JGetUserNotification()->ReportError(JGetString("EmptyClipboard::JXExprEditor"));
	}

	return gotData;
}

/******************************************************************************
 BuildStyledText (virtual)

 ******************************************************************************/

JStyledText*
JXExprEditor::BuildStyledText()
{
	JStyledText* text = jnew StyledText(JXScrollableWidget::GetFontManager());
	assert( text != nullptr );
	return text;
}

/******************************************************************************
 AdjustStylesBeforeBroadcast (virtual protected)

	Draw the empty string using gray.

 ******************************************************************************/

void
JXExprEditor::StyledText::AdjustStylesBeforeBroadcast
	(
	const JString&			text,
	JRunArray<JFont>*		styles,
	JStyledText::TextRange*	recalcRange,
	JStyledText::TextRange*	redrawRange,
	const bool			deletion
	)
{
	JUserInputFunction::AdjustStylesBeforeBroadcast(text, styles, recalcRange, redrawRange, deletion);
}
