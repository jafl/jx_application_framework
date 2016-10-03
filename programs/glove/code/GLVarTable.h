/******************************************************************************
 GLVarTable.h

	Interface for the GLVarTable class

	Copyright (C) 2000 by Glenn Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_GLVarTable
#define _H_GLVarTable

#include <JXEditTable.h>

class JXTextMenu;
class JXExprInput;
class GVarList;

class GLVarTable : public JXEditTable
{
public:

	enum
	{
		kNameColumn = 1,
		kFnColumn   = 2
	};

public:

	GLVarTable(GVarList* varList, 
				JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	virtual ~GLVarTable();

	void	NewConstant();
	void	RemoveSelectedConstant();

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

	GVarList*		itsVarList;		// not owned
	JXExprInput*	itsTextInput;	// NULL unless editing
	JString*		itsOrigText;	// used while itsTextInput != NULL

private:

	void	CreateRow(const JIndex rowIndex);
	void	AdjustColWidths();

	// not allowed

	GLVarTable(const GLVarTable& source);
	const GLVarTable& operator=(const GLVarTable& source);
};

#endif
