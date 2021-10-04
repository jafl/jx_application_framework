/******************************************************************************
 JXFSRunCommandDialog.h

	Copyright (C) 2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXFSRunCommandDialog
#define _H_JXFSRunCommandDialog

#include <jx-af/jx/JXWindowDirector.h>
#include <jx-af/jcore/jTime.h>

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

	void	Activate() override;

	void	SetPath(const JString& path);

	bool	ReadSetup(const bool update);
	void		WriteSetup();

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXCheckModTimeTask*	itsSignalTask;	// can be nullptr

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
};

#endif
