/******************************************************************************
 JXSaveFileDialog.h

	Interface for the JXSaveFileDialog class

	Copyright (C) 1996 by John Lindal.

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
			   const JString& fileFilter,
			   const JString& origName, const JString& prompt,
			   const JString& message = JString::empty);

	virtual ~JXSaveFileDialog();

	virtual void	Activate() override;

	void		Save(const JString& path);
	JBoolean	GetFileName(JString* name) const;

protected:

	JXSaveFileDialog(JXDirector* supervisor, JDirInfo* dirInfo,
					 const JString& fileFilter);

	void	SetObjects(JXScrollbarSet* scrollbarSet,
					   JXStaticText* promptLabel, const JString& prompt,
					   JXSaveFileInput* fileNameInput, const JString& origName,
					   JXStaticText* pathLabel, JXPathInput* pathInput,
					   JXPathHistoryMenu* pathHistory,
					   JXStaticText* filterLabel, JXInputField* filterInput,
					   JXStringHistoryMenu* filterHistory,
					   JXTextButton* saveButton, JXTextButton* cancelButton,
					   JXTextButton* upButton, JXTextButton* homeButton,
					   JXTextButton* desktopButton,
					   JXNewDirButton* newDirButton, JXTextCheckbox* showHiddenCB,
					   JXCurrentPathMenu* currPathMenu, const JString& message);

	JXInputField*	GetFileNameInput();

	virtual void	UpdateDisplay() override;	// must call inherited

	virtual JBoolean	OKToDeactivate() override;
	virtual void		Receive(JBroadcaster* sender, const Message& message) override;

private:

	JString				itsFileName;
	JXDirectSaveSource*	itsXDSSource;

	JXTextButton*		itsSaveButton;
	JXSaveFileInput*	itsFileNameInput;

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow(const JString& origName, const JString& prompt,
						const JString& message);

	// not allowed

	JXSaveFileDialog(const JXSaveFileDialog& source);
	const JXSaveFileDialog& operator=(const JXSaveFileDialog& source);
};

#endif
