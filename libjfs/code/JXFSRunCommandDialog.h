/******************************************************************************
 JXFSRunCommandDialog.h

	Copyright (C) 2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXFSRunCommandDialog
#define _H_JXFSRunCommandDialog

#include <JXWindowDirector.h>
#include <jTime.h>

class JXInputField;
class JXTextButton;
class JXTextCheckbox;
class JXPathInput;
class JXPathHistoryMenu;
class JXFSCommandHistoryMenu;
class JXCheckModTimeTask;

class JXFSRunCommandDialog : public JXWindowDirector
{
public:

	JXFSRunCommandDialog();

	virtual ~JXFSRunCommandDialog();

	virtual void	Activate() override;

	void	SetPath(const JCharacter* path);

	JBoolean	ReadSetup(const JBoolean update);
	void		WriteSetup();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXCheckModTimeTask*	itsSignalTask;	// can be NULL

// begin JXLayout

	JXInputField*           itsCmdInput;
	JXTextButton*           itsCloseButton;
	JXTextButton*           itsHelpButton;
	JXTextButton*           itsRunButton;
	JXFSCommandHistoryMenu* itsCmdHistoryMenu;
	JXTextButton*           itsChooseCmdButton;
	JXTextCheckbox*         itsUseShellCB;
	JXTextCheckbox*         itsUseWindowCB;
	JXTextCheckbox*         itsStayOpenCB;
	JXTextButton*           itsChoosePathButton;
	JXPathInput*            itsPathInput;
	JXPathHistoryMenu*      itsPathHistoryMenu;

// end JXLayout

private:

	void	BuildWindow();
	void	UpdateDisplay();
	void	Exec();

	void	ReadSetup(std::istream& input);
	void	UpdateSetup(std::istream& input);
	void	WriteSetup(std::ostream& output) const;

	// not allowed

	JXFSRunCommandDialog(const JXFSRunCommandDialog& source);
	const JXFSRunCommandDialog& operator=(const JXFSRunCommandDialog& source);
};

#endif
