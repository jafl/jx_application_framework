/******************************************************************************
 JXChooseFileDialog.h

	Interface for the JXChooseFileDialog class

	Copyright (C) 1996-98 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXChooseFileDialog
#define _H_JXChooseFileDialog

#include "JXCSFDialogBase.h"

class JXChooseFileDialog : public JXCSFDialogBase
{
public:

	static JXChooseFileDialog*
		Create(JXDirector* supervisor, JDirInfo* dirInfo,
			   const JString& fileFilter,
			   const bool allowSelectMultiple,
			   const JString& origName, const JString& message);

	virtual ~JXChooseFileDialog();

	bool	GetFullName(JString* fullName) const;
	bool	GetFullNames(JPtrArray<JString>* fullNameList) const;

protected:

	JXChooseFileDialog(JXDirector* supervisor, JDirInfo* dirInfo,
					   const JString& fileFilter, const bool allowSelectMultiple);

	void	SetObjects(JXScrollbarSet* scrollbarSet,
					   JXStaticText* pathLabel, JXPathInput* pathInput,
					   JXPathHistoryMenu* pathHistory,
					   JXStaticText* filterLabel, JXInputField* filterInput,
					   JXStringHistoryMenu* filterHistory,
					   JXTextButton* openButton, JXTextButton* cancelButton,
					   JXTextButton* upButton, JXTextButton* homeButton,
					   JXTextButton* desktopButton,
					   JXTextButton* selectAllButton, JXTextCheckbox* showHiddenCB,
					   JXCurrentPathMenu* currPathMenu,
					   const JString& origName, const JString& message);

	virtual void	AdjustSizings() override;
	virtual void	UpdateDisplay() override;

	virtual bool	OKToDeactivate() override;
	virtual void		Receive(JBroadcaster* sender, const Message& message) override;

private:

	const bool	itsSelectMultipleFlag;

	JXTextButton*	itsOpenButton;
	JXTextButton*	itsSelectAllButton;

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow(const JString& origName, const JString& message);

	// not allowed

	JXChooseFileDialog(const JXChooseFileDialog& source);
	const JXChooseFileDialog& operator=(const JXChooseFileDialog& source);
};

#endif
