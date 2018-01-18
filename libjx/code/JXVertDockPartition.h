/******************************************************************************
 JXVertDockPartition.h

	Copyright (C) 2002-08 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXVertDockPartition
#define _H_JXVertDockPartition

#include <JXVertPartition.h>

class JXTextMenu;
class JXDockDirector;
class JXHorizDockPartition;
class JXDockWidget;

class JXVertDockPartition : public JXVertPartition
{
public:

	JXVertDockPartition(JXDockDirector* director, JXDockWidget* parent,
						const JArray<JCoordinate>& widths, const JIndex elasticIndex,
						const JArray<JCoordinate>& minWidths, JXContainer* enclosure,
						const HSizingOption hSizing, const VSizingOption vSizing,
						const JCoordinate x, const JCoordinate y,
						const JCoordinate w, const JCoordinate h);

	virtual ~JXVertDockPartition();

	JXDockDirector*	GetDockDirector() const;
	JBoolean		FindDock(const JIndex id, JXDockWidget** dock);
	JBoolean		HasWindows() const;
	JBoolean		CloseAllWindows();
	JPoint			UpdateMinSize();

	void	InsertCompartment(const JIndex index, const JBoolean reportError = kJFalse);
	void	DeleteCompartment(const JIndex index);

	JBoolean	SplitHoriz(const JIndex index,
						   JXHorizDockPartition** returnPartition = NULL,
						   const JBoolean reportError = kJFalse);

	void	ReadSetup(std::istream& input, const JFileVersion vers);
	void	WriteSetup(std::ostream& output) const;

	// for use by JXHorizDockPartition

	JXDockWidget*	GetDockWidget(const JIndex index) const;

protected:

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

	virtual JXWidgetSet*	CreateCompartment(const JIndex index,
											  const JCoordinate position,
											  const JCoordinate size);
	virtual JBoolean		SaveGeometryForLater(const JArray<JCoordinate>& sizes);

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXDockDirector*				itsDirector;
	JXTextMenu*					itsDockMenu;			// NULL until first used
	JXDockWidget*				itsParentDock;			// NULL if top-level partition
	JIndex						itsCompartmentIndex;	// left index; used when Dock menu is open
	JPtrArray<JXDockWidget>*	itsDockList;			// items not owned

private:

	void	UpdateDockMenu();
	void	HandleDockMenu(const JIndex index);

	// not allowed

	JXVertDockPartition(const JXVertDockPartition& source);
	const JXVertDockPartition& operator=(const JXVertDockPartition& source);
};


/******************************************************************************
 GetDockDirector

 ******************************************************************************/

inline JXDockDirector*
JXVertDockPartition::GetDockDirector()
	const
{
	return itsDirector;
}

/******************************************************************************
 GetDockWidget

 ******************************************************************************/

inline JXDockWidget*
JXVertDockPartition::GetDockWidget
	(
	const JIndex index
	)
	const
{
	return itsDockList->GetElement(index);
}

#endif
