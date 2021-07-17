/******************************************************************************
 CBViewManPageDialog.h

	Interface for the CBViewManPageDialog class

	Copyright © 1996-98 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBViewManPageDialog
#define _H_CBViewManPageDialog

#include <JXWindowDirector.h>
#include <JPrefObject.h>

class JXTextButton;
class JXInputField;
class JXCharInput;
class JXTextCheckbox;
class JXStringHistoryMenu;

class CBViewManPageDialog : public JXWindowDirector, public JPrefObject
{
public:

	CBViewManPageDialog(JXDirector* supervisor);

	virtual ~CBViewManPageDialog();

	virtual void	Activate() override;

	// called by CBManPageDocument

	void	AddToHistory(const JString& pageName, const JString& pageIndex,
						 const bool apropos);

protected:

	virtual void	ReadPrefs(std::istream& input) override;
	virtual void	WritePrefs(std::ostream& output) const override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

// begin JXLayout

	JXTextButton*        itsCloseButton;
	JXTextButton*        itsViewButton;
	JXInputField*        itsFnName;
	JXTextCheckbox*      itsAproposCheckbox;
	JXTextButton*        itsHelpButton;
	JXStringHistoryMenu* itsFnHistoryMenu;
	JXTextCheckbox*      itsStayOpenCB;
	JXInputField*        itsManIndex;

// end JXLayout

private:

	void	BuildWindow();
	void	UpdateDisplay();
	void	ViewManPage();
	void	SetFunction(const JString& historyStr);

	// not allowed

	CBViewManPageDialog(const CBViewManPageDialog& source);
	const CBViewManPageDialog& operator=(const CBViewManPageDialog& source);
};

#endif
