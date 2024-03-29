/******************************************************************************
 JXFSRunFileDialog.h

	Copyright (C) 2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXFSRunFileDialog
#define _H_JXFSRunFileDialog

#include <jx-af/jx/JXModalDialogDirector.h>
#include "JFSBinding.h"		// for CommandType

class JString;
class JXInputField;
class JXTextButton;
class JXTextCheckbox;
class JXFSCommandHistoryMenu;

class JXFSRunFileDialog : public JXModalDialogDirector
{
public:

	JXFSRunFileDialog(const JString& fileName, const bool allowSaveCmd);

	~JXFSRunFileDialog() override;

	const JString&	GetCommand(JFSBinding::CommandType* type,
							   bool* singleFile, bool* saveBinding) const;

	static void	HandleHistoryMenu(const Message& message, JXFSCommandHistoryMenu* menu,
								  JXInputField* cmdInput, JXTextCheckbox* shellCB,
								  JXTextCheckbox* windowCB, JXTextCheckbox* singleFileCB);
	static void	HandleChooseCmdButton(JXInputField* cmdInput);

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;
	bool	OKToDeactivate() override;

private:

// begin JXLayout

	JXFSCommandHistoryMenu* itsCmdHistoryMenu;
	JXTextButton*           itsChooseCmdButton;
	JXTextCheckbox*         itsUseShellCB;
	JXTextButton*           itsHelpButton;
	JXTextButton*           itsOKButton;
	JXTextCheckbox*         itsUseWindowCB;
	JXTextCheckbox*         itsSingleFileCB;
	JXTextCheckbox*         itsSaveBindingCB;
	JXInputField*           itsCmdInput;

// end JXLayout

private:

	void	BuildWindow(const JString& fileName, const bool allowSaveCmd);
	void	UpdateDisplay();

	bool	ReadSetup();
	void	WriteSetup() const;

	void	ReadSetup(std::istream& input);
	void	WriteSetup(std::ostream& output) const;
};

#endif
