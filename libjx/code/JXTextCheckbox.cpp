/******************************************************************************
 JXTextCheckbox.cpp

	BASE CLASS = JXCheckbox

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXTextCheckbox.h"
#include "JXWindow.h"
#include "JXWindowPainter.h"
#include "JXFontManager.h"
#include "JXColorManager.h"
#include "jXPainterUtil.h"
#include "jXGlobals.h"
#include <jAssert.h>

const JCoordinate kMarginWidth   = 5;
const JCoordinate kBoxHeight     = 10;
const JCoordinate kBoxHalfHeight = 5;

/******************************************************************************
 Constructor

 ******************************************************************************/

JXTextCheckbox::JXTextCheckbox
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
	JXCheckbox(enclosure, hSizing, vSizing, x,y, w,h),
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

JXTextCheckbox::~JXTextCheckbox()
{
	jdelete itsShortcuts;
}

/******************************************************************************
 ToString (virtual)

 ******************************************************************************/

JString
JXTextCheckbox::ToString()
	const
{
	return JXCheckbox::ToString() + ": " + itsLabel;
}

/******************************************************************************
 Set label

 ******************************************************************************/

void
JXTextCheckbox::SetLabel
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
JXTextCheckbox::SetShortcuts
	(
	const JString& list
	)
{
	JXCheckbox::SetShortcuts(list);

	#define LabelVarName	itsLabel
	#include "JXUpdateShortcutIndex.th"
	#undef LabelVarName
}

/******************************************************************************
 GetPreferredWidth

	Returns the minimum width required to show all the text.

 ******************************************************************************/

JCoordinate
JXTextCheckbox::GetPreferredWidth()
	const
{
	return 2*kMarginWidth + kBoxHeight + itsFont.GetStringWidth(GetFontManager(), itsLabel);
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
		p.SetPenColor(JColorManager::GetInactiveLabelColor());
		p.RectInside(boxRect);
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
JXTextCheckbox::DrawBorder
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
JXTextCheckbox::GetFTCMinContentSize
	(
	const JBoolean horizontal
	)
	const
{
	return (horizontal ?
			JMax((JSize) GetApertureWidth(), 2*kMarginWidth + kBoxHeight + itsFont.GetStringWidth(GetFontManager(), itsLabel)) :
			JMax(GetApertureHeight(), kBoxHeight, JRound(itsFont.GetLineHeight(GetFontManager()) * 1.25)));
}
