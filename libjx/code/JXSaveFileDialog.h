/******************************************************************************
 JXSaveFileDialog.h

	Interface for the JXSaveFileDialog class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXSaveFileDialog
#define _H_JXSaveFileDialog

#include "JXCSFDialogBase.h"

class JXSaveFileInput;
class JXDirectSaveSource;

class JXSaveFileDialog : public JXCSFDialogBase
{
public:

	static JXSaveFileDialog*
		Create(const JString& prompt,
			   const JString& startName = JString::empty,
			   const JString& fileFilter = JString::empty,
			   const JString& message = JString::empty);

	~JXSaveFileDialog() override;

	const JString&	GetFileName() const;
	JString			GetFullName() const;

	void	Activate() override;

	void	Save(const JString& path);

protected:

	JXSaveFileDialog(const JString& fileFilter);

	void	SetObjects(JXScrollbarSet* scrollbarSet,
					   JXStaticText* promptLabel, const JString& prompt,
					   JXSaveFileInput* fileNameInput,
					   JXStaticText* pathLabel, JXPathInput* pathInput,
					   JXPathHistoryMenu* pathHistory,
					   JXStaticText* filterLabel, JXInputField* filterInput,
					   JXStringHistoryMenu* filterHistory,
					   JXTextButton* saveButton, JXTextButton* cancelButton,
					   JXTextButton* upButton, JXTextButton* homeButton,
					   JXTextButton* desktopButton,
					   JXNewDirButton* newDirButton, JXTextCheckbox* showHiddenCB,
					   JXCurrentPathMenu* currPathMenu,
					   const JString& startName, const JString& message);

	JXInputField*	GetFileNameInput();

	void	UpdateDisplay() override;	// must call inherited

	bool	OKToDeactivate() override;
	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JString				itsFileName;
	JXDirectSaveSource*	itsXDSSource;

	JXTextButton*		itsSaveButton;
	JXSaveFileInput*	itsFileNameInput;

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow(const JString& startName,
						const JString& prompt, const JString& message);
};

#endif
