/******************************************************************************
 CBMacroTable.h

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBMacroTable
#define _H_CBMacroTable

#include "CBKeyScriptTableBase.h"

class CBMacroManager;
class CBListCSF;

class CBMacroTable : public CBKeyScriptTableBase
{
public:

	CBMacroTable(CBEditMacroDialog* dialog,
				 JXTextButton* addRowButton, JXTextButton* removeRowButton,
				 JXTextButton* loadButton, JXTextButton* saveButton,
				 JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	virtual ~CBMacroTable();

	virtual void	Activate();
	virtual void	Deactivate();

	void	GetData(CBMacroManager* mgr) const;
	void	SetData(const CBMacroManager& mgr);
	void	ClearData();

protected:

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

	CBMacroTable(const CBMacroTable& source);
	const CBMacroTable& operator=(const CBMacroTable& source);
};

#endif
