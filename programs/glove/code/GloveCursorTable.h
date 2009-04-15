/******************************************************************************
 GloveCursorTable.h

	Interface for the GloveCursorTable class

	Copyright © 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_GloveCursorTable
#define _H_GloveCursorTable

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXTable.h>

class J2DPlotWidget;
class GloveHistoryDir;

class GloveCursorTable : public JXTable
{
public:

	GloveCursorTable(GloveHistoryDir* session, J2DPlotWidget* plot,
					JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	virtual ~GloveCursorTable();
	void	SendToSession();

protected:

	virtual void			TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);
	virtual void			Receive(JBroadcaster* sender,
									const JBroadcaster::Message& message);

private:

	J2DPlotWidget*		itsPlot;
	JBoolean			itsShowX;
	JBoolean			itsShowY;
	JBoolean			itsDual;
	JFloat				itsX1;
	JFloat				itsX2;
	JFloat				itsY1;
	JFloat				itsY2;
	GloveHistoryDir*	itsSession;

private:

	void			AdjustTable();

	// not allowed

	GloveCursorTable(const GloveCursorTable& source);
	const GloveCursorTable& operator=(const GloveCursorTable& source);
};


#endif
