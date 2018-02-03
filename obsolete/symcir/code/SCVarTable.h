/******************************************************************************
 SCVarTable.h

	Interface for the SCVarTable class

	Copyright (C) 1996 by John Lindal.

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

	void	ReadSetup(std::istream& input, const JFileVersion vers);
	void	WriteSetup(std::ostream& output) const;

	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers) override;

protected:

	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);
	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

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

#endif
