/******************************************************************************
 GMFilterActionTable.h

	Interface for the GMFilterActionTable class

	Copyright © 2000 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#ifndef _H_GMFilterActionTable
#define _H_GMFilterActionTable


#include <JXEditTable.h>

class GMFilterDialog;
class GMFilterAction;
class GMFilter;
class JXTextButton;
class JXInputField;

class GMFilterActionTable : public JXEditTable
{
public:

public:

	static GMFilterActionTable* Create(GMFilterDialog* dialog,
				JXTextButton* newButton,
				JXTextButton* removeButton, JXTextButton* chooseButton,
				JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);
	virtual ~GMFilterActionTable();

	void		SetFilter(GMFilter* filter);
	JBoolean	OKToSwitch();

protected:

	GMFilterActionTable(GMFilterDialog* dialog,
				JXTextButton* newButton,
				JXTextButton* removeButton, JXTextButton* chooseButton,
				JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	virtual void	Receive(JBroadcaster* sender, const Message& message);
	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);
	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh);
	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);

	virtual JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h);
	virtual void			PrepareDeleteXInputField();
	virtual JBoolean		ExtractInputData(const JPoint& cell);

private:

	GMFilterDialog* itsDialog;
	JXTextButton*	itsNewButton;
	JXTextButton*	itsRemoveButton;
	JXTextButton*	itsChooseButton;
	JXInputField*	itsInput;

	GMFilter*						itsFilter;	// we don't own this.
	JPtrArray<GMFilterAction>*	itsActions;	// we don't own this.

	JSize			itsRowHeight;

private:

	void GMFilterActionTableX();
	void AdjustColWidths();
	void AdjustButtons();
	void NewAction();
	void RemoveSelectedActions();
	void ChooseMailbox();

	// not allowed

	GMFilterActionTable(const GMFilterActionTable& source);
	const GMFilterActionTable& operator=(const GMFilterActionTable& source);

public:
};

#endif
