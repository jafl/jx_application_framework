/******************************************************************************
 JXDocktab.h

	Copyright (C) 2002 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXDocktab
#define _H_JXDocktab

#include "JXWidget.h"
#include "JXDNDManager.h"
#include <jx-af/jcore/JPartition.h>		// for kDragRegionSize

class JXTextMenu;

class JXDocktab : public JXWidget
{
public:

	static const JCoordinate kWidth = 9;

public:

	JXDocktab(JXContainer* enclosure,
			  const HSizingOption hSizing, const VSizingOption vSizing,
			  const JCoordinate x, const JCoordinate y,
			  const JCoordinate w, const JCoordinate h);

	~JXDocktab() override;

protected:

	void	Draw(JXWindowPainter& p, const JRect& rect) override;
	void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;

	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
							const JSize clickCount,
							const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;

	Atom	GetDNDAction(const JXContainer* target,
						 const JXButtonStates& buttonStates,
						 const JXKeyModifiers& modifiers) override;

private:

	class DockFinder : public JXDNDManager::TargetFinder
		{
		public:

			DockFinder(JXDisplay* display);

			~DockFinder() override;

			bool	FindTarget(const JXContainer* coordOwner,
							   const JPoint& pt,
							   Window* xWindow, Window* msgWindow,
							   JXContainer** target, Atom* vers) override;
		};

private:

	JXTextMenu*	itsActionMenu;	// nullptr until first used
	DockFinder*	itsDockFinder;
	JColorID	itsFocusColor;

private:

	void	OpenActionMenu(const JPoint& pt, const JXButtonStates& buttonStates,
						   const JXKeyModifiers& modifiers);
	void	UpdateActionMenu();
	void	HandleActionMenu(const JIndex index);
};

#endif
