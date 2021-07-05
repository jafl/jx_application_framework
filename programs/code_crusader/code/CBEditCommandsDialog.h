/******************************************************************************
 CBEditCommandsDialog.h

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBEditCommandsDialog
#define _H_CBEditCommandsDialog

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

	virtual void	ReadPrefs(std::istream& input) override;
	virtual void	WritePrefs(std::ostream& output) const override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	CBCommandTable*		itsAllTable;
	CBCommandManager*	itsCmdMgr;			// can be nullptr
	CBCommandTable*		itsThisTable;		// nullptr if itsCmdMgr == nullptr
	JString				itsThisTableSetup;	// used when itsThisTable == nullptr

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
