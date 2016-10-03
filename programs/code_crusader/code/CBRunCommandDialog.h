/******************************************************************************
 CBRunCommandDialog.h

	Copyright (C) 2002 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBRunCommandDialog
#define _H_CBRunCommandDialog

#include <JXDialogDirector.h>
#include <JPrefObject.h>

class JXInputField;
class JXTextButton;
class JXTextCheckbox;
class JXTextMenu;
class JXPathHistoryMenu;
class CBProjectDocument;
class CBTextDocument;
class CBCommandManager;
class CBCommandPathInput;

class CBRunCommandDialog : public JXDialogDirector, public JPrefObject
{
public:

	CBRunCommandDialog(CBProjectDocument* projDoc, CBTextDocument* textDoc);
	CBRunCommandDialog(CBProjectDocument* projDoc,
					   const JPtrArray<JString>& fullNameList,
					   const JArray<JIndex>& lineIndexList);

	virtual ~CBRunCommandDialog();

	virtual void	Activate();

protected:

	virtual void	ReadPrefs(istream& input);
	virtual void	WritePrefs(ostream& output) const;

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	CBProjectDocument*	itsProjDoc;			// can be NULL
	CBTextDocument*		itsTextDoc;			// can be NULL
	JPtrArray<JString>*	itsFullNameList;	// can be NULL
	JArray<JIndex>*		itsLineIndexList;	// can be NULL

// begin JXLayout

	JXTextButton*       itsRunButton;
	JXTextButton*       itsHelpButton;
	CBCommandPathInput* itsPathInput;
	JXTextCheckbox*     itsSaveAllCB;
	JXTextCheckbox*     itsOneAtATimeCB;
	JXTextCheckbox*     itsUseWindowCB;
	JXTextCheckbox*     itsIsMakeCB;
	JXInputField*       itsCmdInput;
	JXPathHistoryMenu*  itsPathHistoryMenu;
	JXTextButton*       itsChoosePathButton;
	JXTextButton*       itsChooseCmdButton;
	JXTextCheckbox*     itsRaiseCB;
	JXTextCheckbox*     itsBeepCB;
	JXTextMenu*         itsSaveCmdMenu;
	JXTextCheckbox*     itsIsCVSCB;

// end JXLayout

private:

	void	BuildWindow();
	void	UpdateDisplay();
	void	Exec();

	void	UpdateSaveCmdMenu();
	void	HandleSaveCmdMenu(const JIndex item);
	void	AddCommandToMenu(CBCommandManager* mgr);

	// not allowed

	CBRunCommandDialog(const CBRunCommandDialog& source);
	const CBRunCommandDialog& operator=(const CBRunCommandDialog& source);
};

#endif
