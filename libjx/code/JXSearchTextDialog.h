/******************************************************************************
 JXSearchTextDialog.h

	Copyright (C) 1996-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXSearchTextDialog
#define _H_JXSearchTextDialog

#include <JXWindowDirector.h>
#include <JUtf8Character.h>
#include <X11/X.h>

class JString;
class JRegex;
class JInterpolate;
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

	static JXSearchTextDialog*	Create(const JUtf8Byte* searchTextHelpName = NULL);

	virtual ~JXSearchTextDialog();

	virtual void		Activate();
	virtual JBoolean	Deactivate();

	JBoolean	HasActiveTE() const;
	JBoolean	GetActiveTE(JXTEBase** te) const;
	void		SetActiveTE(JXTEBase* te);
	void		TEDeactivated(JXTEBase* te);

	JBoolean	HasSearchText() const;
	void		SetSearchText(const JString& str);
	void		SetRegexSearch(const JBoolean regex = kJTrue);

	JBoolean	HasReplaceText() const;
	void		SetReplaceText(const JString& str);
	void		SetRegexReplace(const JBoolean regex = kJTrue);

	JBoolean	GetSearchParameters(JString* searchStr, JBoolean* searchIsRegex,
									JBoolean* caseSensitive, JBoolean* entireWord,
									JBoolean* wrapSearch,
									JString* replaceStr, JBoolean* replaceIsRegex,
									JBoolean* preserveCase,
									JRegex** regex) const;

	void	SetFont(const JString& name, const JSize size);

	void	ReadSetup(std::istream& input);
	void	WriteSetup(std::ostream& output) const;

	void	SetSearchTextHelpName(const JUtf8Byte* name);
	void	SetRegexQRefName(const JUtf8Byte* name);

protected:

	JXSearchTextDialog(const JUtf8Byte* searchTextHelpName = NULL);

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
	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveWithFeedback(JBroadcaster* sender, Message* message) override;

public:		// kAtomCount required at global scope

	enum
	{
		kXSearchSelectionAtomIndex,
		kXSearchWindowsAtomIndex,
		kXSearchVersionAtomIndex,
		kXSearchDataV1AtomIndex,

		kAtomCount
	};

private:

	JXTEBase*			itsTE;					// can be NULL; not owned
	JRegex*				itsRegex;
	JInterpolate*		itsInterpolator;
	JXTimerTask*		itsUpdateTask;

	const JUtf8Byte*	itsSearchTextHelpName;
	Atom				itsAtoms[ kAtomCount ];

	Window	itsVersionWindow;		// None if init failed
	Window	itsDataWindow;			// None if init failed

	mutable JBoolean	itsNeedXSearchBcastFlag;		// kJTrue => bcast in GetSearchParameters()
	JBoolean			itsIgnoreXSearchChangeFlag;		// kJTrue => ignore in Receive()

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
	void	ReadXSearch(std::istream& input, const Atom vers);
	void	SetStateForXSearch(JXTextCheckbox* cb,
							   const JUtf8Byte state, const JBoolean negate);
	void	WriteXSearchV1(std::ostream& output) const;

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
