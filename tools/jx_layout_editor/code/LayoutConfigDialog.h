/******************************************************************************
 LayoutConfigDialog.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_LayoutConfigDialog
#define _H_LayoutConfigDialog

#include <jx-af/jx/JXModalDialogDirector.h>

class JXInputField;
class JXTextCheckbox;

class LayoutConfigDialog : public JXModalDialogDirector
{
public:

	LayoutConfigDialog(const JString& windowTitle,
						const JString& containerName, const JString& codeTag);

	~LayoutConfigDialog() override;

	void	GetConfig(JString* windowTitle, JString* containerName, JString* codeTag) const;

private:

// begin JXLayout

	JXTextCheckbox* itsWindowTitleCB;
	JXTextCheckbox* itsCustomContainerCB;
	JXInputField*   itsWindowTitleInput;
	JXInputField*   itsContainerInput;
	JXInputField*   itsCodeTagInput;

// end JXLayout

private:

	void	BuildWindow(const JString& windowTitle,
						const JString& containerName, const JString& codeTag);

	void UpdateDisplay();
};

#endif
