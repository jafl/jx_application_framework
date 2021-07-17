/******************************************************************************
 CBRunTEScriptDialog.h

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBRunTEScriptDialog
#define _H_CBRunTEScriptDialog

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

	virtual void	Activate() override;

	bool	RunScript();
	bool	RunSimpleScript(const JString& scriptName, JTextEditor* te,
								const JString& fileName);

protected:

	virtual void	ReadPrefs(std::istream& input) override;
	virtual void	WritePrefs(std::ostream& output) const override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

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
	bool	RunScript(JTextEditor* te, const JString& fullName);
	bool	RunScript(const JString& cmd, JTextEditor* te,
						  const JString& fullName);
	void		ReplaceVariables(JString* cmd, JTextEditor* te,
								 const JString& fullName);

	// not allowed

	CBRunTEScriptDialog(const CBRunTEScriptDialog& source);
	const CBRunTEScriptDialog& operator=(const CBRunTEScriptDialog& source);
};

#endif
