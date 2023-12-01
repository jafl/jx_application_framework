/******************************************************************************
 ImportDialog.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_ImportDialog
#define _H_ImportDialog

#include <jx-af/jx/JXModalDialogDirector.h>

class JXTextButton;
class JXInputField;
class JXFileInput;

class ImportDialog : public JXModalDialogDirector
{
public:

	ImportDialog();

	~ImportDialog() override;

	const JString&	GetMenuText() const;
	const JString&	GetEnumText() const;
	JString			GetActionDefsFile() const;

private:

// begin JXLayout

	JXInputField* itsMenuText;
	JXInputField* itsMenuEnum;
	JXTextButton* itsHelpButton;
	JXFileInput*  itsActionDefsFileInput;
	JXTextButton* itsChooseActionDefsFileButton;

// end JXLayout

private:

	void	BuildWindow();
};

#endif
