/******************************************************************************
 LayoutConfigDialog.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_LayoutConfigDialog
#define _H_LayoutConfigDialog

#include <jx-af/jx/JXModalDialogDirector.h>

class JXInputField;
class JXIntegerInput;
class JXTextCheckbox;
class JXRadioGroup;
class JXTextRadioButton;

class LayoutConfigDialog : public JXModalDialogDirector
{
public:

	LayoutConfigDialog(const JString& codeTag,
						const JString& windowTitle, const JString& xwmClass,
						const JSize minWidth, const JSize minHeight,
						const JString& containerName, const bool adjustToFit);

	~LayoutConfigDialog() override;

	void	GetConfig(JString* codeTag,
					  JString* windowTitle, JString* xwmClass,
					  JSize* minWidth, JSize* minHeight,
					  JString* containerName, bool* adjustToFit) const;

protected:

	bool	OKToDeactivate() override;

private:

// begin JXLayout

	JXRadioGroup*      itsLayoutTypeRG;
	JXTextRadioButton* itsWindowTitleRB;
	JXTextRadioButton* itsCustomContainerRB;
	JXTextCheckbox*    itsAdjustContentCB;
	JXInputField*      itsCodeTagInput;
	JXInputField*      itsWindowTitleInput;
	JXIntegerInput*    itsMinWindowWidthInput;
	JXIntegerInput*    itsMinWindowHeightInput;
	JXInputField*      itsXResourceClass;
	JXInputField*      itsContainerInput;

// end JXLayout

private:

	void	BuildWindow(const JString& codeTag,
						const JString& windowTitle, const JString& xwmClass,
						const JSize minWidth, const JSize minHeight,
						const JString& containerName, const bool adjustToFit);

	void UpdateDisplay();
};

#endif
