/******************************************************************************
 CMThreadsWidget.h

	Copyright © 2001-11 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_CMThreadsWidget
#define _H_CMThreadsWidget

#include <JXNamedTreeListWidget.h>

class JTree;
class CMLink;
class CMCommandDirector;
class CMThreadsDir;
class CMGetThreads;
class CMGetThread;

class CMThreadsWidget : public JXNamedTreeListWidget
{
public:

	CMThreadsWidget(CMCommandDirector* commandDir,
					  CMThreadsDir* threadDir, JTree* tree, JNamedTreeList* treeList,
					  JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	virtual	~CMThreadsWidget();

	void	Update();
	void	SelectThread(const JIndex id);

	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers);

	// void called by CMGetStack

	void	FinishedLoading(const JIndex currentID);

protected:

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
	CMThreadsDir*		itsThreadDir;
	JTree*				itsTree;
	CMGetThreads*		itsGetThreadsCmd;
	CMGetThread*		itsGetCurrentThreadCmd;
	JBoolean			itsNeedsUpdateFlag;
	JBoolean			itsIsWaitingForReloadFlag;
	JBoolean			itsChangingThreadFlag;
	JBoolean			itsSelectingThreadFlag;
	JBoolean			itsFlushWhenRunFlag;
	JArray<JUInt64>*	itsOpenIDList;
	DisplayState		itsDisplayState;

private:

	JBoolean	SelectNextThread(const JInteger delta);
	JBoolean	SelectThread1(const JTreeNode* root, const JIndex id);

	void		FlushOldData();
	JBoolean	ShouldRebuild() const;
	void		Rebuild();

	void	SaveOpenNodes();
	void	SaveOpenNodes1(JTreeNode* root);
	void	RestoreOpenNodes(JTreeNode* root);

	// not allowed

	CMThreadsWidget(const CMThreadsWidget& source);
	const CMThreadsWidget& operator=(const CMThreadsWidget& source);
};

#endif
