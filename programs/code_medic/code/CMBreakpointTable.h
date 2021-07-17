/******************************************************************************
 CMBreakpointTable.h

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#ifndef _H_CMBreakpointTable
#define _H_CMBreakpointTable

#include <JXEditTable.h>
#include <JPrefObject.h>
#include <JFont.h>

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

	virtual bool	IsEditable(const JPoint& cell) const override;
	virtual void		HandleKeyPress(const JUtf8Character& c, const int keySym,
									   const JXKeyModifiers& modifiers) override;

protected:

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

	virtual void			TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;
	virtual JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h) override;
	virtual bool		ExtractInputData(const JPoint& cell) override;
	virtual void			PrepareDeleteXInputField() override;

	virtual void	ReadPrefs(std::istream& input) override;
	virtual void	WritePrefs(std::ostream& output) const override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	CMBreakpointsDir*			itsDir;
	JPtrArray<CMBreakpoint>*	itsBPList;
	JPoint						itsSelectedCell;	// x=col, y=bpIndex

	JXInputField*	itsTextInput;			// nullptr unless editing
	JFont			itsFont;

private:

	bool	FindBreakpointByDebuggerIndex(const CMBreakpoint* bp, JIndex* index) const;

	static JListT::CompareResult
		CompareBreakpointLocations(CMBreakpoint* const & bp1,
								   CMBreakpoint* const & bp2);

	// not allowed

	CMBreakpointTable(const CMBreakpointTable& source);
	const CMBreakpointTable& operator=(const CMBreakpointTable& source);
};

#endif
