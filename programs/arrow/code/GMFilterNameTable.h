/******************************************************************************
 GMFilterNameTable.h

	Interface for the GMFilterNameTable class

	Copyright © 1999 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#ifndef _H_GMFilterNameTable
#define _H_GMFilterNameTable


#include <JXEditTable.h>

class GMFilterDialog;
class GMFilterConditionTable;
class GMFilterActionTable;
class GMFilter;
class JXTextButton;
class JXInputField;

class GMFilterNameTable : public JXEditTable
{
public:

public:

	static GMFilterNameTable* Create(GMFilterDialog* dialog,
				JPtrArray<GMFilter>* filters,
				GMFilterConditionTable* conditionTable, GMFilterActionTable* mboxTable,
				JXTextButton* newButton, JXTextButton* removeButton,
				JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);
	virtual ~GMFilterNameTable();

	JBoolean	OKToDeactivate();

protected:

	GMFilterNameTable(GMFilterDialog* dialog,
				JPtrArray<GMFilter>* filters,
				GMFilterConditionTable* conditionTable, GMFilterActionTable* mboxTable,
				JXTextButton* newButton, JXTextButton* removeButton,
				JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	virtual void	Receive(JBroadcaster* sender, const Message& message);
	virtual void	Draw(JXWindowPainter& p, const JRect& rect);
	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);
	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh);
	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers);
	virtual Atom	GetDNDAction(const JXContainer* target,
								 const JXButtonStates& buttonStates,
								 const JXKeyModifiers& modifiers);
	virtual JBoolean	WillAcceptDrop(const JArray<Atom>& typeList, Atom* action,
									   const JPoint& pt, const Time time,
									   const JXWidget* source);
	virtual void	HandleDNDHere(const JPoint& pt, const JXWidget* source);
	virtual void	HandleDNDLeave();
	virtual void	HandleDNDDrop(	const JPoint& pt, const JArray<Atom>& typeList,
									const Atom	action, const Time time,
									const JXWidget* source);

	virtual JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h);
	virtual void			PrepareDeleteXInputField();
	virtual JBoolean		ExtractInputData(const JPoint& cell);

private:

	GMFilterDialog*			itsDialog;
	JXTextButton*			itsNewButton;
	JXTextButton*			itsRemoveButton;
	JXInputField*			itsInput;
	JPtrArray<GMFilter>*	itsFilters;		// we don't own this.
	GMFilterConditionTable*	itsConditionTable;
	GMFilterActionTable*	itsActionTable;

	JSize					itsRowHeight;

	// DND
	JBoolean				itsDNDIsValid;
	JIndex					itsCurrentDNDIndex;
	JPoint					itsCurrentCell;
	JPoint					itsStartPt;
	JBoolean				itsIsWaitingForDrag;

private:

	void GMFilterNameTableX();
	void AdjustColWidths();
	void AdjustButtons();
	void NewFilter();
	void RemoveSelectedFilters();
	void SelectFilter(const JIndex index);

	// not allowed

	GMFilterNameTable(const GMFilterNameTable& source);
	const GMFilterNameTable& operator=(const GMFilterNameTable& source);

public:
};

#endif
