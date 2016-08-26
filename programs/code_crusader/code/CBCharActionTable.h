/******************************************************************************
 CBCharActionTable.h

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBCharActionTable
#define _H_CBCharActionTable

#include "CBKeyScriptTableBase.h"

class CBCharActionManager;
class CBListCSF;

class CBCharActionTable : public CBKeyScriptTableBase
{
public:

	CBCharActionTable(CBEditMacroDialog* dialog,
					  JXTextButton* addRowButton, JXTextButton* removeRowButton,
					  JXTextButton* loadButton, JXTextButton* saveButton,
					  JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	virtual ~CBCharActionTable();

	virtual void	Activate();
	virtual void	Deactivate();

	void	GetData(CBCharActionManager* mgr) const;
	void	SetData(const CBCharActionManager& mgr);
	void	ClearData();

protected:

	virtual JXInputField*
		CreateStringTableInput(const JPoint& cell, JXContainer* enclosure,
							   const HSizingOption hSizing, const VSizingOption vSizing,
							   const JCoordinate x, const JCoordinate y,
							   const JCoordinate w, const JCoordinate h);

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	CBListCSF*		itsCSF;
	JXTextButton*	itsLoadButton;
	JXTextButton*	itsSaveButton;

private:

	void	LoadMacros();
	void	SaveMacros() const;

	void	ReadData(const JCharacter* fileName, const JBoolean replace);
	void	WriteData(const JCharacter* fileName) const;

	// not allowed

	CBCharActionTable(const CBCharActionTable& source);
	const CBCharActionTable& operator=(const CBCharActionTable& source);
};

#endif
