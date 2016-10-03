/******************************************************************************
 CBKeyScriptTableBase.h

	Interface for the CBKeyScriptTableBase class

	Copyright (C) 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBKeyScriptTableBase
#define _H_CBKeyScriptTableBase

#include <JXStringTable.h>

class JXTextButton;
class CBEditMacroDialog;

class CBKeyScriptTableBase : public JXStringTable
{
public:

	enum
	{
		kMacroColumn  = 1,
		kScriptColumn = 2
	};

public:

	CBKeyScriptTableBase(CBEditMacroDialog* dialog,
						 JXTextButton* addRowButton, JXTextButton* removeRowButton,
						 JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
						 const HSizingOption hSizing, const VSizingOption vSizing,
						 const JCoordinate x, const JCoordinate y,
						 const JCoordinate w, const JCoordinate h);

	virtual ~CBKeyScriptTableBase();

	virtual void	Activate();
	virtual void	Deactivate();

	JBoolean	ContentsValid() const;

protected:

	CBEditMacroDialog*	GetDialog() const;

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);

	virtual JXInputField*
		CreateStringTableInput(const JPoint& cell, JXContainer* enclosure,
							   const HSizingOption hSizing, const VSizingOption vSizing,
							   const JCoordinate x, const JCoordinate y,
							   const JCoordinate w, const JCoordinate h);

	virtual void	PrepareDeleteXInputField();
	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh);
	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	CBEditMacroDialog*	itsDialog;
	JXTextButton*		itsAddRowButton;
	JXTextButton*		itsRemoveRowButton;

private:

	void	AddRow();
	void	RemoveRow();

	void	AdjustColWidths();

	// not allowed

	CBKeyScriptTableBase(const CBKeyScriptTableBase& source);
	const CBKeyScriptTableBase& operator=(const CBKeyScriptTableBase& source);
};


/******************************************************************************
 GetDialog (protected)

 ******************************************************************************/

inline CBEditMacroDialog*
CBKeyScriptTableBase::GetDialog()
	const
{
	return itsDialog;
}

#endif
