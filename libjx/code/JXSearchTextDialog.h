/******************************************************************************
 JXSearchTextDialog.h

	Copyright © 1996-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXSearchTextDialog
#define _H_JXSearchTextDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWindowDirector.h>
#include <X11/X.h>

class JString;
class JRegex;
class JXTEBase;
class JXTextButton;
class JXTextMenu;
class JXInputField;
class JXTextCheckbox;
class JXStringHistoryMenu;
class JXTimerTask;
class JXSearchTextButton;

class JXSearchTextDialog : public JXWindowDirector
{
public:

	enum
	{
		kHistoryLength = 20,

		// XSearch

		kCurrentXSearchVersion = 1
	};

public:

	static JXSearchTextDialog*	Create(const JCharacter* searchTextHelpName = NULL);

	virtual ~JXSearchTextDialog();

	virtual void		Activate();
	virtual JBoolean	Deactivate();

	JBoolean	HasActiveTE() const;
	JBoolean	GetActiveTE(JXTEBase** te) const;
	void		SetActiveTE(JXTEBase* te);
	void		TEDeactivated(JXTEBase* te);

	JBoolean	HasSearchText() const;
	void		SetSearchText(const JCharacter* str);

	JBoolean	HasReplaceText() const;
	void		SetReplaceText(const JCharacter* str);

	JBoolean	GetSearchParameters(JString* searchStr, JBoolean* searchIsRegex,
									JBoolean* caseSensitive, JBoolean* entireWord,
									JBoolean* wrapSearch,
									JString* replaceStr, JBoolean* replaceIsRegex,
									JBoolean* preserveCase,
									JRegex** regex) const;

	void	SetFont(const JCharacter* name, const JSize size);

	void	ReadSetup(istream& input);
	void	WriteSetup(ostream& output) const;

	void	SetSearchTextHelpName(const JCharacter* name);
	void	SetRegexQRefName(const JCharacter* name);

protected:

	JXSearchTextDialog(const JCharacter* searchTextHelpName = NULL);

	void	JXSearchTextDialogX();

	void	SetObjects(JXInputField* searchInput, JXStringHistoryMenu* prevSearchMenu,
					   JXTextCheckbox* ignoreCaseCB, JXTextCheckbox* entireWordCB,
					   JXTextCheckbox* wrapSearchCB,
					   JXTextCheckbox* searchIsRegexCB, JXTextCheckbox* singleLineCB,
					   JXInputField* replaceInput, JXStringHistoryMenu* prevReplaceMenu,
					   JXTextCheckbox* replaceIsRegexCB, JXTextCheckbox* preserveCaseCB,
					   JXTextCheckbox* stayOpenCB, JXTextCheckbox* retainFocusCB,
					   JXSearchTextButton* findFwdButton, JXSearchTextButton* findBackButton,
					   JXTextButton* replaceButton,
					   JXSearchTextButton* replaceFindFwdButton, JXSearchTextButton* replaceFindBackButton,
					   JXSearchTextButton* replaceAllFwdButton, JXSearchTextButton* replaceAllBackButton,
					   JXTextButton* replaceAllInSelButton,
					   JXTextButton* closeButton, JXTextButton* helpButton, JXTextButton* qRefButton);

	JXSearchTextButton*	GetFindFwdButton() const;
	JXSearchTextButton*	GetFindBackButton() const;
	JXTextButton*		GetReplaceButton() const;
	JXSearchTextButton*	GetReplaceFindFwdButton() const;
	JXSearchTextButton*	GetReplaceFindBackButton() const;
	JXSearchTextButton*	GetReplaceAllFwdButton() const;
	JXSearchTextButton*	GetReplaceAllBackButton() const;
	JXTextButton*		GetReplaceAllInSelButton() const;

	virtual void	UpdateDisplay();
	virtual void	Receive(JBroadcaster* sender, const Message& message);
	virtual void	ReceiveWithFeedback(JBroadcaster* sender, Message* message);

private:

	JXTEBase*			itsTE;					// can be NULL; not owned
	JRegex*				itsRegex;
	JXTimerTask*		itsUpdateTask;

	const JCharacter*	itsSearchTextHelpName;

	// Xsearch protocol

	Atom	itsXSearchSelectionName;
	Atom	itsXSearchWindowsXAtom;
	Atom	itsXSearchVersionXAtom;
	Atom	itsXSearchDataXAtom [ kCurrentXSearchVersion ];

	Window	itsVersionWindow;		// None if init failed
	Window	itsDataWindow;			// None if init failed

	JBoolean	itsNeedXSearchBcastFlag;		// kJTrue => bcast in GetSearchParameters()
	JBoolean	itsIgnoreXSearchChangeFlag;		// kJTrue => ignore in Receive()

// begin JXLayout

    JXTextButton*        itsCloseButton;
    JXTextCheckbox*      itsIgnoreCaseCB;
    JXTextCheckbox*      itsWrapSearchCB;
    JXTextButton*        itsReplaceButton;
    JXTextCheckbox*      itsEntireWordCB;
    JXStringHistoryMenu* itsPrevReplaceMenu;
    JXStringHistoryMenu* itsPrevSearchMenu;
    JXTextButton*        itsHelpButton;
    JXTextCheckbox*      itsSearchIsRegexCB;
    JXTextCheckbox*      itsReplaceIsRegexCB;
    JXTextCheckbox*      itsSingleLineCB;
    JXTextCheckbox*      itsPreserveCaseCB;
    JXTextButton*        itsQRefButton;
    JXSearchTextButton*  itsFindBackButton;
    JXSearchTextButton*  itsFindFwdButton;
    JXSearchTextButton*  itsReplaceFindBackButton;
    JXSearchTextButton*  itsReplaceFindFwdButton;
    JXSearchTextButton*  itsReplaceAllBackButton;
    JXSearchTextButton*  itsReplaceAllFwdButton;
    JXTextButton*        itsReplaceAllInSelButton;
    JXTextCheckbox*      itsStayOpenCB;
    JXTextCheckbox*      itsRetainFocusCB;
    JXInputField*        itsSearchInput;
    JXInputField*        itsReplaceInput;

// end JXLayout

private:

	void	BuildWindow();

	void	InitXSearch();
	void	GetXSearch();
	void	SetXSearch(const JBoolean grabServer = kJTrue) const;
	void	ReadXSearch(istream& input, const Atom vers);
	void	SetStateForXSearch(JXTextCheckbox* cb,
							   const JCharacter state, const JBoolean negate);
	void	WriteXSearchV1(ostream& output) const;

	// not allowed

	JXSearchTextDialog(const JXSearchTextDialog& source);
	const JXSearchTextDialog& operator=(const JXSearchTextDialog& source);
};


/******************************************************************************
 Active text editor

 ******************************************************************************/

