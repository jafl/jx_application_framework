/******************************************************************************
 CMLineNumberTable.h

	Copyright © 2001 by John Lindal.  All rights reserved.

 ******************************************************************************/

#ifndef _H_CMLineNumberTable
#define _H_CMLineNumberTable

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXTable.h>

class JColormap;
class JXTextMenu;
class JXScrollbar;
class CMLink;
class CMSourceDirector;
class CMSourceText;
class CMBreakpoint;
class CMDeselectLineNumberTask;

class CMLineNumberTable : public JXTable
{
	friend class CMDeselectLineNumberTask;

public:

	CMLineNumberTable(CMSourceDirector* dir, CMSourceText* text,
				JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	virtual ~CMLineNumberTable();

	void		ClearLineNumbers();
	void		SetLineNumbers(JPtrArray<JString>*	list);
	JBoolean	FindAddressLineNumber(const JString& addr, JIndex* index) const;

	JCoordinate	GetCurrentLine() const;
	void		SetCurrentLine(const JIndex line);

	static JColorIndex	GetBreakpointColor(const CMBreakpoint* bp, JColormap* cmap);
	static void			DrawBreakpoint(const CMBreakpoint* bp, JPainter& p,
									   JColormap* cmap, const JRect& rect);

protected:

	virtual void	TablePrepareToDrawCol(const JIndex colIndex,
										  const JIndex firstRow, const JIndex lastRow);
	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);

	virtual void	Receive(JBroadcaster* sender, const Message& message);
	virtual void	ReceiveGoingAway(JBroadcaster* sender);

private:

	CMLink*				itsLink;
	CMSourceDirector*	itsDirector;
	CMSourceText*		itsText;
	JXScrollbar*		itsVScrollbar;
	JCoordinate			itsCurrentVisualIndex;

	JPtrArray<CMBreakpoint>*	itsBPList;				// contents owned by CMBreakpointManager
	JArray<JIndex>*				itsVisualBPIndexList;	// visual line of each breakpoint
	JIndex						itsBPDrawIndex;			// index of next bp to draw; may be invalid

	JPtrArray<JString>*			itsLineNumberList;		// can be NULL

	JXTextMenu*					itsLineMenu;
	JIndex						itsLineMenuLineIndex;
	JBoolean					itsIsFullLineMenuFlag;
	JIndexRange					itsLineMenuRange;
	CMDeselectLineNumberTask*	itsDeselectTask;

private:

	void	AdjustToText();
	void	UpdateBreakpoints();

	void		DrawBreakpoints(JPainter& p, const JPoint& cell, const JRect& rect);
	JBoolean	FindNextBreakpoint(const JIndex rowIndex, JBoolean* multiple = NULL);
	JBoolean	HasMultipleBreakpointsOnLine(const JIndex firstBPIndex) const;

	void	AdjustBreakpoints(const JIndex visualIndex, const JPoint& pt,
							  const JXButtonStates& buttonStates,
							  const JXKeyModifiers& modifiers);
	void	OpenLineMenu(const JIndex visualIndex, const JPoint& pt,
						 const JXButtonStates& buttonStates,
						 const JXKeyModifiers& modifiers,
						 const JBoolean onlyBreakpoints,
						 const JIndex firstBPIndex = 0);
	void	UpdateLineMenu();
	void	HandleLineMenu(const JIndex index);

	static JString	GetAddressForBreakpoint(const JString& addr);
	JString			GetLinePrefixFromAddress(const JString& addr) const;

	static JOrderedSetT::CompareResult
		CompareBreakpointLines(CMBreakpoint* const & bp1, CMBreakpoint* const & bp2);
	static JOrderedSetT::CompareResult
		CompareBreakpointAddresses(CMBreakpoint* const & bp1, CMBreakpoint* const & bp2);

	// not allowed

	CMLineNumberTable(const CMLineNumberTable& source);
	const CMLineNumberTable& operator=(const CMLineNumberTable& source);
};


/******************************************************************************
 GetCurrentLine

 ******************************************************************************/

inline JCoordinate
CMLineNumberTable::GetCurrentLine()
	const
{
	return itsCurrentVisualIndex;
}

#endif
