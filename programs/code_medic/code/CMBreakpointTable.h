/******************************************************************************
 CMBreakpointTable.h

	Copyright © 2010 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CMBreakpointTable
#define _H_CMBreakpointTable

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXEditTable.h>
#include <JPrefObject.h>

class JXTextMenu;
class JXInputField;
class JXColHeaderWidget;
class CMBreakpoint;
class CMBreakpointsDir;

class CMBreakpointTable : public JXEditTable, public JPrefObject
{
public:

	CMBreakpointTable(CMBreakpointsDir* dir,
					  JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	virtual ~CMBreakpointTable();

	void	SetColTitles(JXColHeaderWidget* widget) const;
	void	Update();
	void	Show(const CMBreakpoint* bp);
	void	EditIgnoreCount(const CMBreakpoint* bp);
	void	EditCondition(const CMBreakpoint* bp);

	virtual JBoolean	IsEditable(const JPoint& cell) const;
	virtual void		HandleKeyPress(const int key, const JXKeyModifiers& modifiers);

protected:

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);

	virtual void			TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);
	virtual JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h);
	virtual JBoolean		ExtractInputData(const JPoint& cell);
	virtual void			PrepareDeleteXInputField();

	virtual void	ReadPrefs(istream& input);
	virtual void	WritePrefs(ostream& output) const;

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	CMBreakpointsDir*			itsDir;
	JPtrArray<CMBreakpoint>*	itsBPList;
	JPoint						itsSelectedCell;	// x=col, y=bpIndex

	JXInputField*	itsTextInput;			// NULL unless editing
	JString			itsFontName;
	JSize			itsFontSize;

private:

	JBoolean	FindBreakpointByDebuggerIndex(const CMBreakpoint* bp, JIndex* index) const;

	static JOrderedSetT::CompareResult
		CompareBreakpointLocations(CMBreakpoint* const & bp1,
								   CMBreakpoint* const & bp2);

	// not allowed

	CMBreakpointTable(const CMBreakpointTable& source);
	const CMBreakpointTable& operator=(const CMBreakpointTable& source);
};

#endif