inline JBoolean
JXSearchTextDialog::HasActiveTE()
	const
{
	return JI2B(itsTE != NULL);
}

inline JBoolean
JXSearchTextDialog::GetActiveTE
	(
	JXTEBase** te
	)
	const
{
	*te = itsTE;
	return JI2B(itsTE != NULL);
}

inline void
JXSearchTextDialog::SetActiveTE
	(
	JXTEBase* te
	)
{
	itsTE = te;
	UpdateDisplay();
}

inline void
JXSearchTextDialog::TEDeactivated
	(
	JXTEBase* te
	)
{
	if (itsTE == te)
		{
		itsTE = NULL;
		UpdateDisplay();
		}
}

/******************************************************************************
 Buttons

 ******************************************************************************/

inline JXSearchTextButton*
JXSearchTextDialog::GetFindFwdButton()
	const
{
	return itsFindFwdButton;
}

inline JXSearchTextButton*
JXSearchTextDialog::GetFindBackButton()
	const
{
	return itsFindBackButton;
}

inline JXTextButton*
JXSearchTextDialog::GetReplaceButton()
	const
{
	return itsReplaceButton;
}

inline JXSearchTextButton*
JXSearchTextDialog::GetReplaceFindFwdButton()
	const
{
	return itsReplaceFindFwdButton;
}

inline JXSearchTextButton*
JXSearchTextDialog::GetReplaceFindBackButton()
	const
{
	return itsReplaceFindBackButton;
}

inline JXSearchTextButton*
JXSearchTextDialog::GetReplaceAllFwdButton()
	const
{
	return itsReplaceAllFwdButton;
}

inline JXSearchTextButton*
JXSearchTextDialog::GetReplaceAllBackButton()
	const
{
	return itsReplaceAllBackButton;
}

inline JXTextButton*
JXSearchTextDialog::GetReplaceAllInSelButton()
	const
{
	return itsReplaceAllInSelButton;
}

#endif
