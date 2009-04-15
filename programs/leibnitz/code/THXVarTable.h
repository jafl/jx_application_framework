/******************************************************************************
 THXVarTable.h

	Interface for the THXVarTable class

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_THXVarTable
#define _H_THXVarTable

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXEditTable.h>

class JXTextMenu;
class JXExprInput;
class THXVarList;

class THXVarTable : public JXEditTable
{
public:

	enum
	{
		kNameColumn = 1,
		kFnColumn   = 2
	};

public:

	THXVarTable(THXVarList*	varList, JXTextMenu* fontMenu,
				JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	virtual ~THXVarTable();

	void	NewConstant();

	JBoolean	OKToRemoveSelectedConstant() const;
	void		RemoveSelectedConstant();

protected:

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);

	virtual void			TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);
	virtual JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h);
	virtual JBoolean		ExtractInputData(const JPoint& cell);
	virtual void			PrepareDeleteXInputField();

	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh);
	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	THXVarList*		itsVarList;		// not owned
	JXExprInput*	itsTextInput;	// NULL unless editing
	JXTextMenu*		itsFontMenu;
	JString*		itsOrigText;	// used while itsTextInput != NULL

private:

	void	CreateRow(const JIndex rowIndex);
	void	AdjustColWidths();

	// not allowed

	THXVarTable(const THXVarTable& source);
	const THXVarTable& operator=(const THXVarTable& source);
};

#endif
