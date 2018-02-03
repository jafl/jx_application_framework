/******************************************************************************
 JXFSEditBindingsDialog.h

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_JXFSEditBindingsDialog
#define _H_JXFSEditBindingsDialog

#include <JXDialogDirector.h>
#include "JFSBinding.h"		// need defn of CommandType

class JXTextButton;
class JXTextCheckbox;
class JXInputField;
class JFSBindingList;
class JXFSBindingTable;

class JXFSEditBindingsDialog : public JXDialogDirector
{
public:

	JXFSEditBindingsDialog(JFSBindingList* list);

	virtual	~JXFSEditBindingsDialog();

	void	CheckIfNeedRevert();
	void	AddBinding(const JCharacter* suffix, const JCharacter* cmd,
					   const JFSBinding::CommandType type,
					   const JBoolean singleFile);

protected:

	virtual JBoolean	OKToDeactivate();
	virtual void		Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXFSBindingTable*	itsTable;
	JFSBindingList*		itsBindingList;
	JBoolean			itsNeedsSaveFlag;
	JFSBindingList*		itsOrigBindingList;		// not owned

// begin JXLayout

	JXTextButton*   itsSaveButton;
	JXTextButton*   itsCloseButton;
	JXTextButton*   itsRevertButton;
	JXTextButton*   itsHelpButton;
	JXInputField*   itsDefCmd;
	JXTextCheckbox* itsUseDefaultCB;
	JXInputField*   itsShellCmd;
	JXInputField*   itsWindowCmd;
	JXTextCheckbox* itsDefShellCB;
	JXTextCheckbox* itsDefWindowCB;
	JXTextCheckbox* itsDefSingleCB;
	JXTextCheckbox* itsAutoShellCB;

// end JXLayout

private:

	void		BuildWindow();
	void		NeedsSave();
	void		DataReverted();
	JBoolean	Save(const JBoolean askReplace);
	void		Revert(const JBoolean updateList);
	JBoolean	CheckData();

	JBoolean	ReadSetup();
	void		WriteSetup();

	void		ReadSetup(std::istream& input);
	void		WriteSetup(std::ostream& output);

	// not allowed

	JXFSEditBindingsDialog(const JXFSEditBindingsDialog& source);
	const JXFSEditBindingsDialog& operator=(const JXFSEditBindingsDialog& source);
};

#endif
