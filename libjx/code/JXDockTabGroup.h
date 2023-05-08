/******************************************************************************
 JXDockTabGroup.h

	Copyright (C) 2006 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXDockTabGroup
#define _H_JXDockTabGroup

#include "JXTabGroup.h"

class JXTextMenu;
class JXDockWidget;

class JXDockTabGroup : public JXTabGroup
{
public:

	JXDockTabGroup(JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	~JXDockTabGroup() override;

	void	SetDockWidget(JXDockWidget* dock);

protected:

	void	DrawTab(const JIndex index, JXWindowPainter& p,
					const JRect& rect, const Edge edge) override;
	bool	OKToDeleteTab(const JIndex index) override;

	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
							const JSize clickCount,
							const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;

private:

	JXDockWidget*	itsDockWidget;

	JXTextMenu*		itsDockContextMenu;
	JIndex			itsDockContextIndex;

private:

	void	CreateDockContextMenu();
	void	UpdateDockContextMenu();
	void	HandleDockContextMenu(const JIndex index);
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
