/******************************************************************************
 JXChoosePathDialog.h

	Interface for the JXChoosePathDialog class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXChoosePathDialog
#define _H_JXChoosePathDialog

#include "JXCSFDialogBase.h"

class JXChoosePathDialog : public JXCSFDialogBase
{
public:

	enum SelectPathType		// supports cast from bool
	{
		kAcceptReadable = 0,
		kRequireWritable
	};

public:

	static JXChoosePathDialog*
		Create(const SelectPathType type,
			   const JString& startPath = JString::empty,
			   const JString& fileFilter = JString::empty,
			   const JString& message = JString::empty);

	~JXChoosePathDialog() override;

protected:

	JXChoosePathDialog(const SelectPathType type,
					   const JString& fileFilter);

	void	SetObjects(JXScrollbarSet* scrollbarSet,
					   JXStaticText* pathLabel, JXPathInput* pathInput,
					   JXPathHistoryMenu* pathHistory,
					   JXStaticText* filterLabel, JXInputField* filterInput,
					   JXStringHistoryMenu* filterHistory,
					   JXTextButton* openButton, JXTextButton* selectButton,
					   JXTextButton* cancelButton,
					   JXTextButton* upButton, JXTextButton* homeButton,
					   JXTextButton* desktopButton,
					   JXNewDirButton* newDirButton, JXTextCheckbox* showHiddenCB,
					   JXCurrentPathMenu* currPathMenu,
					   const JString& startPath, const JString& message);

	void	UpdateDisplay() override;	// must call inherited

	bool	OKToDeactivate() override;
	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	const SelectPathType	itsSelectPathType;

	JXTextButton*	itsOpenButton;
	JXTextButton*	itsSelectButton;

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow(const JString& startPath, const JString& message);
};

#endif
