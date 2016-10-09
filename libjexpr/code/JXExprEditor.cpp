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

	*** Windows containing JXExprEditors must use the display's colormap.

	BASE CLASS = JXScrollableWidget, JExprEditor

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXExprEditor.h>
#include <JXExprEvalDirector.h>
#include <JFunction.h>
#include <JExprRectList.h>
#include <jParserData.h>

#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXWindowPainter.h>
#include <JXSelectionManager.h>
#include <JXTextSelection.h>
#include <JXEPSPrinter.h>
#include <jXEventUtil.h>
#include <JXColormap.h>
#include <jXGlobals.h>
#include <jXActionDefs.h>
#include <jXKeysym.h>

#include <JMinMax.h>
#include <jASCIIConstants.h>
#include <JString.h>
#include <JRect.h>
#include <jMath.h>
#include <jAssert.h>

// Edit menu

static const JCharacter* kEditMenuTitleStr    = "Edit";
static const JCharacter* kEditMenuShortcutStr = "#E";
static const JCharacter* kMacEditMenuStr =
	"    Undo       %k Meta-Z %i" kJXUndoAction
	"%l| Cut        %k Meta-X %i" kJXCutAction
	"  | Copy       %k Meta-C %i" kJXCopyAction
	"  | Paste      %k Meta-V %i" kJXPasteAction
	"  | Clear                %i" kJXClearAction
	"%l| Select all %k Meta-A %i" kJXSelectAllAction;

static const JCharacter* kWinEditMenuStr =
	"    Undo       %h uz %k Ctrl-Z %i" kJXUndoAction
	"%l| Cut        %h tx %k Ctrl-X %i" kJXCutAction
	"  | Copy       %h c  %k Ctrl-C %i" kJXCopyAction
	"  | Paste      %h pv %k Ctrl-V %i" kJXPasteAction
	"  | Clear      %h l            %i" kJXClearAction
	"%l| Select all %h a  %k Ctrl-A %i" kJXSelectAllAction;

