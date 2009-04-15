/******************************************************************************
 EditTable.h

	Interface for the EditTable class

	Written by Glenn Bach - 1998.

 ******************************************************************************/

#ifndef _H_EditTable
#define _H_EditTable

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXEditTable.h>
#include <JArray.h>

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

	virtual ~EditTable();
	
protected:
	
	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);
	virtual void	Receive(JBroadcaster* sender, const Message& message);
	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);

	virtual JBoolean		ExtractInputData(const JPoint& cell);
	virtual JXInputField*	CreateXInputField(const JPoint& cell,
												const JCoordinate x, const JCoordinate y,
												const JCoordinate w, const JCoordinate h);
	virtual void			PrepareDeleteXInputField();

	
private:

	JArray<JIndex>* itsData;		// we don't own this!
	JXTextMenu* 	itsTableMenu;
	JXIntegerInput*	itsIntegerInputField;

private:

	void UpdateTableMenu();
	void HandleTableMenu(const JIndex index);

	// not allowed

	EditTable(const EditTable& source);
	const EditTable& operator=(const EditTable& source);
};

#endif
