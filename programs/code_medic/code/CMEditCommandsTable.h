/******************************************************************************
 CMEditCommandsTable.h

	Copyright (C) 1999 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#ifndef _H_CMEditCommandsTable
#define _H_CMEditCommandsTable

#include <JXEditTable.h>

class JXTextButton;
class CMEditCommandsDialog;

class CMEditCommandsTable : public JXEditTable
{
public:

	CMEditCommandsTable(CMEditCommandsDialog* dialog, JXTextButton* removeButton,
						JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
						const HSizingOption hSizing, const VSizingOption vSizing,
						const JCoordinate x, const JCoordinate y,
						const JCoordinate w, const JCoordinate h);

	virtual	~CMEditCommandsTable();

	void	SyncWithData();
	void	NewStringAppended();
	void	AdjustColWidth();
	void	RemoveCurrent();

protected:

	virtual JBoolean		ExtractInputData(const JPoint& cell);
	virtual JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h);
	virtual void			PrepareDeleteXInputField();

	virtual void			TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);
	virtual void			HandleMouseDown(const JPoint& pt, const JXMouseButton button,
											const JSize clickCount,
											const JXButtonStates& buttonStates,
											const JXKeyModifiers& modifiers);
	virtual void			ApertureResized(const JCoordinate dw, const JCoordinate dh);

private:

	CMEditCommandsDialog*	itsDialog;
	JCoordinate				itsMinColWidth;
	JXInputField*			itsCmdInput;
    JXTextButton*			itsRemoveButton;

private:

	// not allowed

	CMEditCommandsTable(const CMEditCommandsTable& source);
	const CMEditCommandsTable& operator=(const CMEditCommandsTable& source);
};

#endif
