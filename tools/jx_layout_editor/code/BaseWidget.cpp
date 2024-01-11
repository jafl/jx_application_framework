/******************************************************************************
 BaseWidget.cpp

	BASE CLASS = JXWidget

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "BaseWidget.h"
#include "LayoutContainer.h"
#include "LayoutSelection.h"
#include "WidgetParametersDialog.h"
#include <jx-af/jx/JXWindowPainter.h>
#include <jx-af/jx/jXPainterUtil.h>
#include <jx-af/jx/JXMenu.h>
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jcore/JUndoRedoChain.h>
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jMouseUtil.h>
#include <X11/cursorfont.h>
#include <jx-af/jcore/JStringManager.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

const JCoordinate kHandleHalfWidth      = 5;
const JCoordinate kTabIndexHMarginWidth = 2;
const JCoordinate kTabIndexVMarginWidth = 1;

/******************************************************************************
 Constructor

 ******************************************************************************/

BaseWidget::BaseWidget
	(
	LayoutContainer*	layout,
	const bool			wantsInput,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXWidget(enclosure, hSizing, vSizing, x,y, w,h),
	itsLayout(layout),
	itsIsMemberVarFlag(false),
	itsSelectedFlag(false),
	itsTabIndex(0)
{
	BaseWidgetX();

	// itsVarName must first be initialized
	itsVarName = itsLayout->GenerateUniqueVarName();

	// itsTabIndex must first be initialized
	if (wantsInput)
	{
		itsTabIndex = itsLayout->GetNextTabIndex();
	}
}

BaseWidget::BaseWidget
	(
	LayoutContainer*	layout,
	std::istream&		input,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXWidget(enclosure, hSizing, vSizing, x,y, w,h),
	itsLayout(layout),
	itsSelectedFlag(false)
{
	BaseWidgetX();

	input >> itsVarName >> itsIsMemberVarFlag >> itsTabIndex;
}

// private

void
BaseWidget::BaseWidgetX()
{
	itsCursors[ kTopLeftHandle     ] = GetDisplay()->CreateBuiltInCursor("XC_top_left_corner", XC_top_left_corner);
	itsCursors[ kTopHandle         ] = GetDisplay()->CreateBuiltInCursor("XC_top_side", XC_top_side);
	itsCursors[ kTopRightHandle    ] = GetDisplay()->CreateBuiltInCursor("XC_top_right_corner", XC_top_right_corner);
	itsCursors[ kRightHandle       ] = GetDisplay()->CreateBuiltInCursor("XC_right_side", XC_right_side);
	itsCursors[ kBottomRightHandle ] = GetDisplay()->CreateBuiltInCursor("XC_bottom_right_corner", XC_bottom_right_corner);
	itsCursors[ kBottomHandle      ] = GetDisplay()->CreateBuiltInCursor("XC_bottom_side", XC_bottom_side);
	itsCursors[ kBottomLeftHandle  ] = GetDisplay()->CreateBuiltInCursor("XC_bottom_left_corner", XC_bottom_left_corner);
	itsCursors[ kLeftHandle        ] = GetDisplay()->CreateBuiltInCursor("XC_left_side", XC_left_side);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

BaseWidget::~BaseWidget()
{
}

/******************************************************************************
 StreamOut (virtual)

	Remember to keep LayoutDocument::ReadFile in sync!

 ******************************************************************************/

void
BaseWidget::StreamOut
	(
	std::ostream& output
	)
	const
{
	// read by LayoutDocument

	output << (int) GetHSizing() << std::endl;
	output << (int) GetVSizing() << std::endl;
	output << GetFrame() << std::endl;

	// read by ctor

	output << itsVarName << std::endl;
	output << itsIsMemberVarFlag << std::endl;
	output << itsTabIndex << std::endl;
}

/******************************************************************************
 ToString (virtual)

 ******************************************************************************/

JString
BaseWidget::ToString()
	const
{
	JString s = JGetString("VariableNameLabel::BaseWidget");
	s += itsVarName;
	if (itsIsMemberVarFlag)
	{
		s += JGetString("MemberFlagLabel::BaseWidget");
	}
	return s;
}

/******************************************************************************
 GenerateCode

 ******************************************************************************/

void
BaseWidget::GenerateCode
	(
	std::ostream&		output,
	const JString&		indent,
	JPtrArray<JString>*	objTypes,
	JPtrArray<JString>*	objNames,
	JStringManager*		stringdb
	)
	const
{
	indent.Print(output);
	if (itsIsMemberVarFlag)
	{
		objTypes->Append(GetClassName());
		objNames->Append(itsVarName);
	}
	else
	{
		output << "auto* ";
	}
	itsVarName.Print(output);
	output << " =" << std::endl;

	indent.Print(output);
	indent.Print(output);
	GetCtor().Print(output);
	output << '(';
	PrintCtorArgsWithComma(output, itsVarName, stringdb);
	itsLayout->GetEnclosureName().Print(output);
	output << ',' << std::endl;

	indent.Print(output);
	indent.Print(output);
	indent.Print(output);
	indent.Print(output);
	indent.Print(output);
	output << "JXWidget::"
		<< (GetHSizing() == kFixedLeft  ? "kFixedLeft" :
			GetHSizing() == kFixedRight ? "kFixedRight" : "kHElastic") << ", ";
	output << "JXWidget::"
		<< (GetVSizing() == kFixedTop    ? "kFixedTop" :
			GetVSizing() == kFixedBottom ? "kFixedBottom" : "kVElastic") << ", ";

	const JRect f = GetFrame();
	output << f.left << ',' << f.top << ", " << f.width() << ',' << f.height();
	output << ");" << std::endl;

	PrintConfiguration(output, indent, itsVarName, stringdb);
	output << std::endl;
}

/******************************************************************************
 GetCtor (virtual protected)

 ******************************************************************************/

JString
BaseWidget::GetCtor()
	const
{
	return "jnew " + GetClassName();
}

/******************************************************************************
 SetSelected

 ******************************************************************************/

void
BaseWidget::SetSelected
	(
	const bool on
	)
{
	if (on != itsSelectedFlag)
	{
		itsSelectedFlag = on;
		itsLayout->Refresh();
	}
}

/******************************************************************************
 SetWantsInput (protected)

 ******************************************************************************/

void
BaseWidget::SetWantsInput
	(
	const bool wantsInput
	)
{
	if (wantsInput && itsTabIndex == 0)
	{
		itsTabIndex = itsLayout->GetNextTabIndex();
	}
	else if (!wantsInput && itsTabIndex > 0)
	{
		const JIndex i = itsTabIndex;
		itsTabIndex    = 0;
		itsLayout->TabIndexRemoved(i);
	}
}

/******************************************************************************
 SetTabIndex

	Only allowed to set tab index for widgets that enabled it.

 ******************************************************************************/

void
BaseWidget::SetTabIndex
	(
	const JIndex i
	)
{
	assert( itsTabIndex > 0 );

	itsTabIndex = i;
	Refresh();
}

/******************************************************************************
 DrawSelection (protected)

 ******************************************************************************/

void
BaseWidget::DrawSelection
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	if (itsSelectedFlag)
	{
		p.SetPenColor(JColorManager::GetDefaultSelectionColor());
		p.Rect(rect);
	}
}

/******************************************************************************
 DrawOver (virtual protected)

 ******************************************************************************/

void
BaseWidget::DrawOver
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	SetHint(ToString());

	for (auto& r : itsHandles)
	{
		r.SetSize(0,0);
	}

	JRect f = GetFrameLocal();
	if (itsSelectedFlag && itsLayout->GetSelectionCount() == 1)
	{
		itsHandles[ kTopLeftHandle     ].Place(f.top, f.left);
		itsHandles[ kTopHandle         ].Place(f.top, f.xcenter() - kHandleHalfWidth);
		itsHandles[ kTopRightHandle    ].Place(f.top, f.right - 2*kHandleHalfWidth);
		itsHandles[ kRightHandle       ].Place(f.ycenter() - kHandleHalfWidth, f.right - 2*kHandleHalfWidth);
		itsHandles[ kBottomRightHandle ].Place(f.bottom - 2*kHandleHalfWidth, f.right - 2*kHandleHalfWidth);
		itsHandles[ kBottomHandle      ].Place(f.bottom - 2*kHandleHalfWidth, f.xcenter() - kHandleHalfWidth);
		itsHandles[ kBottomLeftHandle  ].Place(f.bottom - 2*kHandleHalfWidth, f.left);
		itsHandles[ kLeftHandle        ].Place(f.ycenter() - kHandleHalfWidth, f.left);

		itsHandles[ kTopLeftHandle     ].SetSize(2*kHandleHalfWidth, 2*kHandleHalfWidth);
		itsHandles[ kTopRightHandle    ].SetSize(2*kHandleHalfWidth, 2*kHandleHalfWidth);
		itsHandles[ kBottomRightHandle ].SetSize(2*kHandleHalfWidth, 2*kHandleHalfWidth);
		itsHandles[ kBottomLeftHandle  ].SetSize(2*kHandleHalfWidth, 2*kHandleHalfWidth);

		if (f.width() > 6*kHandleHalfWidth)
		{
			itsHandles[ kTopHandle    ].SetSize(2*kHandleHalfWidth, 2*kHandleHalfWidth);
			itsHandles[ kBottomHandle ].SetSize(2*kHandleHalfWidth, 2*kHandleHalfWidth);
		}

		if (f.height() > 6*kHandleHalfWidth)
		{
			itsHandles[ kRightHandle ].SetSize(2*kHandleHalfWidth, 2*kHandleHalfWidth);
			itsHandles[ kLeftHandle  ].SetSize(2*kHandleHalfWidth, 2*kHandleHalfWidth);
		}

		p.SetPenColor(JColorManager::GetDefaultSelectionColor());
		p.SetFilling(true);
		for (auto& r : itsHandles)
		{
			p.Rect(r);
		}
	}

	p.ResetAllButClipping();
	p.SetLineWidth(2);
	p.SetPenColor(JColorManager::GetOrangeColor());

	if (!itsSelectedFlag && GetHSizing() == JXWidget::kFixedLeft)
	{
		f.Shrink(1,0);
		p.Line(f.topLeft(), f.bottomLeft());
	}
	else if (!itsSelectedFlag && GetHSizing() == JXWidget::kFixedRight)
	{
		f.Shrink(2,0);
		p.Line(f.topRight(), f.bottomRight());
	}

	if (!itsSelectedFlag && GetVSizing() == JXWidget::kFixedTop)
	{
		f.Shrink(0,1);
		p.Line(f.topLeft(), f.topRight());
	}
	else if (!itsSelectedFlag && GetVSizing() == JXWidget::kFixedBottom)
	{
		f.Shrink(0,2);
		p.Line(f.bottomLeft(), f.bottomRight());
	}

	p.ResetAllButClipping();
	p.SetFontSize(8);

	if (itsTabIndex > 0)
	{
		const JString s((JUInt64) itsTabIndex);
		const auto w = p.GetStringWidth(s);
		const auto h = p.GetLineHeight();

		JPoint pt(JMax(JSize(0), f.width()/4 - w/2 - kTabIndexHMarginWidth), 0);
		JRect r;
		r.Place(pt);
		r.SetSize(w + 2*kTabIndexHMarginWidth, h + 2*kTabIndexVMarginWidth);
		p.SetPenColor(JColorManager::GetGrayColor(70));
		p.SetFilling(true);
		p.Rect(r);

		p.SetPenColor(JColorManager::GetBlackColor());
		pt += JPoint(kTabIndexHMarginWidth, kTabIndexVMarginWidth);
		p.String(pt, s);
	}
}

/******************************************************************************
 AdjustCursor (virtual protected)

 ******************************************************************************/

void
BaseWidget::AdjustCursor
	(
	const JPoint&			pt,
	const JXKeyModifiers&	modifiers
	)
{
	bool found = false;

	JUnsignedOffset i = 0;
	for (auto& r : itsHandles)
	{
		if (r.Contains(pt))
		{
			DisplayCursor(itsCursors[i]);
			found = true;
			break;
		}
		i++;
	}

	if (!found)
	{
		JXWidget::AdjustCursor(pt, modifiers);
	}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
BaseWidget::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	itsStartPtG = itsPrevPtG = LocalToGlobal(pt);
	itsLastClickCount        = 0;
	itsWaitingForDragFlag    = false;
	itsClearIfNotDNDFlag     = false;
	itsIsResizingFlag        = false;

	if (button == kJXLeftButton && clickCount == 1)
	{
		itsResizeDragType = 0;
		for (const auto& r : itsHandles)
		{
			if (r.Contains(pt))
			{
				return;
			}
			itsResizeDragType++;
		}
	}
	itsResizeDragType = kHandleCount;

	if (button == kJXLeftButton && clickCount == 1 && modifiers.shift())
	{
		if (!IsSelected())
		{
			JPtrArray<BaseWidget> list(JPtrArrayT::kForgetAll);
			if (!itsLayout->GetSelectedWidgets(&list) ||
				list.GetFirstItem()->GetEnclosure() == GetEnclosure())
			{
				SetSelected(true);
			}
		}
		else if (IsSelected())
		{
			SetSelected(false);
		}
	}
	else if (button == kJXLeftButton && clickCount > 1)
	{
		itsWaitingForDragFlag = itsLayout->HasSelection();
		itsLastClickCount     = clickCount;		// save for HandleMouseUp()
	}
	else if (button == kJXLeftButton)
	{
		itsWaitingForDragFlag = true;

		if (itsSelectedFlag)
		{
			itsClearIfNotDNDFlag = true;
		}
		else
		{
			itsLayout->ClearSelection();
			SetSelected(true);
		}
	}
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
BaseWidget::HandleMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	const JPoint ptG = LocalToGlobal(pt);
	const bool moved = JMouseMoved(itsStartPtG, ptG);

	if (itsWaitingForDragFlag && moved)
	{
		assert( itsLayout->HasSelection() );

		itsWaitingForDragFlag = false;
		itsClearIfNotDNDFlag  = false;

		itsLayout->SetSelectedWidgetsVisible(false);

		auto* data = jnew LayoutSelection(itsLayout, itsStartPtG);
		BeginDND(pt, buttonStates, modifiers, data);
		return;
	}
	else if (!itsIsResizingFlag && itsResizeDragType < kHandleCount && moved)
	{
		itsIsResizingFlag = true;
	}
	else if (!itsIsResizingFlag)
	{
		return;
	}

	JPoint gridPtG =
		itsLayout->LocalToGlobal(
			itsLayout->SnapToGrid(
				itsLayout->GlobalToLocal(ptG)));

	JPoint delta = gridPtG - itsPrevPtG;
	if (delta.x == 0 && delta.y == 0)
	{
		return;
	}

	LayoutUndo* undo = nullptr;
	if (!itsLayout->CurrentUndoIs(LayoutUndo::kDragResizeType))
	{
		undo = jnew LayoutUndo(itsLayout->GetDocument(), LayoutUndo::kDragResizeType);
	}

	if (itsResizeDragType == kTopLeftHandle)
	{
		Move(delta.x, delta.y);
		AdjustSize(-delta.x, -delta.y);

		delta = itsLayout->SnapToGrid(this);
		AdjustSize(-delta.x, -delta.y);
	}
	else if (itsResizeDragType == kTopHandle)
	{
		Move(0, delta.y);
		AdjustSize(0, -delta.y);

		delta = itsLayout->SnapToGrid(this);
		AdjustSize(0, -delta.y);
	}
	else if (itsResizeDragType == kTopRightHandle)
	{
		Move(0, delta.y);
		AdjustSize(delta.x, -delta.y);

		delta = itsLayout->SnapToGrid(this);
		AdjustSize(delta.x, -delta.y);
	}
	else if (itsResizeDragType == kRightHandle)
	{
		AdjustSize(delta.x, 0);

		delta = itsLayout->SnapToGrid(this);
		AdjustSize(delta.x, 0);
	}
	else if (itsResizeDragType == kBottomRightHandle)
	{
		AdjustSize(delta.x, delta.y);

		delta = itsLayout->SnapToGrid(this);
		AdjustSize(delta.x, delta.y);
	}
	else if (itsResizeDragType == kBottomHandle)
	{
		AdjustSize(0, delta.y);

		delta = itsLayout->SnapToGrid(this);
		AdjustSize(0, delta.y);
	}
	else if (itsResizeDragType == kBottomLeftHandle)
	{
		Move(delta.x, 0);
		AdjustSize(-delta.x, delta.y);

		delta = itsLayout->SnapToGrid(this);
		AdjustSize(-delta.x, delta.y);
	}
	else if (itsResizeDragType == kLeftHandle)
	{
		Move(delta.x, 0);
		AdjustSize(-delta.x, 0);

		delta = itsLayout->SnapToGrid(this);
		AdjustSize(-delta.x, 0);
	}

	if (undo != nullptr)
	{
		itsLayout->NewUndo(undo);
	}

	itsPrevPtG = gridPtG;
}

/******************************************************************************
 HandleMouseUp (virtual protected)

 ******************************************************************************/

void
BaseWidget::HandleMouseUp
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	itsLayout->GetUndoRedoChain()->DeactivateCurrentUndo();

	if (itsWaitingForDragFlag && itsLastClickCount == 2)
	{
		auto* dlog = jnew WidgetParametersDialog(itsVarName, itsIsMemberVarFlag,
												 GetHSizing(), GetVSizing());
		AddPanels(dlog);

		auto* undo = jnew LayoutUndo(itsLayout->GetDocument());

		if (dlog->DoDialog())
		{
			itsVarName = dlog->GetVarName(&itsIsMemberVarFlag);
			SetSizing(dlog->GetHSizing(), dlog->GetVSizing());
			SavePanelData();
			itsLayout->NewUndo(undo);
			Refresh();
		}
		else
		{
			jdelete undo;
		}
	}
	else if (itsClearIfNotDNDFlag)
	{
		itsLayout->ClearSelection();
		SetSelected(true);
	}
}

/******************************************************************************
 GetDNDAction (virtual protected)

	This is called repeatedly during the drag so the drop action can be
	changed based on the current target, buttons, and modifier keys.

 ******************************************************************************/

Atom
BaseWidget::GetDNDAction
	(
	const JXContainer*		target,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	const JXMenu::Style style = JXMenu::GetDisplayStyle();
	bool move =
		(style == JXMenu::kMacintoshStyle && modifiers.meta()) ||
		(style == JXMenu::kWindowsStyle   && modifiers.control());

	if (target == itsLayout)
	{
		move = !move;
	}

	itsLayout->SetSelectedWidgetsVisible(!move);

	return (move ?
			GetDNDManager()->GetDNDActionMoveXAtom() : 
			GetDNDManager()->GetDNDActionCopyXAtom());
}

/******************************************************************************
 DNDFinish (virtual protected)

	This is called when DND is terminated, but before the actual data
	transfer.

	If it is not a drop, or the drop target is not within the same
	application, target is nullptr.

 ******************************************************************************/

void
BaseWidget::DNDFinish
	(
	const bool			isDrop,
	const JXContainer*	target
	)
{
	if (!isDrop)
	{
		itsLayout->SetSelectedWidgetsVisible(true);
	}
}
