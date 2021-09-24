/******************************************************************************
 JXSearchTextDialog.h

	Copyright (C) 1996-99 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXSearchTextDialog
#define _H_JXSearchTextDialog

#include "JXWindowDirector.h"
#include <JUtf8Character.h>
#include <X11/X.h>

class JFont;
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

	static JXSearchTextDialog*	Create();

	virtual ~JXSearchTextDialog();

	virtual void	Activate() override;
	virtual bool	Deactivate() override;

	bool	HasActiveTE() const;
	bool	GetActiveTE(JXTEBase** te) const;
	void	SetActiveTE(JXTEBase* te);
	void	TEDeactivated(JXTEBase* te);

	bool			HasSearchText() const;
	const JString&	GetSearchText() const;
	void			SetSearchText(const JString& str);
	void			SetRegexSearch(const bool regex = true);

	bool	HasReplaceText() const;
	void	SetReplaceText(const JString& str);
	void	SetRegexReplace(const bool regex = true);

	bool	GetSearchParameters(JRegex** searchRegex, bool* entireWord,
								bool* wrapSearch,
								JString* replaceStr, JInterpolate** interpolator,
								bool* preserveCase) const;

	void	SetFont(const JFont& font);

	void	ReadSetup(std::istream& input);
	void	WriteSetup(std::ostream& output) const;

protected:

	JXSearchTextDialog();

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
					   JXTextButton* replaceAllButton, JXTextButton* replaceAllInSelButton,
					   JXTextButton* closeButton, JXTextButton* helpButton, JXTextButton* qRefButton);

	JXSearchTextButton*	GetFindFwdButton() const;
	JXSearchTextButton*	GetFindBackButton() const;
	JXTextButton*		GetReplaceButton() const;
	JXSearchTextButton*	GetReplaceFindFwdButton() const;
	JXSearchTextButton*	GetReplaceFindBackButton() const;
	JXTextButton*		GetReplaceAllButton() const;
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

	JXTEBase*		itsTE;					// can be nullptr; not owned
	JRegex*			itsRegex;
	JInterpolate*	itsInterpolator;
	JXTimerTask*	itsUpdateTask;

	Atom	itsAtoms[ kAtomCount ];

	Window	itsVersionWindow;		// None if init failed
	Window	itsDataWindow;			// None if init failed

	mutable bool	itsNeedXSearchBcastFlag;		// true => bcast in GetSearchParameters()
	bool			itsIgnoreXSearchChangeFlag;		// true => ignore in Receive()

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
	JXTextButton*        itsReplaceAllInSelButton;
	JXTextCheckbox*      itsStayOpenCB;
	JXTextCheckbox*      itsRetainFocusCB;
	JXInputField*        itsSearchInput;
	JXInputField*        itsReplaceInput;
	JXTextButton*        itsReplaceAllButton;

// end JXLayout

private:

	void	BuildWindow();

	void	InitXSearch();
	void	GetXSearch();
	void	SetXSearch(const bool grabServer = true) const;
	void	ReadXSearch(std::istream& input, const Atom vers);
	void	SetStateForXSearch(JXTextCheckbox* cb,
							   const JUtf8Byte state, const bool negate);
	void	WriteXSearchV1(std::ostream& output) const;
};


/******************************************************************************
 Active text editor

 ******************************************************************************/

inline bool
JXSearchTextDialog::HasActiveTE()
	const
{
	return itsTE != nullptr;
}

inline bool
JXSearchTextDialog::GetActiveTE
	(
	JXTEBase** te
	)
	const
{
	*te = itsTE;
	return itsTE != nullptr;
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
		itsTE = nullptr;
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

inline JXTextButton*
JXSearchTextDialog::GetReplaceAllButton()
	const
{
	return itsReplaceAllButton;
}

inline JXTextButton*
JXSearchTextDialog::GetReplaceAllInSelButton()
	const
{
	return itsReplaceAllInSelButton;
}

#endif
