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

	enum SelectType		// supports cast from bool
	{
		kSelectSingleFile = 0,
		kSelectMultipleFiles
	};

public:

	static JXChooseFileDialog*
		Create(const SelectType type = kSelectSingleFile,
			   const JString& selectName = JString::empty,
			   const JString& fileFilter = JString::empty,
			   const JString& message = JString::empty);

	~JXChooseFileDialog() override;

	const JString&	GetFullName() const;
	void			GetFullNames(JPtrArray<JString>* fullNameList) const;

protected:

	JXChooseFileDialog(const JString& fileFilter);

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
					   const SelectType type, const JString& selectName,
					   const JString& message);

	void	AdjustSizings() override;
	void	UpdateDisplay() override;

	bool	OKToDeactivate() override;
	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXTextButton*	itsOpenButton;
	JXTextButton*	itsSelectAllButton;

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow(const SelectType type, const JString& selectName,
						const JString& message);
};

#endif
