/******************************************************************************
 JXChoosePathDialog.h

	Interface for the JXChoosePathDialog class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXChoosePathDialog
#define _H_JXChoosePathDialog

#include <JXCSFDialogBase.h>

class JXChoosePathDialog : public JXCSFDialogBase
{
public:

	static JXChoosePathDialog*
		Create(JXDirector* supervisor, JDirInfo* dirInfo,
			   const JCharacter* fileFilter,
			   const JBoolean selectOnlyWritable,
			   const JCharacter* message = NULL);

	virtual ~JXChoosePathDialog();

protected:

	JXChoosePathDialog(JXDirector* supervisor, JDirInfo* dirInfo,
					   const JCharacter* fileFilter,
					   const JBoolean selectOnlyWritable);

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
					   JXCurrentPathMenu* currPathMenu, const JCharacter* message);

	virtual void	UpdateDisplay();	// must call inherited

	virtual JBoolean	OKToDeactivate();
	virtual void		Receive(JBroadcaster* sender, const Message& message);

private:

	const JBoolean	itsSelectOnlyWritableFlag;

	JXTextButton*	itsOpenButton;
	JXTextButton*	itsSelectButton;

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow(const JCharacter* message = NULL);

	// not allowed

	JXChoosePathDialog(const JXChoosePathDialog& source);
	const JXChoosePathDialog& operator=(const JXChoosePathDialog& source);
};

#endif
