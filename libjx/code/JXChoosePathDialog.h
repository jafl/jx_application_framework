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

	static JXChoosePathDialog*
		Create(JXDirector* supervisor, JDirInfo* dirInfo,
			   const JString& fileFilter,
			   const bool selectOnlyWritable,
			   const JString& message = JString::empty);

	virtual ~JXChoosePathDialog();

protected:

	JXChoosePathDialog(JXDirector* supervisor, JDirInfo* dirInfo,
					   const JString& fileFilter,
					   const bool selectOnlyWritable);

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
					   JXCurrentPathMenu* currPathMenu, const JString& message);

	virtual void	UpdateDisplay() override;	// must call inherited

	virtual bool	OKToDeactivate() override;
	virtual void		Receive(JBroadcaster* sender, const Message& message) override;

private:

	const bool	itsSelectOnlyWritableFlag;

	JXTextButton*	itsOpenButton;
	JXTextButton*	itsSelectButton;

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow(const JString& message);

	// not allowed

	JXChoosePathDialog(const JXChoosePathDialog& source);
	const JXChoosePathDialog& operator=(const JXChoosePathDialog& source);
};

#endif
