/******************************************************************************
 JXWMainDialog.h

	Copyright © 2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXWMainDialog
#define _H_JXWMainDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

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

	virtual void	ReadPrefs(istream& input);
	virtual void	WritePrefs(ostream& output) const;

	virtual void	Receive(JBroadcaster* sender, const Message& message);

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
	JBoolean	WriteTemplate();
	void		CopyAndAdjustTemplateFiles(const JCharacter* sourceDir,
										   const JCharacter* targetDir,
										   const JCharacter** value);

	void	UpdatePath(JString* s);

	// not allowed

	JXWMainDialog(const JXWMainDialog& source);
	const JXWMainDialog& operator=(const JXWMainDialog& source);
};

#endif
