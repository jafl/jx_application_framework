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
						const JString& containerName, const JString& codeTag,
						const bool adjustToFit);

	~LayoutConfigDialog() override;

	void	GetConfig(JString* windowTitle, JString* containerName,
					  JString* codeTag, bool* adjustToFit) const;

private:

// begin JXLayout

	JXTextCheckbox* itsWindowTitleCB;
	JXTextCheckbox* itsCustomContainerCB;
	JXInputField*   itsWindowTitleInput;
	JXInputField*   itsContainerInput;
	JXInputField*   itsCodeTagInput;
	JXTextCheckbox* itsAdjustContentCB;

// end JXLayout

private:

	void	BuildWindow(const JString& windowTitle,
						const JString& containerName, const JString& codeTag,
						const bool adjustToFit);

	void UpdateDisplay();
};

#endif
