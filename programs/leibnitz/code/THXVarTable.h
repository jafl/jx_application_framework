/******************************************************************************
 THXVarTable.h

	Interface for the THXVarTable class

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_THXVarTable
#define _H_THXVarTable

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

	bool	OKToRemoveSelectedConstant() const;
	void		RemoveSelectedConstant();

protected:

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

	virtual void			TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;
	virtual JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h) override;
	virtual bool		ExtractInputData(const JPoint& cell) override;
	virtual void			PrepareDeleteXInputField() override;

	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;
	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	THXVarList*		itsVarList;		// not owned
	JXExprInput*	itsTextInput;	// nullptr unless editing
	JXTextMenu*		itsFontMenu;
	JString			itsOrigText;	// used while itsTextInput != nullptr

private:

	void	CreateRow(const JIndex rowIndex);
	void	AdjustColWidths();

	// not allowed

	THXVarTable(const THXVarTable& source);
	const THXVarTable& operator=(const THXVarTable& source);
};

#endif
