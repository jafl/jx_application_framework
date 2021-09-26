/******************************************************************************
 MDSizeHistogram.cpp

	Displays a histogram of allocated block sizes.

	BASE CLASS = JXScrollableWidget

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#include "MDSizeHistogram.h"
#include <jx-af/jx/JXWindowPainter.h>
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

const JCoordinate kMarginWidth = 5;

/******************************************************************************
 Constructor

 ******************************************************************************/

MDSizeHistogram::MDSizeHistogram
	(
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXScrollableWidget(scrollbarSet, enclosure, hSizing, vSizing, x,y, w,h),
	itsTotal(0)
{
	bzero(itsHisto, sizeof(itsHisto));
}

/******************************************************************************
 Destructor

 ******************************************************************************/

MDSizeHistogram::~MDSizeHistogram()
{
}

/******************************************************************************
 SetValues

 ******************************************************************************/

void
MDSizeHistogram::SetValues
	(
	const JSize total,
	const JSize histo[ JMemoryManager::kHistogramSlotCount ]
	)
{
	itsTotal = total;
	memcpy(itsHisto, histo, sizeof(itsHisto));
	Refresh();
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
MDSizeHistogram::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	const JSize lineHeight = p.GetLineHeight();
	SetBounds(GetBoundsWidth(),
			  (JMemoryManager::kHistogramSlotCount + 1) * lineHeight + 2*kMarginWidth);

	const JSize numberColWidth = p.GetStringWidth(JGetString("Size::MDSizeHistogram"));

	JRect r  = GetBounds();
	r.Shrink(kMarginWidth, kMarginWidth);
	r.bottom = r.top + lineHeight;

	// column headers

	p.SetFontStyle(JFontStyle(false, false, 1, false));

	JRect r1 = r;
	r1.right = r1.left + numberColWidth;
	p.JPainter::String(r1, JGetString("Size::MDSizeHistogram"));

	r1.Shift(numberColWidth + 2*kMarginWidth, 0);
	p.JPainter::String(r1, JGetString("Count::MDSizeHistogram"));

	r.Shift(0, lineHeight);

	// histogram

	p.SetFontStyle(JFontStyle());
	p.SetFilling(true);
	p.SetPenColor(JColorManager::GetGrayColor(50));

	JIndex exp   = 32 - (JMemoryManager::kHistogramSlotCount - 1);
	JFloat total = itsTotal;
	for (JUnsignedOffset i=0; i<JMemoryManager::kHistogramSlotCount; i++)
	{
		r1 = r;

		JRect r2         = r1;
		r2.right         = r2.left + numberColWidth - kMarginWidth;
		const JString s1 = JPrintFileSize(pow(2, exp));
		p.JPainter::String(r2, s1, JPainter::kHAlignRight);

		r1.left += numberColWidth;

		r2.Shift(numberColWidth, 0);
		const JString s2((JUInt64) itsHisto[i]);
		p.JPainter::String(r2, s2, JPainter::kHAlignRight);

		if (total > 0 && itsHisto[i] > 0)
		{
			r1.left += numberColWidth;

			r2 = r1;
			r2.Shrink(0, 2);
			r2.right = r2.left + JRound(r2.width() * (itsHisto[i] / total)) + 1;
			p.JPainter::Rect(r2);
		}

		r.Shift(0, lineHeight);
		exp++;
	}
}

/******************************************************************************
 ApertureResized (virtual protected)

 ******************************************************************************/

void
MDSizeHistogram::ApertureResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXScrollableWidget::ApertureResized(dw,dh);
	AdjustBounds(GetApertureWidth() - GetBoundsWidth(), 0);
}
