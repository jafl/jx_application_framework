/******************************************************************************
 JXFSEditBindingsDialog.h

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_JXFSEditBindingsDialog
#define _H_JXFSEditBindingsDialog

#include <jx-af/jx/JXModalDialogDirector.h>
#include "JFSBinding.h"		// for CommandType

class JXTextButton;
class JXTextCheckbox;
class JXInputField;
class JFSBindingList;
class JXFSBindingTable;

class JXFSEditBindingsDialog : public JXWindowDirector
{
public:

	JXFSEditBindingsDialog(JFSBindingList* list);

	~JXFSEditBindingsDialog() override;

	void	CheckIfNeedRevert();
	void	AddBinding(const JString& suffix, const JString& cmd,
					   const JFSBinding::CommandType type,
					   const bool singleFile);

protected:

	bool	OKToDeactivate() override;
	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JFSBindingList*		itsBindingList;
	bool				itsSaveNeededFlag;
	JFSBindingList*		itsOrigBindingList;		// not owned

// begin JXLayout

	JXTextButton*     itsHelpButton;
	JXTextCheckbox*   itsUseDefaultCB;
	JXTextCheckbox*   itsDefShellCB;
	JXTextCheckbox*   itsDefWindowCB;
	JXTextCheckbox*   itsDefSingleCB;
	JXTextButton*     itsRevertButton;
	JXTextButton*     itsSaveButton;
	JXTextCheckbox*   itsAutoShellCB;
	JXTextButton*     itsCloseButton;
	JXFSBindingTable* itsTable;
	JXInputField*     itsDefCmd;
	JXInputField*     itsShellCmd;
	JXInputField*     itsWindowCmd;

// end JXLayout

private:

	void	BuildWindow();
	void	SaveNeeded();
	void	DataReverted();
	bool	Save(const bool askReplace);
	void	Revert(const bool updateList);
	bool	CheckData();

	bool	ReadSetup();
	void	WriteSetup() const;

	void	ReadSetup(std::istream& input);
	void	WriteSetup(std::ostream& output) const;
};

#endif
