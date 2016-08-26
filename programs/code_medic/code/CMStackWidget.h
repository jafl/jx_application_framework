/******************************************************************************
 CMStackWidget.h

	Copyright © 2001 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_CMStackWidget
#define _H_CMStackWidget

#include <JXNamedTreeListWidget.h>

class JTree;
class CMLink;
class CMCommandDirector;
class CMStackDir;
class CMGetStack;
class CMGetFrame;
class CMStackFrameNode;

class CMStackWidget : public JXNamedTreeListWidget
{
public:

	CMStackWidget(CMCommandDirector* commandDir,
				  CMStackDir* stackDir, JTree* tree, JNamedTreeList* treeList,
				  JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				  const HSizingOption hSizing, const VSizingOption vSizing,
				  const JCoordinate x, const JCoordinate y,
				  const JCoordinate w, const JCoordinate h);

	virtual	~CMStackWidget();

	void		Update();
	void		SelectFrame(const JUInt64 id);
	JBoolean	GetStackFrame(const JUInt64 id, const CMStackFrameNode** frame);
	void		SwitchToFrame(const JUInt64 id);

	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers);

	// void called by CMGetStack

	void	FinishedLoading(const JIndex initID);

protected:

	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);
	virtual JSize	GetMinCellWidth(const JPoint& cell) const;

	virtual void	AdjustCursor(const JPoint& pt, const JXKeyModifiers& modifiers);
	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers);

	virtual void	Receive(JBroadcaster* sender, const Message& message);
	virtual void	ReceiveGoingAway(JBroadcaster* sender);

private:

	CMLink*				itsLink;
	CMCommandDirector*	itsCommandDir;
	CMStackDir*			itsStackDir;
	JTree*				itsTree;
	CMGetStack*			itsGetStackCmd;
	CMGetFrame*			itsGetFrameCmd;
	JBoolean			itsNeedsUpdateFlag;
	JBoolean			itsSmartFrameSelectFlag;
	JBoolean			itsIsWaitingForReloadFlag;
	JBoolean			itsChangingFrameFlag;
	JBoolean			itsSelectingFrameFlag;

private:

	JBoolean	SelectNextFrame(const JInteger delta);

	void		FlushOldData();
	JBoolean	ShouldRebuild() const;
	void		Rebuild();

	// not allowed

	CMStackWidget(const CMStackWidget& source);
	const CMStackWidget& operator=(const CMStackWidget& source);
};

#endif
