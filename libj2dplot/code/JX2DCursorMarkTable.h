/******************************************************************************
 JX2DCursorMarkTable.h

	Interface for the JX2DCursorMarkTable class

	Copyright © 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_JX2DCursorMarkTable
#define _H_JX2DCursorMarkTable

#include <JXTable.h>

class J2DPlotWidget;
class JPagePrinter;

class JX2DCursorMarkTable : public JXTable
{
public:

	JX2DCursorMarkTable(J2DPlotWidget* plot,
						JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
						const HSizingOption hSizing, const VSizingOption vSizing,
						const JCoordinate x, const JCoordinate y,
						const JCoordinate w, const JCoordinate h);

	virtual ~JX2DCursorMarkTable();

	JBoolean	Print(JPagePrinter&	p, const JBoolean putOnSamePage,
					  const JRect& partialPageRect);
	void		TableDrawCells(JPainter& p, const JRect& cellRect,
								const JRect& regionRect);
	void		AdjustTable();

protected:

	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);
	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	J2DPlotWidget*	itsPlot;
	JBoolean		itsPrintingFlag;

private:

	JBoolean	DrawRegions(JPainter& p, const JRect& pageRect,
							const JCoordinate regionWidth,
							JIndex* currentRow);

	// not allowed

	JX2DCursorMarkTable(const JX2DCursorMarkTable& source);
	const JX2DCursorMarkTable& operator=(const JX2DCursorMarkTable& source);
};


#endif
