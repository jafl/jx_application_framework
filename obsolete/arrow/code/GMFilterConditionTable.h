/******************************************************************************
 GMFilterConditionTable.h

	Interface for the GMFilterConditionTable class

	Copyright (C) 2000 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#ifndef _H_GMFilterConditionTable
#define _H_GMFilterConditionTable


#include <JXEditTable.h>

class GMFilterDialog;
class GMFilterCondition;
class GMFilter;
class JXTextButton;
class JXInputField;
class JXTextMenu;

class GMFilterConditionTable : public JXEditTable
{
public:

public:

	static GMFilterConditionTable* Create(GMFilterDialog* dialog,
				JXTextButton* newButton, JXTextButton* removeButton,
				JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);
	virtual ~GMFilterConditionTable();

	void		SetFilter(GMFilter* filter);
	JBoolean	OKToSwitch();

protected:

	GMFilterConditionTable(GMFilterDialog* dialog,
				JXTextButton* newButton, JXTextButton* removeButton,
				JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);
	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;
	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

	virtual JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h);
	virtual void			PrepareDeleteXInputField();
	virtual JBoolean		ExtractInputData(const JPoint& cell);

private:

	GMFilterDialog* itsDialog;
	JXTextButton*	itsNewButton;
	JXTextButton*	itsRemoveButton;
	JXInputField*	itsInput;

	GMFilter*						itsFilter;	// we don't own this.
	JPtrArray<GMFilterCondition>*	itsConditions;	// we don't own this.

	JSize			itsRowHeight;
	JXTextMenu*		itsTypeMenu;
	JXTextMenu*		itsRelationMenu;

private:

	void GMFilterConditionTableX();
	void AdjustColWidths();
	void AdjustButtons();
	void NewCondition();
	void RemoveSelectedConditions();

	void HandleTypeMenu(const JIndex index);
	void HandleRelationMenu(const JIndex index);

	// not allowed

	GMFilterConditionTable(const GMFilterConditionTable& source);
	const GMFilterConditionTable& operator=(const GMFilterConditionTable& source);

public:
};

#endif
