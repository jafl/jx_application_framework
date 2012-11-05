/******************************************************************************
 CBRunTEScriptDialog.h

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBRunTEScriptDialog
#define _H_CBRunTEScriptDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWindowDirector.h>
#include <JPrefObject.h>

class JTextEditor;
class JXTextButton;
class JXTextCheckbox;
class JXInputField;
class JXStringHistoryMenu;

class CBRunTEScriptDialog : public JXWindowDirector, public JPrefObject
{
public:

	CBRunTEScriptDialog(JXDirector* supervisor);

	virtual ~CBRunTEScriptDialog();

	virtual void	Activate();

	JBoolean	RunScript();
	JBoolean	RunSimpleScript(const JCharacter* scriptName, JTextEditor* te,
								const JCharacter* fileName);

protected:

	virtual void	ReadPrefs(istream& input);
	virtual void	WritePrefs(ostream& output) const;

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

// begin JXLayout

	JXTextButton*        itsCloseButton;
	JXTextButton*        itsRunButton;
	JXInputField*        itsCmdInput;
	JXStringHistoryMenu* itsHistoryMenu;
	JXTextButton*        itsHelpButton;
	JXTextCheckbox*      itsStayOpenCB;

// end JXLayout

private:

	void		BuildWindow();
	void		UpdateDisplay();
	JBoolean	RunScript(JTextEditor* te, const JCharacter* fullName);
	JBoolean	RunScript(const JCharacter* cmd, JTextEditor* te,
						  const JCharacter* fullName);
	void		ReplaceVariables(JString* cmd, JTextEditor* te,
								 const JCharacter* fullName);

	// not allowed

	CBRunTEScriptDialog(const CBRunTEScriptDialog& source);
	const CBRunTEScriptDialog& operator=(const CBRunTEScriptDialog& source);
};

#endif
