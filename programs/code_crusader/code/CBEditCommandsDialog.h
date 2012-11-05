/******************************************************************************
 CBEditCommandsDialog.h

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBEditCommandsDialog
#define _H_CBEditCommandsDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDialogDirector.h>
#include <JPrefObject.h>
#include "CBCommandManager.h"	// need defn of CmdInfo
#include <JString.h>

class JXTextButton;
class JXVertPartition;
class JXInputField;
class CBProjectDocument;
class CBCommandTable;

class CBEditCommandsDialog : public JXDialogDirector, public JPrefObject
{
public:

	CBEditCommandsDialog(CBProjectDocument* projDoc);

	virtual ~CBEditCommandsDialog();

protected:

	virtual void	ReadPrefs(istream& input);
	virtual void	WritePrefs(ostream& output) const;

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	CBCommandTable*		itsAllTable;
	CBCommandManager*	itsCmdMgr;			// can be NULL
	CBCommandTable*		itsThisTable;		// NULL if itsCmdMgr == NULL
	JString				itsThisTableSetup;	// used when itsThisTable == NULL

// begin JXLayout

	JXVertPartition* itsPartition;
	JXTextButton*    itsHelpButton;

// end JXLayout

// begin allProjectsLayout


// end allProjectsLayout

// begin thisProjectLayout

	JXInputField* itsMakeDependCmd;

// end thisProjectLayout

private:

	void	BuildWindow(CBProjectDocument* projDoc);

	// not allowed

	CBEditCommandsDialog(const CBEditCommandsDialog& source);
	const CBEditCommandsDialog& operator=(const CBEditCommandsDialog& source);
};

#endif
