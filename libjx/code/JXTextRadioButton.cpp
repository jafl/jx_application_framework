/******************************************************************************
 JXTextRadioButton.cpp

	BASE CLASS = JXRadioButton

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "jx-af/jx/JXTextRadioButton.h"
#include "jx-af/jx/JXWindow.h"
#include "jx-af/jx/JXWindowPainter.h"
#include "jx-af/jx/jXPainterUtil.h"
#include "jx-af/jx/JXFontManager.h"
#include "jx-af/jx/JXColorManager.h"
#include "jx-af/jx/jXGlobals.h"
#include <jx-af/jcore/jAssert.h>

const JCoordinate kMarginWidth   = 5;
const JCoordinate kBoxHeight     = 12;
const JCoordinate kBoxHalfHeight = 6;

/******************************************************************************
 Constructor

 ******************************************************************************/

JXTextRadioButton::JXTextRadioButton
	(
	const JIndex		id,
	const JString&		label,
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
	itsFont(JFontManager::GetDefaultFont())
{
	itsShortcuts = nullptr;
	itsULIndex   = 0;

	itsNormalColor = JColorManager::GetDefaultBackColor();
	itsPushedColor = JColorManager::GetDefaultSelButtonColor();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXTextRadioButton::~JXTextRadioButton()
{
	jdelete itsShortcuts;
}

/******************************************************************************
 ToString (virtual)

 ******************************************************************************/

JString
JXTextRadioButton::ToString()
	const
{
	return JXRadioButton::ToString() + ": " + itsLabel;
}

/******************************************************************************
 Set label

 ******************************************************************************/

void
JXTextRadioButton::SetLabel
	(
	const JString& label
	)
{
	itsLabel   = label;
	itsULIndex = JXWindow::GetULShortcutIndex(itsLabel, itsShortcuts);
	Refresh();
}

/******************************************************************************
 SetShortcuts (virtual)

 ******************************************************************************/

void
JXTextRadioButton::SetShortcuts
	(
	const JString& list
	)
{
	JXRadioButton::SetShortcuts(list);

	#define LabelVarName	itsLabel
	#include "jx-af/jx/JXUpdateShortcutIndex.th"
	#undef LabelVarName
}

/******************************************************************************
 GetPreferredWidth

	Returns the minimum width required to show all the text.

 ******************************************************************************/

JCoordinate
JXTextRadioButton::GetPreferredWidth()
	const
{
	return 2*kMarginWidth + kBoxHeight + itsFont.GetStringWidth(GetFontManager(), itsLabel);
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
	const bool drawChecked = DrawChecked();
	const bool isActive    = IsActive();
	if (drawChecked && isActive)
	{
		JXDrawDownDiamond(p, boxRect, kJXDefaultBorderWidth, true, itsPushedColor);
	}
	else if (isActive)
	{
		JXDrawUpDiamond(p, boxRect, kJXDefaultBorderWidth, true, itsNormalColor);
	}
	else if (drawChecked)
	{
		JXDrawFlatDiamond(p, boxRect, kJXDefaultBorderWidth,
						  JColorManager::GetInactiveLabelColor(), true, itsPushedColor);
	}
	else
	{
		JXDrawFlatDiamond(p, boxRect, kJXDefaultBorderWidth,
						  JColorManager::GetInactiveLabelColor(), true, itsNormalColor);
	}

	// draw text

	if (IsActive())
	{
		p.SetFont(itsFont);
	}
	else
	{
		JFont f = itsFont;
		f.SetColor(JColorManager::GetInactiveLabelColor());
		p.SetFont(f);
	}

	JRect textRect  = bounds;
	textRect.left  += 2*kMarginWidth + kBoxHeight;
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

/******************************************************************************
 GetFTCMinContentSize (virtual protected)

 ******************************************************************************/

JCoordinate
JXTextRadioButton::GetFTCMinContentSize
	(
	const bool horizontal
	)
	const
{
	return (horizontal ?
			JMax(GetApertureWidth(), GetPreferredWidth()) :
			JMax(GetApertureHeight(), kBoxHeight,
				 JRound(itsFont.GetLineHeight(GetFontManager()) * 1.25)));
}
