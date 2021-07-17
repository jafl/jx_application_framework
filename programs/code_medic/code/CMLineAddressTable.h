/******************************************************************************
 CMLineAddressTable.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_CMLineAddressTable
#define _H_CMLineAddressTable

#include "CMLineIndexTable.h"

class CMLineAddressTable : public CMLineIndexTable
{
public:

	CMLineAddressTable(CMSourceDirector* dir, CMSourceText* text,
					   JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					   const HSizingOption hSizing, const VSizingOption vSizing,
					   const JCoordinate x, const JCoordinate y,
					   const JCoordinate w, const JCoordinate h);

	virtual ~CMLineAddressTable();

	void		SetLineNumbers(JPtrArray<JString>* list);
	void		ClearLineNumbers();
	bool	FindAddressLineNumber(const JString& address, JIndex* index) const;

protected:

	virtual JColorID	GetCurrentLineMarkerColor() const;
	virtual JString		GetLineText(const JIndex lineIndex) const;
	virtual JString		GetLongestLineText(const JIndex lineCount) const;
	virtual JIndex		GetBreakpointLineIndex(const JIndex bpIndex, const CMBreakpoint* bp) const;
	virtual bool	GetFirstBreakpointOnLine(const JIndex lineIndex, JIndex* bpIndex) const;
	virtual bool	BreakpointsOnSameLine(const CMBreakpoint* bp1, const CMBreakpoint* bp2) const;
	virtual void		GetBreakpoints(JPtrArray<CMBreakpoint>* list);
	virtual void		SetBreakpoint(const JIndex lineIndex, const bool temporary);
	virtual void		RemoveAllBreakpointsOnLine(const JIndex lineIndex);
	virtual void		RunUntil(const JIndex lineIndex);
	virtual void		SetExecutionPoint(const JIndex lineIndex);

private:

	JArray<JIndex>*		itsVisualBPIndexList;	// visual line of each breakpoint
	JPtrArray<JString>*	itsLineTextList;		// text for each line

private:

	static JString	BuildAddress(const JString& addr);
	JString			GetLineTextFromAddress(const JString& addr) const;

	static JListT::CompareResult
		CompareBreakpointAddresses(CMBreakpoint* const & bp1, CMBreakpoint* const & bp2);

	// not allowed

	CMLineAddressTable(const CMLineAddressTable& source);
	const CMLineAddressTable& operator=(const CMLineAddressTable& source);
};

#endif
