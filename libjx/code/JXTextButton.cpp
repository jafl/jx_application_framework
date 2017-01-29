/******************************************************************************
 JXTextButton.cpp

	Maintains a pushable button with a text label.

	BASE CLASS = JXButton

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXTextButton.h>
#include <JXWindow.h>
#include <JXWindowPainter.h>
#include <JXFontManager.h>
#include <JXColormap.h>
#include <jXGlobals.h>
#include <JStringIterator.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXTextButton::JXTextButton
	(
	const JString&		label,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXButton(enclosure, hSizing, vSizing, x,y, w,h),
	itsLabel(label),
	itsFont(GetFontManager()->GetDefaultFont())
{
	itsShortcuts = NULL;
	itsULIndex   = 0;

	itsPushedColor = GetColormap()->GetDefaultBackColor();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXTextButton::~JXTextButton()
{
	jdelete itsShortcuts;
}

/******************************************************************************
 Set label

 ******************************************************************************/

void
JXTextButton::SetLabel
	(
	const JString& label
	)
{
	itsLabel = label;
	CalcULIndex();
	Refresh();
}

/******************************************************************************
 SetShortcuts (virtual)

 ******************************************************************************/

void
JXTextButton::SetShortcuts
	(
	const JString& list
	)
{
	JXButton::SetShortcuts(list);

	if (list.IsEmpty())
		{
		jdelete itsShortcuts;
		itsShortcuts = NULL;
		itsULIndex   = 0;
		}
	else
		{
		if (itsShortcuts == NULL)
			{
			itsShortcuts = jnew JString(list);
			assert( itsShortcuts != NULL );
			}
		else
			{
			*itsShortcuts = list;
			}

		CalcULIndex();
		}
}

/******************************************************************************
 CalcULIndex (private)

 ******************************************************************************/

void
JXTextButton::CalcULIndex()
{
	if (itsShortcuts != NULL)
		{
		JString s = *itsShortcuts;
		JStringIterator iter(&s);
		if (iter.Next("^M") || iter.Prev("^m"))	// indicated with black border
			{
			iter.RemoveLastMatch();
			}

		itsULIndex = JXWindow::GetULShortcutIndex(itsLabel, &s);
		}
	else
		{
		itsULIndex = 0;
		}
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
JXTextButton::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	const JRect bounds = GetBounds();

	if (IsActive())
		{
		p.SetFont(itsFont);
		}
	else
		{
		JFont f = itsFont;
		f.SetColor(GetColormap()->GetInactiveLabelColor());
		p.SetFont(f);
		}

	p.String(bounds.left, bounds.top, itsLabel, itsULIndex,
			 bounds.width(), JPainter::kHAlignCenter,
			 bounds.height(), JPainter::kVAlignCenter);
}

/******************************************************************************
 DrawBackground (virtual protected)

 ******************************************************************************/

void
JXTextButton::DrawBackground
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
	if (IsPushed())
		{
		p.SetPenColor(itsPushedColor);
		p.SetFilling(kJTrue);
		p.JPainter::Rect(frame);
		p.SetPenColor(GetColormap()->GetBlackColor());
		p.SetFilling(kJFalse);
		}
	else
		{
		JXButton::DrawBackground(p, frame);
		}
}
