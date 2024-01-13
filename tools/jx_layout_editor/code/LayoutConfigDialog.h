/******************************************************************************
 LayoutConfigDialog.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_LayoutConfigDialog
#define _H_LayoutConfigDialog

#include <jx-af/jx/JXModalDialogDirector.h>

class JXInputField;
class JXTextCheckbox;
class JXRadioGroup;
class JXTextRadioButton;

class LayoutConfigDialog : public JXModalDialogDirector
{
public:

	LayoutConfigDialog(const JString& codeTag, const JString& windowTitle,
						const JString& containerName, const bool adjustToFit);

	~LayoutConfigDialog() override;

	void	GetConfig(JString* codeTag, JString* windowTitle,
					  JString* containerName, bool* adjustToFit) const;

private:

// begin JXLayout

	JXRadioGroup*      itsLayoutTypeRG;
	JXInputField*      itsCodeTagInput;
	JXTextRadioButton* itsWindowTitleRB;
	JXTextRadioButton* itsCustomContainerRB;
	JXInputField*      itsWindowTitleInput;
	JXInputField*      itsContainerInput;
	JXTextCheckbox*    itsAdjustContentCB;

// end JXLayout

private:

	void	BuildWindow(const JString& codeTag, const JString& windowTitle,
						const JString& containerName, const bool adjustToFit);

	void UpdateDisplay();
};

#endif
