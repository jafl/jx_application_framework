/******************************************************************************
 JXFSRunScriptDialog.h

	Copyright (C) 2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXFSRunScriptDialog
#define _H_JXFSRunScriptDialog

#include <jx-af/jx/JXModalDialogDirector.h>
#include "JFSBinding.h"		// for CommandType

class JString;
class JXInputField;
class JXTextCheckbox;

class JXFSRunScriptDialog : public JXModalDialogDirector
{
public:

	JXFSRunScriptDialog(const JString& cmd);

	~JXFSRunScriptDialog() override;

	void	Activate() override;

	const JString&	GetCommand(JFSBinding::CommandType* type) const;

private:

// begin JXLayout

	JXTextCheckbox* itsUseShellCB;
	JXTextCheckbox* itsUseWindowCB;
	JXInputField*   itsCmdInput;

// end JXLayout

private:

	void	BuildWindow(const JString& cmd);
};

#endif
