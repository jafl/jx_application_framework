/******************************************************************************
 SizeHistogram.h

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#ifndef _H_SizeHistogram
#define _H_SizeHistogram

#include <jx-af/jx/JXScrollableWidget.h>
#include <jx-af/jcore/JMemoryManager.h>

class SizeHistogram : public JXScrollableWidget
{
public:

	SizeHistogram(JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	~SizeHistogram() override;

	void	SetValues(const JSize total,
					  const JSize histo[ JMemoryManager::kHistogramSlotCount ]);

protected:

	void	Draw(JXWindowPainter& p, const JRect& rect) override;
	void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;

private:

	JSize itsTotal;
	JSize itsHisto[ JMemoryManager::kHistogramSlotCount ];
};

#endif
