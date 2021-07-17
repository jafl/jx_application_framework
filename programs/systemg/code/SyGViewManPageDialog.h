/******************************************************************************
 SyGViewManPageDialog.h

	Interface for the SyGViewManPageDialog class

	Copyright (C) 1996-98 by John Lindal.

 ******************************************************************************/

#ifndef _H_SyGViewManPageDialog
#define _H_SyGViewManPageDialog

#include <JXWindowDirector.h>
#include <JPrefObject.h>
#include <JString.h>

class JXTextButton;
class JXInputField;
class JXCharInput;
class JXTextCheckbox;
class JXStringHistoryMenu;

class SyGViewManPageDialog : public JXWindowDirector, public JPrefObject
{
public:

	SyGViewManPageDialog(JXDirector* supervisor);

	virtual ~SyGViewManPageDialog();

	virtual void	Activate() override;

	void	ViewManPage(const JString& item, const JUtf8Character& index = JUtf8Character(' '),
						const bool apropos = false);
	void	ViewManPages(const JPtrArray<JString>& list);

	static const JString&	GetViewManPageCommand();
	static void				SetViewManPageCommand(const JString& cmd);

protected:

	virtual void	ReadPrefs(std::istream& input) override;
	virtual void	WritePrefs(std::ostream& output) const override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	static JString	itsViewCmd;		// static so SyGPrefsMgr can set it

// begin JXLayout

	JXTextButton*        itsCloseButton;
	JXTextButton*        itsViewButton;
	JXInputField*        itsFnName;
	JXTextCheckbox*      itsAproposCheckbox;
	JXTextButton*        itsHelpButton;
	JXStringHistoryMenu* itsFnHistoryMenu;
	JXCharInput*         itsManIndex;
	JXTextCheckbox*      itsStayOpenCB;

// end JXLayout

private:

	void	BuildWindow();
	void	UpdateDisplay();
	void	ViewManPage();
	void	SetFunction(const JString& historyStr);
	void	AddToHistory(const JString& pageName, const JUtf8Character& pageIndex,
						 const bool apropos);

	// not allowed

	SyGViewManPageDialog(const SyGViewManPageDialog& source);
	const SyGViewManPageDialog& operator=(const SyGViewManPageDialog& source);
};


/******************************************************************************
 View man page command (static)

 ******************************************************************************/

inline const JString&
SyGViewManPageDialog::GetViewManPageCommand()
{
	return itsViewCmd;
}

inline void
SyGViewManPageDialog::SetViewManPageCommand
	(
	const JString& cmd
	)
{
	itsViewCmd = cmd;
	itsViewCmd.TrimWhitespace();
}

#endif
