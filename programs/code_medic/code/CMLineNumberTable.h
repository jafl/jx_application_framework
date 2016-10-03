/******************************************************************************
 CMLineNumberTable.h

	Copyright (C) 2016 by John Lindal.  All rights reserved.

 ******************************************************************************/

#ifndef _H_CMLineNumberTable
#define _H_CMLineNumberTable

#include "CMLineIndexTable.h"

class CMLineNumberTable : public CMLineIndexTable
{
public:

	CMLineNumberTable(CMSourceDirector* dir, CMSourceText* text,
					  JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	virtual ~CMLineNumberTable();

protected:

	virtual JColorIndex	GetCurrentLineMarkerColor() const;
	virtual JString		GetLineText(const JIndex lineIndex) const;
	virtual JString		GetLongestLineText(const JIndex lineCount) const;
	virtual JIndex		GetBreakpointLineIndex(const JIndex bpIndex, const CMBreakpoint* bp) const;
	virtual JBoolean	GetFirstBreakpointOnLine(const JIndex lineIndex, JIndex* bpIndex) const;
	virtual JBoolean	BreakpointsOnSameLine(const CMBreakpoint* bp1, const CMBreakpoint* bp2) const;
	virtual void		GetBreakpoints(JPtrArray<CMBreakpoint>* list);
	virtual void		SetBreakpoint(const JIndex lineIndex, const JBoolean temporary);
	virtual void		RemoveAllBreakpointsOnLine(const JIndex lineIndex);
	virtual void		RunUntil(const JIndex lineIndex);
	virtual void		SetExecutionPoint(const JIndex lineIndex);

private:

	static JOrderedSetT::CompareResult
		CompareBreakpointLines(CMBreakpoint* const & bp1, CMBreakpoint* const & bp2);

	// not allowed

	CMLineNumberTable(const CMLineNumberTable& source);
	const CMLineNumberTable& operator=(const CMLineNumberTable& source);
};

#endif
