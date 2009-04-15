/******************************************************************************
 JXTextCheckbox.cpp

	BASE CLASS = JXCheckbox

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXTextCheckbox.h>
#include <JXWindow.h>
#include <JXWindowPainter.h>
#include <JXColormap.h>
#include <jXPainterUtil.h>
#include <jXGlobals.h>
#include <JString.h>
#include <jAssert.h>

const JCoordinate kMarginWidth   = 5;
const JCoordinate kBoxHeight     = 10;
const JCoordinate kBoxHalfHeight = 5;

/******************************************************************************
 Constructor

 ******************************************************************************/

JXTextCheckbox::JXTextCheckbox
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
	JXCheckbox(enclosure, hSizing, vSizing, x,y, w,h),
	itsLabel(label),
	itsFontName(JGetDefaultFontName())
{
	itsShortcuts = NULL;
	itsULIndex   = 0;

	itsFontSize = kJXDefaultFontSize;
	// itsFontStyle already initialized to correct default

	JXColormap* colormap = GetColormap();
	itsNormalColor = colormap->GetDefaultBackColor();
	itsPushedColor = colormap->GetDefaultSelButtonColor();

	itsTrueLabelColor = itsFontStyle.color;

	if (!IsActive())
		{
		itsFontStyle.color = colormap->GetInactiveLabelColor();
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXTextCheckbox::~JXTextCheckbox()
{
	delete itsShortcuts;
}

/******************************************************************************
 Set label

 ******************************************************************************/

void
JXTextCheckbox::SetLabel
	(
	const JCharacter* label
	)
{
	itsLabel   = label;
	itsULIndex = JXWindow::GetULShortcutIndex(itsLabel, itsShortcuts);
	Refresh();
}

void
JXTextCheckbox::SetFontName
	(
	const JCharacter* fontName
	)
{
	itsFontName = fontName;
	Refresh();
}

JFontStyle
JXTextCheckbox::GetFontStyle()
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
JXTextCheckbox::SetFontStyle
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
JXTextCheckbox::SetShortcuts
	(
	const JCharacter* list
	)
{
	JXCheckbox::SetShortcuts(list);

	#define LabelVarName	itsLabel
	#include <JXUpdateShortcutIndex.th>
	#undef LabelVarName
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
JXTextCheckbox::Activate()
{
	const JBoolean wasActive = IsActive();
	JXCheckbox::Activate();
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
JXTextCheckbox::Deactivate()
{
	const JBoolean wasActive = IsActive();
	JXCheckbox::Deactivate();
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
JXTextCheckbox::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	const JRect bounds  = GetBounds();
	const JCoordinate y = bounds.ycenter();

	// draw button

	const JRect boxRect(y - kBoxHalfHeight, kMarginWidth,
						y + kBoxHalfHeight, kMarginWidth + kBoxHeight);
	const JBoolean drawChecked = DrawChecked();
	const JBoolean isActive    = IsActive();
	if (drawChecked && isActive)
		{
		JXDrawDownFrame(p, boxRect, kJXDefaultBorderWidth, kJTrue, itsPushedColor);
		}
	else if (isActive)
		{
		JXDrawUpFrame(p, boxRect, kJXDefaultBorderWidth, kJTrue, itsNormalColor);
		}
	else
		{
		p.SetFilling(kJTrue);
		if (drawChecked)
			{
			p.SetPenColor(itsPushedColor);
			}
		else
			{
			p.SetPenColor(itsNormalColor);
			}
		p.JPainter::Rect(boxRect);
		p.SetFilling(kJFalse);

		p.SetLineWidth(kJXDefaultBorderWidth);
		p.SetPenColor((GetColormap())->GetInactiveLabelColor());
		p.RectInside(boxRect);
		}

	// draw text

	JRect textRect  = bounds;
	textRect.left  += 2*kMarginWidth + kBoxHeight;
	p.SetFont(itsFontName, itsFontSize, itsFontStyle);
	p.String(textRect.left, textRect.top, itsLabel, itsULIndex,
			 textRect.width(), JPainter::kHAlignLeft,
			 textRect.height(), JPainter::kVAlignCenter);
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
JXTextCheckbox::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
}
