/******************************************************************************
 SelectionTable.h

	Interface for the SelectionTable class

	Written by Glenn Bach - 1998.

 ******************************************************************************/

#ifndef _H_SelectionTable
#define _H_SelectionTable

#include <jx-af/jx/JXTable.h>

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

	~SelectionTable() override;
	
protected:
	
	void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;
	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
							const JSize clickCount,
							const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;

	
private:

	JArray<JIndex>* itsData;	// we don't own this!
	JXTextMenu* 	itsTableMenu;

private:

	void UpdateTableMenu();
	void HandleTableMenu(const JIndex index);
};

#endif
