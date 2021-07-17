/******************************************************************************
 CBCharActionTable.h

	Copyright © 1998 by John Lindal.

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

	virtual void	Activate() override;
	virtual void	Deactivate() override;

	void	GetData(CBCharActionManager* mgr) const;
	void	SetData(const CBCharActionManager& mgr);
	void	ClearData();

protected:

	virtual JXInputField*
		CreateStringTableInput(const JPoint& cell, JXContainer* enclosure,
							   const HSizingOption hSizing, const VSizingOption vSizing,
							   const JCoordinate x, const JCoordinate y,
							   const JCoordinate w, const JCoordinate h) override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	CBListCSF*		itsCSF;
	JXTextButton*	itsLoadButton;
	JXTextButton*	itsSaveButton;

private:

	void	LoadMacros();
	void	SaveMacros() const;

	void	ReadData(const JString& fileName, const bool replace);
	void	WriteData(const JString& fileName) const;

	// not allowed

	CBCharActionTable(const CBCharActionTable& source);
	const CBCharActionTable& operator=(const CBCharActionTable& source);
};

#endif
