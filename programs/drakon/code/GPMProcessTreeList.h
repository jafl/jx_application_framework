/******************************************************************************
 GPMProcessTreeList.h

	Copyright © 2006 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_GPMProcessTreeList
#define _H_GPMProcessTreeList

#include <JXNamedTreeListWidget.h>

class GPMProcessList;
class GPMProcessEntry;

class JXTextMenu;
class JXToolBar;
class JXTEBase;
class JXImage;

class GPMProcessTreeList : public JXNamedTreeListWidget
{
public:

	GPMProcessTreeList(GPMProcessList* list, JNamedTreeList* treeList,
						JXTEBase* fullCmdDisplay,
						JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
						const HSizingOption hSizing, const VSizingOption vSizing,
						const JCoordinate x, const JCoordinate y,
						const JCoordinate w, const JCoordinate h);

	virtual ~GPMProcessTreeList();

	JBoolean	GetSelectedProcess(const GPMProcessEntry** entry) const;
	void		SelectProcess(const GPMProcessEntry& entry);

protected:

	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	GPMProcessList*			itsList;			// not owned
	JXTextMenu*				itsContextMenu;
	const GPMProcessEntry* 	itsSelectedEntry;
	JXTEBase*				itsFullCmdDisplay;
	JXImage*				itsZombieImage;

private:

	void	UpdateContextMenu();
	void	HandleContextMenu(const JIndex index);

	// not allowed

	GPMProcessTreeList(const GPMProcessTreeList& source);
	const GPMProcessTreeList& operator=(const GPMProcessTreeList& source);

};

#endif
