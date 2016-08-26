/******************************************************************************
 SCVarTable.h

	Interface for the SCVarTable class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SCVarTable
#define _H_SCVarTable

#include <JXTable.h>

class SCCircuitVarList;
class SCVarListDirector;
class JXColHeaderWidget;

class SCVarTable : public JXTable
{
public:

	SCVarTable(JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
			   const HSizingOption hSizing, const VSizingOption vSizing,
			   const JCoordinate x, const JCoordinate y,
			   const JCoordinate w, const JCoordinate h);

	virtual ~SCVarTable();

	void	SetVarList(SCCircuitVarList* varList);
	void	ConfigureColHeader(JXColHeaderWidget* colHeader);
	void	EditFunction(const JIndex varIndex = 0);

	void	ReadSetup(istream& input, const JFileVersion vers);
	void	WriteSetup(ostream& output) const;

	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers);

protected:

	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);
	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	SCCircuitVarList*	itsVarList;		// owned by document
	JArray<JIndex>*		itsVisibleList;	// indicies of visible variables

private:

	JBoolean	GetSelectedRow(JIndex* rowIndex) const;
	JBoolean	GetSelectedVariable(JIndex* varIndex) const;
	void		SelectRow(const JIndex rowIndex);
	void		MoveSelection(const JCoordinate delta);

	JIndex		RowToVarIndex(const JIndex rowIndex) const;

	// not allowed

	SCVarTable(const SCVarTable& source);
	const SCVarTable& operator=(const SCVarTable& source);
};


/******************************************************************************
 RowToVarIndex (private)

 ******************************************************************************/

inline JIndex
SCVarTable::RowToVarIndex
	(
	const JIndex rowIndex
	)
	const
{
	return itsVisibleList->GetElement(rowIndex);
}

#endif
