/******************************************************************************
 JXFSRunScriptDialog.h

	Copyright (C) 2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXFSRunScriptDialog
#define _H_JXFSRunScriptDialog

#include <JXDialogDirector.h>
#include "JFSBinding.h"		// need defn of CommandType

class JString;
class JXInputField;
class JXTextCheckbox;

class JXFSRunScriptDialog : public JXDialogDirector
{
public:

	JXFSRunScriptDialog(const JString& cmd);

	virtual ~JXFSRunScriptDialog();

	virtual void	Activate() override;

	const JString&	GetCommand(JFSBinding::CommandType* type) const;

private:

// begin JXLayout

	JXInputField*   itsCmdInput;
	JXTextCheckbox* itsUseShellCB;
	JXTextCheckbox* itsUseWindowCB;

// end JXLayout

private:

	void	BuildWindow(const JString& cmd);
};

#endif
