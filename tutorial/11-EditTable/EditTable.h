/******************************************************************************
 EditTable.h

	Interface for the EditTable class

	Written by Glenn Bach - 1998.

 ******************************************************************************/

#ifndef _H_EditTable
#define _H_EditTable

#include <jx-af/jx/JXEditTable.h>

class JXMenuBar;
class JXTextMenu;
class JXIntegerInput;
class JXInputField;

class EditTable : public JXEditTable
{
public:

	EditTable(JXMenuBar* menuBar, JArray<JIndex>* data,
			JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
		   const HSizingOption hSizing, const VSizingOption vSizing,
		   const JCoordinate x, const JCoordinate y,
		   const JCoordinate w, const JCoordinate h);

	~EditTable() override;
	
protected:
	
	void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;
	void	Receive(JBroadcaster* sender, const Message& message) override;
	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
							const JSize clickCount,
							const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;

	bool			ExtractInputData(const JPoint& cell) override;
	JXInputField*	CreateXInputField(const JPoint& cell,
									  const JCoordinate x, const JCoordinate y,
									  const JCoordinate w, const JCoordinate h) override;
	void			PrepareDeleteXInputField() override;

	
private:

	JArray<JIndex>* itsData;		// we don't own this!
	JXTextMenu* 	itsTableMenu;
	JXIntegerInput*	itsIntegerInputField;

private:

	void UpdateTableMenu();
	void HandleTableMenu(const JIndex index);
};

#endif
