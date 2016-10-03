/******************************************************************************
 JXTextCheckbox.cpp

	BASE CLASS = JXCheckbox

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXTextCheckbox.h>
#include <JXWindow.h>
#include <JXWindowPainter.h>
#include <JXFontManager.h>
#include <JXColormap.h>
#include <jXPainterUtil.h>
#include <jXGlobals.h>
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
	itsFont(GetFontManager()->GetDefaultFont())
{
	itsShortcuts = NULL;
	itsULIndex   = 0;

	JXColormap* colormap = GetColormap();
	itsNormalColor = colormap->GetDefaultBackColor();
	itsPushedColor = colormap->GetDefaultSelButtonColor();
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
		p.SetPenColor(GetColormap()->GetInactiveLabelColor());
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
		f.SetColor(GetColormap()->GetInactiveLabelColor());
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
