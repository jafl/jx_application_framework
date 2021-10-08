/******************************************************************************
 JX2DCursorMarkTable.h

	Interface for the JX2DCursorMarkTable class

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_JX2DCursorMarkTable
#define _H_JX2DCursorMarkTable

#include <jx-af/jx/JXTable.h>

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

	~JX2DCursorMarkTable();

	bool	Print(JPagePrinter&	p, const bool putOnSamePage,
				  const JRect& partialPageRect);
	void	TableDrawCells(JPainter& p, const JRect& cellRect,
							const JRect& regionRect);
	void	AdjustTable();

protected:

	void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;
	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	J2DPlotWidget*	itsPlot;
	bool			itsPrintingFlag;

private:

	bool	DrawRegions(JPainter& p, const JRect& pageRect,
						const JCoordinate regionWidth,
						JIndex* currentRow);
};


#endif
