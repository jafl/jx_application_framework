/******************************************************************************
 JXHorizDockPartition.h

	Copyright (C) 2002-08 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXHorizDockPartition
#define _H_JXHorizDockPartition

#include "jx-af/jx/JXHorizPartition.h"

class JXTextMenu;
class JXDockDirector;
class JXVertDockPartition;
class JXDockWidget;

class JXHorizDockPartition : public JXHorizPartition
{
public:

	JXHorizDockPartition(JXDockDirector* director, JXDockWidget* parent,
						 const JArray<JCoordinate>& widths, const JIndex elasticIndex,
						 const JArray<JCoordinate>& minWidths, JXContainer* enclosure,
						 const HSizingOption hSizing, const VSizingOption vSizing,
						 const JCoordinate x, const JCoordinate y,
						 const JCoordinate w, const JCoordinate h);

	~JXHorizDockPartition() override;

	JXDockDirector*	GetDockDirector() const;
	bool			FindDock(const JIndex id, JXDockWidget** dock);
	bool			HasWindows() const;
	bool			CloseAllWindows();
	JPoint			UpdateMinSize();

	void	InsertCompartment(const JIndex index, const bool reportError = false);
	void	DeleteCompartment(const JIndex index);

	bool	SplitVert(const JIndex index,
					  JXVertDockPartition** returnPartition = nullptr,
					  const bool reportError = false);

	void	ReadSetup(std::istream& input, const JFileVersion vers);
	void	WriteSetup(std::ostream& output) const;

	// for use by JXVertDockPartition

	JXDockWidget*	GetDockWidget(const JIndex index) const;

protected:

	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
							const JSize clickCount,
							const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;

	JXWidgetSet*	CreateCompartment(const JIndex index,
									  const JCoordinate position,
									  const JCoordinate size) override;
	bool			SaveGeometryForLater(const JArray<JCoordinate>& sizes) override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXDockDirector*				itsDirector;
	JXTextMenu*					itsDockMenu;			// nullptr until first used
	JXDockWidget*				itsParentDock;			// nullptr if top-level partition
	JIndex						itsCompartmentIndex;	// left index; used when Dock menu is open
	JPtrArray<JXDockWidget>*	itsDockList;			// items not owned

private:

	void	UpdateDockMenu();
	void	HandleDockMenu(const JIndex index);
};


/******************************************************************************
 GetDockDirector

 ******************************************************************************/

inline JXDockDirector*
JXHorizDockPartition::GetDockDirector()
	const
{
	return itsDirector;
}

/******************************************************************************
 GetDockWidget

 ******************************************************************************/

inline JXDockWidget*
JXHorizDockPartition::GetDockWidget
	(
	const JIndex index
	)
	const
{
	return itsDockList->GetElement(index);
}

#endif
