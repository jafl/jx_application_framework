/******************************************************************************
 JXEditWWWPrefsDialog.h

	Interface for the JXEditWWWPrefsDialog class

	Copyright (C) 1998-2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXEditWWWPrefsDialog
#define _H_JXEditWWWPrefsDialog

#include "JXDialogDirector.h"

class JString;
class JXInputField;

class JXEditWWWPrefsDialog : public JXDialogDirector
{
public:

	JXEditWWWPrefsDialog(JXDirector* supervisor,
						 const JString& showURLCmd,
						 const JString& showFileContentCmd,
						 const JString& showFileLocationCmd,
						 const JString& composeMailCmd);

	virtual ~JXEditWWWPrefsDialog();

	void	GetPrefs(JString* showURLCmd, JString* showFileContentCmd,
					 JString* showFileLocationCmd, JString* composeMailCmd) const;

private:

// begin JXLayout

	JXInputField* itsShowURLCmdInput;
	JXInputField* itsShowFileContentCmdInput;
	JXInputField* itsShowFileLocationCmdInput;
	JXInputField* itsComposeMailCmdInput;

// end JXLayout

private:

	void	BuildWindow(const JString& showURLCmd,
						const JString& showFileContentCmd,
						const JString& showFileLocationCmd,
						const JString& composeMailCmd);

	// not allowed

	JXEditWWWPrefsDialog(const JXEditWWWPrefsDialog& source);
	const JXEditWWWPrefsDialog& operator=(const JXEditWWWPrefsDialog& source);
};

#endif
