/******************************************************************************
 JXWMainDialog.h

	Copyright (C) 2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXWMainDialog
#define _H_JXWMainDialog

#include <JXWindowDirector.h>
#include <JPrefObject.h>

class JXTextButton;
class JXInputField;
class JXPathInput;
class JXTextCheckbox;
class JXPathHistoryMenu;

class JXWMainDialog : public JXWindowDirector, public JPrefObject
{
public:

	JXWMainDialog(JXDirector* supervisor, const JSize argc, char** argv);

	virtual ~JXWMainDialog();

protected:

	virtual void	ReadPrefs(std::istream& input) override;
	virtual void	WritePrefs(std::ostream& output) const override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

// begin JXLayout

	JXTextButton*      itsCreateButton;
	JXTextButton*      itsQuitButton;
	JXTextButton*      itsHelpButton;
	JXInputField*      itsProgramName;
	JXInputField*      itsProgramVersion;
	JXInputField*      itsDevName;
	JXInputField*      itsDevEmail;
	JXTextCheckbox*    itsNeedsMDICB;
	JXTextButton*      itsChooseProjDirButton;
	JXTextButton*      itsChooseTmplDirButton;
	JXInputField*      itsCompanyName;
	JXInputField*      itsDevURL;
	JXInputField*      itsBinaryName;
	JXInputField*      itsSrcPrefix;
	JXPathInput*       itsProjectDir;
	JXPathInput*       itsTemplateDir;
	JXPathHistoryMenu* itsTmplDirHistory;
	JXInputField*      itsOpenCmd;

// end JXLayout

private:

	void		BuildWindow(const JSize argc, char** argv);
	bool	WriteTemplate();
	void		CopyAndAdjustTemplateFiles(const JString& sourceDir,
										   const JString& targetDir,
										   const JPtrArray<JString>& value);

	void	UpdatePath(JString* s);

	// not allowed

	JXWMainDialog(const JXWMainDialog& source);
	const JXWMainDialog& operator=(const JXWMainDialog& source);
};

#endif
