/******************************************************************************
 JXSaveFileDialog.h

	Interface for the JXSaveFileDialog class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXSaveFileDialog
#define _H_JXSaveFileDialog

#include <JXCSFDialogBase.h>

class JXSaveFileInput;
class JXDirectSaveSource;

class JXSaveFileDialog : public JXCSFDialogBase
{
public:

	static JXSaveFileDialog*
		Create(JXDirector* supervisor, JDirInfo* dirInfo,
			   const JCharacter* fileFilter,
			   const JCharacter* origName, const JCharacter* prompt,
			   const JCharacter* message = NULL);

	virtual ~JXSaveFileDialog();

	virtual void	Activate();

	void		Save(const JCharacter* path);
	JBoolean	GetFileName(JString* name) const;

protected:

	JXSaveFileDialog(JXDirector* supervisor, JDirInfo* dirInfo,
					 const JCharacter* fileFilter);

	void	SetObjects(JXScrollbarSet* scrollbarSet,
					   JXStaticText* promptLabel, const JCharacter* prompt,
					   JXSaveFileInput* fileNameInput, const JCharacter* origName,
					   JXStaticText* pathLabel, JXPathInput* pathInput,
					   JXPathHistoryMenu* pathHistory,
					   JXStaticText* filterLabel, JXInputField* filterInput,
					   JXStringHistoryMenu* filterHistory,
					   JXTextButton* saveButton, JXTextButton* cancelButton,
					   JXTextButton* upButton, JXTextButton* homeButton,
					   JXTextButton* desktopButton,
					   JXNewDirButton* newDirButton, JXTextCheckbox* showHiddenCB,
					   JXCurrentPathMenu* currPathMenu, const JCharacter* message);

	JXInputField*	GetFileNameInput();

	virtual void	UpdateDisplay();	// must call inherited

	virtual JBoolean	OKToDeactivate();
	virtual void		Receive(JBroadcaster* sender, const Message& message);

private:

	JString*			itsFileName;
	JXDirectSaveSource*	itsXDSSource;

	JXTextButton*		itsSaveButton;
	JXSaveFileInput*	itsFileNameInput;

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow(const JCharacter* origName, const JCharacter* prompt,
						const JCharacter* message = NULL);

	// not allowed

	JXSaveFileDialog(const JXSaveFileDialog& source);
	const JXSaveFileDialog& operator=(const JXSaveFileDialog& source);
};

#endif
