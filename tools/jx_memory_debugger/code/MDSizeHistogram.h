/******************************************************************************
 MDSizeHistogram.h

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#ifndef _H_MDSizeHistogram
#define _H_MDSizeHistogram

#include <jx-af/jx/JXScrollableWidget.h>
#include <jx-af/jcore/JMemoryManager.h>

class MDSizeHistogram : public JXScrollableWidget
{
public:

	MDSizeHistogram(JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	virtual ~MDSizeHistogram();

	void	SetValues(const JSize total,
					  const JSize histo[ JMemoryManager::kHistogramSlotCount ]);

protected:

	virtual void	Draw(JXWindowPainter& p, const JRect& rect) override;
	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;

private:

	JSize itsTotal;
	JSize itsHisto[ JMemoryManager::kHistogramSlotCount ];
};

#endif
