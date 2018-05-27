/******************************************************************************
 JXDockTabGroup.h

	Copyright (C) 2006 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXDockTabGroup
#define _H_JXDockTabGroup

#include <JXTabGroup.h>

class JXTextMenu;
class JXDockWidget;

class JXDockTabGroup : public JXTabGroup
{
public:

	JXDockTabGroup(JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	virtual ~JXDockTabGroup();

	void	SetDockWidget(JXDockWidget* dock);

protected:

	virtual void		DrawTab(const JIndex index, JXWindowPainter& p,
								const JRect& rect, const Edge edge) override;
	virtual JBoolean	OKToDeleteTab(const JIndex index) override;

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXDockWidget*	itsDockWidget;

	JXTextMenu*		itsDockContextMenu;
	JIndex			itsDockContextIndex;

private:

	void	CreateDockContextMenu();
	void	UpdateDockContextMenu();
	void	HandleDockContextMenu(const JIndex index);

	// not allowed

	JXDockTabGroup(const JXDockTabGroup& source);
	const JXDockTabGroup& operator=(const JXDockTabGroup& source);
};


/******************************************************************************
 SetDockWidget

 ******************************************************************************/

inline void
JXDockTabGroup::SetDockWidget
	(
	JXDockWidget* dock
	)
{
	itsDockWidget = dock;
}

#endif
