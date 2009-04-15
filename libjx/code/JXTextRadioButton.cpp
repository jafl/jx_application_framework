/******************************************************************************
 JXTextRadioButton.cpp

	BASE CLASS = JXRadioButton

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXTextRadioButton.h>
#include <JXWindow.h>
#include <JXWindowPainter.h>
#include <jXPainterUtil.h>
#include <JXColormap.h>
#include <JFontManager.h>
#include <JString.h>
#include <jGlobals.h>
#include <jAssert.h>

const JCoordinate kMarginWidth   = 5;
const JCoordinate kBoxHeight     = 12;
const JCoordinate kBoxHalfHeight = 6;

/******************************************************************************
 Constructor

 ******************************************************************************/

JXTextRadioButton::JXTextRadioButton
	(
	const JIndex		id,
	const JCharacter*	label,
	JXRadioGroup*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXRadioButton(id, enclosure, hSizing, vSizing, x,y, w,h),
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

JXTextRadioButton::~JXTextRadioButton()
{
	delete itsShortcuts;
}

/******************************************************************************
 Set label

 ******************************************************************************/

void
JXTextRadioButton::SetLabel
	(
	const JCharacter* label
	)
{
	itsLabel   = label;
	itsULIndex = JXWindow::GetULShortcutIndex(itsLabel, itsShortcuts);
	Refresh();
}

void
JXTextRadioButton::SetFontName
	(
	const JCharacter* fontName
	)
{
	itsFontName = fontName;
	Refresh();
}

JFontStyle
JXTextRadioButton::GetFontStyle()
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
JXTextRadioButton::SetFontStyle
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
JXTextRadioButton::SetShortcuts
	(
	const JCharacter* list
	)
{
	JXRadioButton::SetShortcuts(list);

	#define LabelVarName	itsLabel
	#include <JXUpdateShortcutIndex.th>
	#undef LabelVarName
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
JXTextRadioButton::Activate()
{
	const JBoolean wasActive = IsActive();
	JXRadioButton::Activate();
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
JXTextRadioButton::Deactivate()
{
	const JBoolean wasActive = IsActive();
	JXRadioButton::Deactivate();
	if (wasActive && !IsActive())
		{
		itsFontStyle.color = (GetColormap())->GetInactiveLabelColor();
		Refresh();
		}
}

/******************************************************************************
 GetPreferredWidth

	Returns the minimum width required to show all the text.

 ******************************************************************************/

JCoordinate
JXTextRadioButton::GetPreferredWidth()
	const
{
	return 2*kMarginWidth + kBoxHeight +
			(GetFontManager())->
				GetStringWidth(itsFontName, itsFontSize, itsFontStyle, itsLabel);
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
JXTextRadioButton::Draw
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
		JXDrawDownDiamond(p, boxRect, kJXDefaultBorderWidth, kJTrue, itsPushedColor);
		}
	else if (isActive)
		{
		JXDrawUpDiamond(p, boxRect, kJXDefaultBorderWidth, kJTrue, itsNormalColor);
		}
	else if (drawChecked)
		{
		JXDrawFlatDiamond(p, boxRect, kJXDefaultBorderWidth,
						  (GetColormap())->GetInactiveLabelColor(), kJTrue, itsPushedColor);
		}
	else
		{
		JXDrawFlatDiamond(p, boxRect, kJXDefaultBorderWidth,
						  (GetColormap())->GetInactiveLabelColor(), kJTrue, itsNormalColor);
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
JXTextRadioButton::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
}
