/******************************************************************************
 GLVarTable.h

	Interface for the GLVarTable class

	Copyright (C) 2000 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_GLVarTable
#define _H_GLVarTable

#include <JXEditTable.h>

class JXTextMenu;
class JXExprInput;
class GLVarList;

class GLVarTable : public JXEditTable
{
public:

	enum
	{
		kNameColumn = 1,
		kFnColumn   = 2
	};

public:

	GLVarTable(GLVarList* varList, 
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
									const JXKeyModifiers& modifiers) override;

	virtual void			TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;
	virtual JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h) override;
	virtual JBoolean		ExtractInputData(const JPoint& cell) override;
	virtual void			PrepareDeleteXInputField() override;

	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;
	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	GLVarList*		itsVarList;		// not owned
	JXExprInput*	itsTextInput;	// nullptr unless editing
	JString*		itsOrigText;	// used while itsTextInput != nullptr

private:

	void	CreateRow(const JIndex rowIndex);
	void	AdjustColWidths();

	// not allowed

	GLVarTable(const GLVarTable& source);
	const GLVarTable& operator=(const GLVarTable& source);
};

#endif
