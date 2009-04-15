/******************************************************************************
 JXTextButton.cpp

	Maintains a pushable button with a text label.

	BASE CLASS = JXButton

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXTextButton.h>
#include <JXWindow.h>
#include <JXWindowPainter.h>
#include <JXColormap.h>
#include <jXGlobals.h>
#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXTextButton::JXTextButton
	(
	const JCharacter*	label,
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
	itsFontName(JGetDefaultFontName())
{
	itsShortcuts = NULL;
	itsULIndex   = 0;

	itsFontSize = kJXDefaultFontSize;
	// itsFontStyle already initialized to correct default

	itsPushedColor    = (GetColormap())->GetDefaultBackColor();
	itsTrueLabelColor = itsFontStyle.color;

	if (!IsActive())
		{
		itsFontStyle.color = (GetColormap())->GetInactiveLabelColor();
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXTextButton::~JXTextButton()
{
	delete itsShortcuts;
}

/******************************************************************************
 Set label

 ******************************************************************************/

void
JXTextButton::SetLabel
	(
	const JCharacter* label
	)
{
	itsLabel = label;
	CalcULIndex();
	Refresh();
}

void
JXTextButton::SetFontName
	(
	const JCharacter* fontName
	)
{
	itsFontName = fontName;
	Refresh();
}

JFontStyle
JXTextButton::GetFontStyle()
	const
{
	JFontStyle style = itsFontStyle;
	if (!IsActive())
		{
		style.color = itsTrueLabelColor;
		}
	return style;
}

void
JXTextButton::SetFontStyle
	(
	const JFontStyle& style
	)
{
	itsFontStyle      = style;
	itsTrueLabelColor = itsFontStyle.color;
	if (!IsActive())
		{
		itsFontStyle.color = (GetColormap())->GetInactiveLabelColor();
		}
	Refresh();
}

/******************************************************************************
 SetShortcuts (virtual)

 ******************************************************************************/

void
JXTextButton::SetShortcuts
	(
	const JCharacter* list
	)
{
	JXButton::SetShortcuts(list);

	if (JStringEmpty(list))
		{
		delete itsShortcuts;
		itsShortcuts = NULL;
		itsULIndex   = 0;
		}
	else
		{
		if (itsShortcuts == NULL)
			{
			itsShortcuts = new JString(list);
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
		if (s.BeginsWith("^M") || s.BeginsWith("^m"))	// indicated with black border
			{
			s.RemoveSubstring(1,2);
			}

		itsULIndex = JXWindow::GetULShortcutIndex(itsLabel, &s);
		}
	else
		{
		itsULIndex = 0;
		}
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
JXTextButton::Activate()
{
	const JBoolean wasActive = IsActive();
	JXButton::Activate();
	if (!wasActive && IsActive())
		{
		itsFontStyle.color = itsTrueLabelColor;
		Refresh();
		}
}

/******************************************************************************
 Deactivate (virtual)

 ******************************************************************************/

void
JXTextButton::Deactivate()
{
	const JBoolean wasActive = IsActive();
	JXButton::Deactivate();
	if (wasActive && !IsActive())
		{
		itsFontStyle.color = (GetColormap())->GetInactiveLabelColor();
		Refresh();
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
	p.SetFont(itsFontName, itsFontSize, itsFontStyle);
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
		p.SetPenColor((GetColormap())->GetBlackColor());
		p.SetFilling(kJFalse);
		}
	else
		{
		JXButton::DrawBackground(p, frame);
		}
}
