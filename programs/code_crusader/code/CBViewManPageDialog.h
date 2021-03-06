/******************************************************************************
 CBViewManPageDialog.h

	Interface for the CBViewManPageDialog class

	Copyright (C) 1996-98 by John Lindal. All rights reserved.

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

	virtual void	Activate();

	// called by CBManPageDocument

	void	AddToHistory(const JCharacter* pageName, const JCharacter pageIndex,
						 const JBoolean apropos);

protected:

	virtual void	ReadPrefs(std::istream& input);
	virtual void	WritePrefs(std::ostream& output) const;

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

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
	void	SetFunction(const JCharacter* historyStr);

	// not allowed

	CBViewManPageDialog(const CBViewManPageDialog& source);
	const CBViewManPageDialog& operator=(const CBViewManPageDialog& source);
};

#endif
