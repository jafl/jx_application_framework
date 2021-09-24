/******************************************************************************
 JXFSRunFileDialog.h

	Copyright (C) 2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXFSRunFileDialog
#define _H_JXFSRunFileDialog

#include <JXDialogDirector.h>
#include "JFSBinding.h"		// need defn of CommandType

class JString;
class JXInputField;
class JXTextButton;
class JXTextCheckbox;
class JXFSCommandHistoryMenu;

class JXFSRunFileDialog : public JXDialogDirector
{
public:

	JXFSRunFileDialog(const JString& fileName, const bool allowSaveCmd);

	virtual ~JXFSRunFileDialog();

	const JString&	GetCommand(JFSBinding::CommandType* type,
							   bool* singleFile, bool* saveBinding) const;

	static void	HandleHistoryMenu(const Message& message, JXFSCommandHistoryMenu* menu,
								  JXInputField* cmdInput, JXTextCheckbox* shellCB,
								  JXTextCheckbox* windowCB, JXTextCheckbox* singleFileCB);
	static void	HandleChooseCmdButton(JXInputField* cmdInput);

protected:

	virtual void		Receive(JBroadcaster* sender, const Message& message) override;
	virtual bool	OKToDeactivate() override;

private:

// begin JXLayout

	JXInputField*           itsCmdInput;
	JXTextButton*           itsHelpButton;
	JXTextButton*           itsOKButton;
	JXFSCommandHistoryMenu* itsCmdHistoryMenu;
	JXTextButton*           itsChooseCmdButton;
	JXTextCheckbox*         itsSaveBindingCB;
	JXTextCheckbox*         itsUseShellCB;
	JXTextCheckbox*         itsUseWindowCB;
	JXTextCheckbox*         itsSingleFileCB;

// end JXLayout

private:

	void		BuildWindow(const JString& fileName, const bool allowSaveCmd);
	void		UpdateDisplay();

	bool	ReadSetup();
	void		WriteSetup();

	void		ReadSetup(std::istream& input);
	void		WriteSetup(std::ostream& output) const;
};

#endif