struct EditMenuItemInfo
{
	JExprEditor::CmdIndex	cmd;
	const JCharacter*		id;
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
const JSize kEditMenuItemCount = sizeof(kEditMenuItemInfo)/sizeof(EditMenuItemInfo);

// used when setting images

enum
{
	kUndoIndex = 1,
	kCutIndex, kCopyIndex, kPasteIndex, kClearIndex,
	kSelectAllIndex
};

// Math menu

static const JCharacter* kMathMenuTitleStr    = "Math";
static const JCharacter* kMathMenuShortcutStr = "#M";
static const JCharacter* kMacMathMenuStr =
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

static const JCharacter* kWinMathMenuStr =
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

static const JCharacter* kFunctionMenuStr =
	"    absolute value               %k abs("
	"  | square root                  %k sqrt("
	"%l| logarithm (any base)         %k log("
	"  | natural logarithm            %k ln("
	"%l| sine                         %k sin("
	"  | cosine                       %k cos("
	"  | tangent                      %k tan("
	"%l| inverse sine                 %k arcsin("
	"  | inverse cosine               %k arccos("
	"  | inverse tangent              %k arctan("
	"  | inverse tangent(y, x)        %k arctan2("
	"%l| hyperbolic sine              %k sinh("
	"  | hyperbolic cosine            %k cosh("
	"  | hyperbolic tangent           %k tanh("
	"%l| inverse hyperbolic sine      %k arcsinh("
	"  | inverse hyperbolic cosine    %k arccosh("
	"  | inverse hyperbolic tangent   %k arctanh("
	"%l| real part                    %k re("
	"  | imaginary part               %k im("
	"  | phase angle                  %k arg("
	"  | complex conjugate            %k conjugate("
	"  | rotate(z, angle)             %k rotate("
	"%l| minimum                      %k min("
	"  | maximum                      %k max("
	"  | parallel                     %k parallel("
	"%l| algebraic sign               %k sign("
	"  | round to integer             %k round("
	"  | truncate to integer          %k truncate(";

static JIndex kFnCmdToFnIndex[] =
{
	kJAbsValueNameIndex,
	kJSquareRootNameIndex,

	kJLogBNameIndex,
	kJLogENameIndex,

	kJSineNameIndex,
	kJCosineNameIndex,
	kJTangentNameIndex,

	kJArcSineNameIndex,
	kJArcCosineNameIndex,
	kJArcTangentNameIndex,
	kJArcTangent2NameIndex,

	kJHypSineNameIndex,
	kJHypCosineNameIndex,
	kJHypTangentNameIndex,
	kJArcHypSineNameIndex,
	kJArcHypCosineNameIndex,
	kJArcHypTangentNameIndex,

	kJRealPartNameIndex,
	kJImagPartNameIndex,
	kJPhaseAngleNameIndex,
	kJConjugateNameIndex,
	kJRotateNameIndex,

	kJMinFuncNameIndex,
	kJMaxFuncNameIndex,
	kJParallelNameIndex,

	kJSignNameIndex,
	kJRoundNameIndex,
	kJTruncateNameIndex
};
const JSize kFunctionMenuItemCount = sizeof(kFnCmdToFnIndex)/sizeof(JIndex);

// Font menu

static const JCharacter* kFontMenuTitleStr    = "Font";
static const JCharacter* kFontMenuShortcutStr = "#F";
static const JCharacter* kMacFontMenuStr = "Normal      %r | Greek      %r";
static const JCharacter* kWinFontMenuStr = "Normal %h n %r | Greek %h g %r";

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
	JExprEditor(varList, JXScrollableWidget::GetFontManager(),
				JXScrollableWidget::GetColormap())
{
	JXExprEditorX();
	CreateMenus(menuBar, NULL);
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
	JExprEditor(varList, JXScrollableWidget::GetFontManager(),
				JXScrollableWidget::GetColormap())
{
	JXExprEditorX();
	CreateMenus(menuBar, editMenu);
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
	JExprEditor(varList, JXScrollableWidget::GetFontManager(),
				JXScrollableWidget::GetColormap())
{
	JXExprEditorX();

	// Listen to the specified menus.  This is safe because
	// we know that they were created by another ExprWidget.

	itsEditMenu = menuProvider->itsEditMenu;
	ListenTo(itsEditMenu);

	itsMathMenu = menuProvider->itsMathMenu;
	ListenTo(itsMathMenu);

	itsFunctionMenu = menuProvider->itsFunctionMenu;
	ListenTo(itsFunctionMenu);

	itsFontMenu = menuProvider->itsFontMenu;
	ListenTo(itsFontMenu);
}

// private

void
JXExprEditor::JXExprEditorX()
{
	// required by JXGetCurrColormap
	assert( GetWindow()->GetColormap() == GetDisplay()->GetColormap() );

	itsEPSPrinter = jnew JXEPSPrinter(GetDisplay());
	assert( itsEPSPrinter != NULL );
	ListenTo(itsEPSPrinter);

	// insure that we have a JFunction to display
	// (also calls EIPBoundsChanged and EIPAdjustNeedTab)

	ClearFunction();
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

#include <jx_edit_undo.xpm>
#include <jx_edit_cut.xpm>
#include <jx_edit_copy.xpm>
#include <jx_edit_paste.xpm>
#include <jx_edit_clear.xpm>

void
JXExprEditor::CreateMenus
	(
	JXMenuBar*	menuBar,
	JXTextMenu*	editMenu
	)
{
	if (editMenu != NULL)
		{
		itsEditMenu = editMenu;
		}
	else
		{
		itsEditMenu = menuBar->AppendTextMenu(kEditMenuTitleStr);
		if (JXMenu::GetDefaultStyle() == JXMenu::kMacintoshStyle)
			{
			itsEditMenu->SetMenuItems(kMacEditMenuStr);
			}
		else
			{
			itsEditMenu->SetShortcuts(kEditMenuShortcutStr);
			itsEditMenu->SetMenuItems(kWinEditMenuStr);
			}

		itsEditMenu->SetItemImage(kUndoIndex,  jx_edit_undo);
		itsEditMenu->SetItemImage(kCutIndex,   jx_edit_cut);
		itsEditMenu->SetItemImage(kCopyIndex,  jx_edit_copy);
		itsEditMenu->SetItemImage(kPasteIndex, jx_edit_paste);
		itsEditMenu->SetItemImage(kClearIndex, jx_edit_clear);
		}
	ListenTo(itsEditMenu);

	itsMathMenu = menuBar->AppendTextMenu(kMathMenuTitleStr);
	if (JXMenu::GetDefaultStyle() == JXMenu::kMacintoshStyle)
		{
		itsMathMenu->SetMenuItems(kMacMathMenuStr);
		}
	else
		{
		itsMathMenu->SetShortcuts(kMathMenuShortcutStr);
		itsMathMenu->SetMenuItems(kWinMathMenuStr);
		}
	itsMathMenu->SetUpdateAction(JXMenu::kDisableAll);
	ListenTo(itsMathMenu);

	itsFunctionMenu = jnew JXTextMenu(itsMathMenu, kApplyFnToSelIndex, menuBar);
	assert( itsFunctionMenu != NULL );
	itsFunctionMenu->SetMenuItems(kFunctionMenuStr);
	itsFunctionMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFunctionMenu);

	itsFontMenu = menuBar->AppendTextMenu(kFontMenuTitleStr);
	if (JXMenu::GetDefaultStyle() == JXMenu::kMacintoshStyle)
		{
		itsFontMenu->SetMenuItems(kMacFontMenuStr);
		}
	else
		{
		itsFontMenu->SetShortcuts(kFontMenuShortcutStr);
		itsFontMenu->SetMenuItems(kWinFontMenuStr);
		}
	ListenTo(itsFontMenu);
}

/******************************************************************************
 GetMultiplicationString (virtual protected)

 ******************************************************************************/

const JCharacter*
JXExprEditor::GetMultiplicationString()
	const
{
	return "\xB7";
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

	Scroll the pane to make the given rectangle visible.  Return kJTrue
	if scrolling was necessary.

 ******************************************************************************/

JBoolean
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

JBoolean
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
	const JBoolean needTab
	)
{
	WantInput(kJTrue, needTab);
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
						   JConvertToBoolean(button == kJXRightButton));
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

JBoolean
JXExprEditor::OKToUnfocus()
{
	return JConvertToBoolean( JXScrollableWidget::OKToUnfocus() && EndEditing() );
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
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	if (0 < key && key <= 255 &&
		!modifiers.meta() && !modifiers.control())
		{
		EIPHandleKeyPress(key);
		}
	else
		{
		JXScrollableWidget::HandleKeyPress(key, modifiers);
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
	const JArray<JBoolean> enableFlags = GetCmdStatus(NULL);

	const JSize count = itsEditMenu->GetItemCount();
	for (JIndex i=1; i<=count; i++)
		{
		CmdIndex cmd;
		if (EditMenuIndexToCmd(i, &cmd) && enableFlags.GetElement(cmd))
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
#define CmdCount     kEditMenuItemCount
#define CmdIDList    kEditMenuItemInfo
#include <JXMenuItemIDUtil.th>
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
	const JCharacter* evalStr;

	const JArray<JBoolean> enableFlags = GetCmdStatus(&evalStr);
	for (JIndex i=1; i<=kMathMenuItemCount; i++)
		{
		if (enableFlags.GetElement(kMathMenuItemToCmd[i-1]))
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
	if (item > kMathMenuItemCount)
		{
		return;
		}

	const JExprEditor::CmdIndex cmd = kMathMenuItemToCmd[ item-1 ];
	if (cmd == kEvaluateSelCmd)
		{
		if (EndEditing())
			{
			EvaluateSelection();
			}
		}
	else if (cmd == kPrintEPSCmd)
		{
		itsEPSPrinter->BeginUserPrintSetup();
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
 UpdateFunctionMenu (private)

 ******************************************************************************/

void
JXExprEditor::UpdateFunctionMenu()
{
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
	if (item <= kFunctionMenuItemCount)
		{
		ApplyFunctionToSelection(
			JPGetStdFnName(kFnCmdToFnIndex[item-1]));
		}
}

/******************************************************************************
 EvaluateSelection (virtual)

 ******************************************************************************/

void
JXExprEditor::EvaluateSelection()
	const
{
	const JFunction* f;
	if (!GetConstSelectedFunction(&f))
		{
		f = GetFunction();
		}

	JXExprEvalDirector* newDir =
		jnew JXExprEvalDirector(GetWindow()->GetDirector(),
							   GetVariableList(), *f);
	assert( newDir != NULL );
	newDir->Activate();
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
	const JArray<JBoolean> enableFlags = GetCmdStatus(NULL);
	JIndex activeIndex = 0;
	for (JIndex i=1; i<=kFontMenuItemCount; i++)
		{
		const JBoolean enableFlag =
			enableFlags.GetElement(kFontMenuItemToCmd[i-1]);
		if (!enableFlag && activeIndex == 0)
			{
			activeIndex = i;
			}
		else if (!enableFlag)
			{
			return;
			}
		}

	assert( activeIndex > 0 );
	itsFontMenu->CheckItem(activeIndex);
	itsFontMenu->EnableAll();
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
	if (item > kFontMenuItemCount)
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
	if (sender == itsEditMenu && HasFocus() &&
		message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateEditMenu();
		}
	else if (sender == itsEditMenu && HasFocus() &&
			 message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleEditMenu(selection->GetIndex());
		}

	else if (sender == itsMathMenu && HasFocus() &&
			 message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateMathMenu();
		}
	else if (sender == itsMathMenu && HasFocus() &&
			 message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleMathMenu(selection->GetIndex());
		}

	else if (sender == itsFunctionMenu && HasFocus() &&
			 message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateFunctionMenu();
		}
	else if (sender == itsFunctionMenu && HasFocus() &&
			 message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleFunctionMenu(selection->GetIndex());
		}

	else if (sender == itsFontMenu && HasFocus() &&
		message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateFontMenu();
		}
	else if (sender == itsFontMenu && HasFocus() &&
			 message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleFontMenu(selection->GetIndex());
		}

	else if (sender == itsEPSPrinter &&
			 message.Is(JPrinter::kPrintSetupFinished))
		{
		const JPrinter::PrintSetupFinished* info =
			dynamic_cast<const JPrinter::PrintSetupFinished*>(&message);
		assert( info != NULL );
		if (info->Successful())
			{
			Print(*itsEPSPrinter);
			}
		}

	else
		{
		JXScrollableWidget::Receive(sender, message);
		JExprEditor::Receive(sender, message);
		}
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

		JXTextSelection* data = jnew JXTextSelection(GetDisplay(), text);
		assert( data != NULL );

		GetSelectionManager()->SetData(kJXClipboardName, data);
		}
}

/******************************************************************************
 EIPOwnsClipboard (virtual protected)

	We can't know if the current data is ours.

 ******************************************************************************/

JBoolean
JXExprEditor::EIPOwnsClipboard()
{
	return kJFalse;
}

/******************************************************************************
 EIPGetExternalClipboard (virtual protected)

	Returns kJTrue if there is something pasteable on the system clipboard.

 ******************************************************************************/

JBoolean
JXExprEditor::EIPGetExternalClipboard
	(
	JString* text
	)
{
	text->Clear();

	JBoolean gotData = kJFalse;
	JXSelectionManager* selManager = GetSelectionManager();

	JArray<Atom> typeList;
	if (selManager->GetAvailableTypes(kJXClipboardName, CurrentTime, &typeList))
		{
		JBoolean canGetText = kJFalse;
		Atom textType       = None;

		const JSize typeCount = typeList.GetElementCount();
		for (JIndex i=1; i<=typeCount; i++)
			{
			Atom type = typeList.GetElement(i);
			if (type == XA_STRING ||
				(!canGetText && type == selManager->GetTextXAtom()))
				{
				canGetText = kJTrue;
				textType   = type;
				break;
				}
			}

		Atom returnType;
		unsigned char* data = NULL;
		JSize dataLength;
		JXSelectionManager::DeleteMethod delMethod;
		if (canGetText &&
			selManager->GetData(kJXClipboardName, CurrentTime, textType,
								&returnType, &data, &dataLength, &delMethod))
			{
			if (returnType == XA_STRING)
				{
				*text = JString(reinterpret_cast<JCharacter*>(data), dataLength);
				gotData = kJTrue;
				}
			selManager->DeleteData(&data, delMethod);
			}
		else
			{
			(JGetUserNotification())->ReportError(
				"Unable to paste the current contents of the X Clipboard.");
			}
		}
	else
		{
		(JGetUserNotification())->ReportError("The X Clipboard is empty.");
		}

	return gotData;
}
