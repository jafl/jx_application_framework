/******************************************************************************
 JXButton.cpp

	Maintains a pushable button.

	BASE CLASS = JXWidget

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include <JXButton.h>
#include <JXWindow.h>
#include <JXWindowPainter.h>
#include <jXPainterUtil.h>
#include <JXColormap.h>
#include <JString.h>

// JBroadcaster message types

const JUtf8Byte* JXButton::kPushed = "Pushed::JXButton";

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXButton::JXButton
	(
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXWidget(enclosure, hSizing, vSizing, x,y, w,h)
{
	itsIsReturnButtonFlag = kJFalse;
	itsIsPushedFlag       = kJFalse;

	SetBorderWidth(kJXDefaultBorderWidth);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXButton::~JXButton()
{
}

/******************************************************************************
 Push

	Provides a way for the program to push its own buttons.

 ******************************************************************************/

void
JXButton::Push()
{
	itsIsPushedFlag = kJTrue;
	Redraw();
	itsIsPushedFlag = kJFalse;
	Refresh();
	Broadcast(Pushed());	// must be last since it could delete us
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
JXButton::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		origFrame
	)
{
	JSize borderWidth = GetBorderWidth();
	if (borderWidth > 0 && IsActive())
		{
		JRect frame = origFrame;
		if (itsIsReturnButtonFlag)
			{
			p.JPainter::Rect(frame);
			frame.Shrink(1,1);
			borderWidth--;
			}

		if (itsIsPushedFlag)
			{
			JXDrawDownFrame(p, frame, borderWidth);
			}
		else
			{
			JXDrawUpFrame(p, frame, borderWidth);
			}
		}
	else if (borderWidth > 0)
		{
		p.SetLineWidth(borderWidth);
		p.SetPenColor(GetColormap()->GetInactiveLabelColor());
		p.RectInside(origFrame);
		}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
JXButton::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (button == kJXLeftButton)
		{
		itsIsPushedFlag = kJTrue;
		Redraw();
		}
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
JXButton::HandleMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (buttonStates.left())
		{
		const JRect frame     = JXContainer::GlobalToLocal(GetFrameGlobal());
		const JBoolean inside = frame.Contains(pt);
		if (inside && !itsIsPushedFlag)
			{
			itsIsPushedFlag = kJTrue;
			Redraw();
			}
		else if (!inside && itsIsPushedFlag)
			{
			itsIsPushedFlag = kJFalse;
			Redraw();
			}
		}
}

/******************************************************************************
 HandleMouseUp (virtual protected)

 ******************************************************************************/

void
JXButton::HandleMouseUp
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (button == kJXLeftButton && itsIsPushedFlag)
		{
		itsIsPushedFlag = kJFalse;
		Refresh();
		Broadcast(Pushed());	// must be last since it could delete us
		}
}

/******************************************************************************
 HandleShortcut

	All shortcuts trigger the button.

 ******************************************************************************/

void
JXButton::HandleShortcut
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	Push();
}

/******************************************************************************
 SetShortcuts (virtual)

	Derived classes can override to adjust their appearance.

 ******************************************************************************/

void
JXButton::SetShortcuts
	(
	const JString& shortcuts
	)
{
	JXWindow* w = GetWindow();
	w->ClearShortcuts(this);
	w->InstallShortcuts(this, shortcuts);

	const JBoolean wasReturnButton = itsIsReturnButtonFlag;
	itsIsReturnButtonFlag = kJFalse;
	if (!shortcuts.IsEmpty() &&
		(shortcuts.Contains("^M") || shortcuts.Contains("^m")))
		{
		itsIsReturnButtonFlag = kJTrue;
		}

	const JSize borderWidth = GetBorderWidth();
	if (!wasReturnButton && itsIsReturnButtonFlag)
		{
		SetBorderWidth(borderWidth+1);
		Move(-1, -1);
		AdjustSize(+2, +2);
		}
	else if (wasReturnButton && !itsIsReturnButtonFlag && borderWidth > 0)
		{
		SetBorderWidth(borderWidth-1);
		Move(+1, +1);
		AdjustSize(-2, -2);
		}

	Refresh();
}

/******************************************************************************
 GetFrameForFTC

	Assumes that widgets are laid out in nested tables and do not overlap.
	Builds the implicit tables and expands the cells and their contents so
	text is not cut off.

 ******************************************************************************/

JRect
JXButton::GetFrameForFTC()
	const
{
	JRect r = JXWidget::GetFrameForFTC();
	if (itsIsReturnButtonFlag)
		{
		r.Shrink(1, 1);
		}
	return r;
}
