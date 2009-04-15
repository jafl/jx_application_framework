/******************************************************************************
 JXFSRunScriptDialog.h

	Copyright © 2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXFSRunScriptDialog
#define _H_JXFSRunScriptDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDialogDirector.h>
#include "JFSBinding.h"		// need defn of CommandType

class JString;
class JXInputField;
class JXTextCheckbox;

class JXFSRunScriptDialog : public JXDialogDirector
{
public:

	JXFSRunScriptDialog(const JCharacter* cmd);

	virtual ~JXFSRunScriptDialog();

	virtual void	Activate();

	const JString&	GetCommand(JFSBinding::CommandType* type) const;

private:

// begin JXLayout

    JXInputField*   itsCmdInput;
    JXTextCheckbox* itsUseShellCB;
    JXTextCheckbox* itsUseWindowCB;

// end JXLayout

private:

	void	BuildWindow(const JCharacter* cmd);

	// not allowed

	JXFSRunScriptDialog(const JXFSRunScriptDialog& source);
	const JXFSRunScriptDialog& operator=(const JXFSRunScriptDialog& source);
};

#endif
