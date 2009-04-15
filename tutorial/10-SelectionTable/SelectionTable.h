/******************************************************************************
 SelectionTable.h

	Interface for the SelectionTable class

	Written by Glenn Bach - 1998.

 ******************************************************************************/

#ifndef _H_SelectionTable
#define _H_SelectionTable

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXTable.h>
#include <JArray.h>

class JXMenuBar;
class JXTextMenu;

class SelectionTable : public JXTable
{
public:

	SelectionTable(JXMenuBar* menuBar, JArray<JIndex>* data,
			JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
		   const HSizingOption hSizing, const VSizingOption vSizing,
		   const JCoordinate x, const JCoordinate y,
		   const JCoordinate w, const JCoordinate h);

	virtual ~SelectionTable();
	
protected:
	
	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);
	virtual void	Receive(JBroadcaster* sender, const Message& message);
	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);

	
private:

	JArray<JIndex>* itsData;	// we don't own this!
	JXTextMenu* 	itsTableMenu;

private:

	void UpdateTableMenu();
	void HandleTableMenu(const JIndex index);

	// not allowed

	SelectionTable(const SelectionTable& source);
	const SelectionTable& operator=(const SelectionTable& source);
};

#endif
