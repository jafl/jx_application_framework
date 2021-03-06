/******************************************************************************
 CMLineIndexTable.h

	Copyright (C) 2001-16 by John Lindal.  All rights reserved.

 ******************************************************************************/

#ifndef _H_CMLineIndexTable
#define _H_CMLineIndexTable

#include <JXTable.h>

class JColormap;
class JXTextMenu;
class JXScrollbar;
class CMLink;
class CMSourceDirector;
class CMSourceText;
class CMBreakpoint;
class CMDeselectLineTask;

class CMLineIndexTable : public JXTable
{
	friend class CMAdjustLineTableToTextTask;
	friend class CMDeselectLineTask;

public:

	CMLineIndexTable(JOrderedSetT::CompareResult (*bpCcompareFn)(CMBreakpoint *const &, CMBreakpoint *const &),
					 CMSourceDirector* dir, CMSourceText* text,
					 JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					 const HSizingOption hSizing, const VSizingOption vSizing,
					 const JCoordinate x, const JCoordinate y,
					 const JCoordinate w, const JCoordinate h);

	virtual ~CMLineIndexTable();

	JCoordinate	GetCurrentLine() const;
	void		SetCurrentLine(const JIndex line);

	static void	DrawBreakpoint(const CMBreakpoint* bp, JPainter& p,
							   JColormap* cmap, const JRect& rect);

protected:

	CMLink*						GetLink() const;
	CMSourceDirector*			GetDirector() const;
	JPtrArray<CMBreakpoint>*	GetBreakpointList() const;

	virtual void	TablePrepareToDrawCol(const JIndex colIndex,
										  const JIndex firstRow, const JIndex lastRow);
	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);

	virtual void	Receive(JBroadcaster* sender, const Message& message);
	virtual void	ReceiveGoingAway(JBroadcaster* sender);

	virtual JColorIndex	GetCurrentLineMarkerColor() const = 0;
	virtual JString		GetLineText(const JIndex lineIndex) const = 0;
	virtual JString		GetLongestLineText(const JIndex lineCount) const = 0;
	virtual JIndex		GetBreakpointLineIndex(const JIndex bpIndex, const CMBreakpoint* bp) const = 0;
	virtual JBoolean	GetFirstBreakpointOnLine(const JIndex lineIndex, JIndex* bpIndex) const = 0;
	virtual JBoolean	BreakpointsOnSameLine(const CMBreakpoint* bp1, const CMBreakpoint* bp2) const = 0;
	virtual void		GetBreakpoints(JPtrArray<CMBreakpoint>* list) = 0;
	virtual void		SetBreakpoint(const JIndex lineIndex, const JBoolean temporary) = 0;
	virtual void		RemoveAllBreakpointsOnLine(const JIndex lineIndex) = 0;
	virtual void		RunUntil(const JIndex lineIndex) = 0;
	virtual void		SetExecutionPoint(const JIndex lineIndex) = 0;

private:

	CMLink*				itsLink;
	CMSourceDirector*	itsDirector;
	CMSourceText*		itsText;
	JXScrollbar*		itsVScrollbar;
	JCoordinate			itsCurrentLineIndex;

	JPtrArray<CMBreakpoint>*	itsBPList;			// contents owned by CMBreakpointManager
	JIndex						itsBPDrawIndex;		// index of next bp to draw; may be invalid

	JXTextMenu*			itsLineMenu;
	JIndex				itsLineMenuLineIndex;
	JBoolean			itsIsFullLineMenuFlag;
	JIndexRange			itsLineMenuBPRange;
	CMDeselectLineTask*	itsDeselectTask;

private:

	void	AdjustToText();
	void	UpdateBreakpoints();

	void		DrawBreakpoints(JPainter& p, const JPoint& cell, const JRect& rect);
	JBoolean	FindNextBreakpoint(const JIndex rowIndex, JBoolean* multiple = NULL);
	JBoolean	HasMultipleBreakpointsOnLine(const JIndex bpIndex) const;

	void	AdjustBreakpoints(const JIndex lineIndex, const JPoint& pt,
							  const JXButtonStates& buttonStates,
							  const JXKeyModifiers& modifiers);
	void	OpenLineMenu(const JIndex lineIndex, const JPoint& pt,
						 const JXButtonStates& buttonStates,
						 const JXKeyModifiers& modifiers,
						 const JBoolean onlyBreakpoints,
						 const JIndex firstBPIndex = 0);
	void	UpdateLineMenu();
	void	HandleLineMenu(const JIndex index);

	// not allowed

	CMLineIndexTable(const CMLineIndexTable& source);
	const CMLineIndexTable& operator=(const CMLineIndexTable& source);
};


/******************************************************************************
 GetLink (protected)

 ******************************************************************************/

inline CMLink*
CMLineIndexTable::GetLink()
	const
{
	return itsLink;
}

/******************************************************************************
 GetDirector (protected)

 ******************************************************************************/

inline CMSourceDirector*
CMLineIndexTable::GetDirector()
	const
{
	return itsDirector;
}

/******************************************************************************
 GetBreakpointList (protected)

 ******************************************************************************/

inline JPtrArray<CMBreakpoint>*
CMLineIndexTable::GetBreakpointList()
	const
{
	return itsBPList;
}

/******************************************************************************
 GetCurrentLine

 ******************************************************************************/

inline JCoordinate
CMLineIndexTable::GetCurrentLine()
	const
{
	return itsCurrentLineIndex;
}

#endif
