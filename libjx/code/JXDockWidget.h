/******************************************************************************
 JXDockWidget.h

	Copyright (C) 2002 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXDockWidget
#define _H_JXDockWidget

#include "JXWidgetSet.h"

class JXWindowDirector;
class JXDockDirector;
class JXPartition;
class JXHorizDockPartition;
class JXVertDockPartition;
class JXTabGroup;
class JXHintDirector;
class JXIdleTask;

class JXDockWidget : public JXWidgetSet
{
public:

	static constexpr JCoordinate kDefaultMinSize = 10;

public:

	JXDockWidget(JXDockDirector* director, JXPartition* partition,
				 const bool isHorizPartition, JXTabGroup* tabGroup,
				 JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	~JXDockWidget() override;

	JXDockDirector*	GetDockDirector() const;
	JXTabGroup*		GetTabGroup() const;
	JPoint			GetMinSize() const;
	void			UpdateMinSize();

	bool	HasWindows() const;
	bool	CloseAllWindows();		// recursive

	using JXContainer::GetWindow;

	JSize	GetWindowCount() const;
	bool	GetWindow(const JIndex index, JXWindow** window);
	void	ShowPreviousWindow();
	void	ShowNextWindow();

	bool	WindowWillFit(JXWindow* w) const;
	bool	Dock(JXWindowDirector* d, const bool reportError = false);
	bool	Dock(JXWindow* w, const bool reportError = false);
	bool	TransferAll(JXDockWidget* target);
	void	UndockAll();
	bool	CloseAll();				// not recursive

	bool	GetHorizChildPartition(JXHorizDockPartition** p) const;
	bool	GetVertChildPartition(JXVertDockPartition** p) const;
	void	SetChildPartition(JXPartition* p);

	JIndex	GetID() const;
	void	SetID(const JIndex id);

protected:

	bool	WillAcceptDrop(const JArray<Atom>& typeList, Atom* action,
					   const JPoint& pt, const Time time,
						   const JXWidget* source) override;
	void	HandleDNDDrop(const JPoint& pt, const JArray<Atom>& typeList,
						  const Atom action, const Time time,
						  const JXWidget* source) override;
	void	HandleDNDLeave() override;

	void	Draw(JXWindowPainter& p, const JRect& rect) override;
	void	BoundsMoved(const JCoordinate dx, const JCoordinate dy) override;
	void	BoundsResized(const JCoordinate dw, const JCoordinate dh) override;

	void	Receive(JBroadcaster* sender, const Message& message) override;
	void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	JXDockDirector*			itsDirector;
	JXPartition*			itsPartition;
	const bool				itsIsHorizFlag;
	JXTabGroup*				itsTabGroup;
	JIndex					itsID;
	JPtrArray<JXWindow>*	itsWindowList;		// nullptr if contains partition
	JPoint					itsMinSize;
	JXPartition*			itsChildPartition;	// nullptr if contains docked windows
	JXHintDirector*			itsHintDirector;	// nullptr unless showing hint; not owned
	JXIdleTask*				itsDeleteHintTask;	// nullptr unless showing hint

private:

	bool	FindWindow(JBroadcaster* sender, JXWindow** window, JIndex* index) const;
	bool	RemoveWindow(JBroadcaster* sender);
	JIndex		GetTabInsertionIndex(JXWindow* w, const JIndex ignoreIndex = 0) const;
};


/******************************************************************************
 GetDockDirector

 ******************************************************************************/

inline JXDockDirector*
JXDockWidget::GetDockDirector()
	const
{
	return itsDirector;
}

/******************************************************************************
 GetTabGroup

 ******************************************************************************/

inline JXTabGroup*
JXDockWidget::GetTabGroup()
	const
{
	return itsTabGroup;
}

/******************************************************************************
 GetID

 ******************************************************************************/

inline JIndex
JXDockWidget::GetID()
	const
{
	return itsID;
}

/******************************************************************************
 GetWindowCount

 ******************************************************************************/

inline JSize
JXDockWidget::GetWindowCount()
	const
{
	return (itsWindowList == nullptr ? 0 : itsWindowList->GetItemCount());
}

/******************************************************************************
 GetWindow

 ******************************************************************************/

inline bool
JXDockWidget::GetWindow
	(
	const JIndex	index,
	JXWindow**		window
	)
{
	if (itsWindowList == nullptr)
		{
		*window = nullptr;
		return false;
		}

	*window = itsWindowList->GetItem(index);
	return true;
}

#endif
