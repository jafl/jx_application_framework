/******************************************************************************
 MainDialog.h

	Copyright (C) 2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_MainDialog
#define _H_MainDialog

#include <jx-af/jx/JXWindowDirector.h>
#include <jx-af/jcore/JPrefObject.h>

class JXTextButton;
class JXInputField;
class JXPathInput;
class JXTextCheckbox;
class JXPathHistoryMenu;

class MainDialog : public JXWindowDirector, public JPrefObject
{
public:

	MainDialog(JXDirector* supervisor, const JSize argc, char** argv);

	~MainDialog() override;

protected:

	void	ReadPrefs(std::istream& input) override;
	void	WritePrefs(std::ostream& output) const override;

private:

// begin JXLayout

	JXTextCheckbox*    itsNeedsMDICB;
	JXTextButton*      itsChooseProjDirButton;
	JXTextButton*      itsChooseTmplDirButton;
	JXPathHistoryMenu* itsTmplDirHistory;
	JXTextButton*      itsQuitButton;
	JXTextButton*      itsHelpButton;
	JXTextButton*      itsCreateButton;
	JXInputField*      itsProgramName;
	JXInputField*      itsDevName;
	JXInputField*      itsDevEmail;
	JXInputField*      itsCompanyName;
	JXInputField*      itsDevURL;
	JXInputField*      itsBinaryName;
	JXPathInput*       itsProjectDir;
	JXPathInput*       itsTemplateDir;
	JXInputField*      itsOpenCmd;

// end JXLayout

private:

	void	BuildWindow(const JSize argc, char** argv);
	bool	WriteTemplate();
	void	CopyAndAdjustTemplateFiles(const JString& sourceDir,
									   const JString& targetDir,
									   const JPtrArray<JString>& value);

	void	UpdatePath(JString* s);
};

#endif
