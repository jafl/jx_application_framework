/******************************************************************************
 CBRunCommandDialog.h

	Copyright © 2002 by John Lindal.

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

	virtual void	Activate() override;

protected:

	virtual void	ReadPrefs(std::istream& input) override;
	virtual void	WritePrefs(std::ostream& output) const  override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	CBProjectDocument*	itsProjDoc;			// can be nullptr
	CBTextDocument*		itsTextDoc;			// can be nullptr
	JPtrArray<JString>*	itsFullNameList;	// can be nullptr
	JArray<JIndex>*		itsLineIndexList;	// can be nullptr

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
