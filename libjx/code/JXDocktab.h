/******************************************************************************
 JXDocktab.h

	Copyright (C) 2002 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXDocktab
#define _H_JXDocktab

#include <JXWidget.h>
#include <JXDNDManager.h>
#include <JPartition.h>		// need defn of kDragRegionSize

class JXTextMenu;

class JXDocktab : public JXWidget
{
public:

	enum
	{
		kWidth = 9
	};

public:

	JXDocktab(JXContainer* enclosure,
			  const HSizingOption hSizing, const VSizingOption vSizing,
			  const JCoordinate x, const JCoordinate y,
			  const JCoordinate w, const JCoordinate h);

	virtual ~JXDocktab();

protected:

	virtual void	Draw(JXWindowPainter& p, const JRect& rect) override;
	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

	virtual Atom	GetDNDAction(const JXContainer* target,
								 const JXButtonStates& buttonStates,
								 const JXKeyModifiers& modifiers) override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	class DockFinder : public JXDNDManager::TargetFinder
		{
		public:

			DockFinder(JXDisplay* display);

			virtual	~DockFinder();

			virtual JBoolean	FindTarget(const JXContainer* coordOwner,
										   const JPoint& pt,
										   Window* xWindow, Window* msgWindow,
										   JXContainer** target, Atom* vers);
		};

private:

	JXTextMenu*	itsActionMenu;	// NULL until first used
	DockFinder*	itsDockFinder;
	JColorIndex	itsFocusColor;

private:

	void	OpenActionMenu(const JPoint& pt, const JXButtonStates& buttonStates,
						   const JXKeyModifiers& modifiers);
	void	UpdateActionMenu();
	void	HandleActionMenu(const JIndex index);

	// not allowed

	JXDocktab(const JXDocktab& source);
	const JXDocktab& operator=(const JXDocktab& source);
};

#endif
