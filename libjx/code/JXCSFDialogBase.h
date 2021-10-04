/******************************************************************************
 JXCSFDialogBase.h

	Interface for the JXCSFDialogBase class

	Copyright (C) 1996-99 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXCSFDialogBase
#define _H_JXCSFDialogBase

#include "jx-af/jx/JXDialogDirector.h"
#include <jx-af/jcore/JString.h>

class JDirInfo;
class JXDirTable;
class JXWidget;
class JXStaticText;
class JXInputField;
class JXPathInput;
class JXTextButton;
class JXTextCheckbox;
class JXStringHistoryMenu;
class JXPathHistoryMenu;
class JXScrollbarSet;
class JXNewDirButton;
class JXCurrentPathMenu;
class JXGetNewDirDialog;
class JXIdleTask;

class JXCSFDialogBase : public JXDialogDirector
{
public:

	virtual ~JXCSFDialogBase();

	void		Activate() override;
	bool	Deactivate() override;

	const JString&	GetPath() const;
	const JString&	GetFilter() const;
	bool		HiddenVisible() const;

	// called by JXChooseSaveFile

	void	ReadBaseSetup(std::istream& input, const bool ignoreScroll);
	void	WriteBaseSetup(std::ostream& output) const;

protected:

	JXCSFDialogBase(JXDirector* supervisor, JDirInfo* dirInfo,
					const JString& fileFilter);

	void	SetObjects(JXScrollbarSet* scrollbarSet,
					   JXStaticText* pathLabel, JXPathInput* pathInput,
					   JXPathHistoryMenu* pathHistory,
					   JXStaticText* filterLabel, JXInputField* filterInput,
					   JXStringHistoryMenu* filterHistory,
					   JXTextButton* enterButton,
					   JXTextButton* upButton, JXTextButton* homeButton,
					   JXTextButton* desktopButton,
					   JXNewDirButton* newDirButton, JXTextCheckbox* showHiddenCB,
					   JXCurrentPathMenu* currPathMenu,
					   const JString& message);

	virtual void	AdjustSizings();	// must call inherited
	virtual void	UpdateDisplay();	// must call inherited

	JDirInfo*		GetDirInfo() const;
	JXDirTable*		GetFileBrowser() const;
	JXPathInput*	GetPathInput() const;
	JXInputField*	GetFilterInput() const;
	JXNewDirButton*	GetNewDirButton() const;

	bool	GoToItsPath();
	void		AdjustFilter();

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JDirInfo*	itsDirInfo;			// we don't own this
	JString		itsPrevPath;
	JString		itsPrevFilterString;
	bool	itsDeactCancelFlag;

	JXDirTable*				itsFileBrowser;
	JXPathHistoryMenu*		itsPathHistory;
	JXPathInput*			itsPathInput;
	JXStringHistoryMenu*	itsFilterHistory;
	JXInputField*			itsFilterInput;
	JXTextButton*			itsEnterButton;
	JXTextButton*			itsUpButton;
	JXTextButton*			itsHomeButton;
	JXTextButton*			itsDesktopButton;
	JXNewDirButton*			itsNewDirButton;	// can be nullptr
	JXTextCheckbox*			itsShowHiddenCB;
	JXCurrentPathMenu*		itsCurrPathMenu;

	JXGetNewDirDialog*	itsNewDirDialog;

private:

	void	DisplayMessage(const JString& message, JXScrollbarSet* scrollbarSet,
						   JXStaticText* pathLabel, JXPathHistoryMenu* pathHistory,
						   JXStaticText* filterLabel, JXStringHistoryMenu* filterHistory);

	void	GetNewDirectory();
	void	CreateNewDirectory();

	void	SelectPrevDirectory();
};


/******************************************************************************
 Access to objects (protected)

 ******************************************************************************/

inline JDirInfo*
JXCSFDialogBase::GetDirInfo()
	const
{
	return itsDirInfo;
}

inline JXDirTable*
JXCSFDialogBase::GetFileBrowser()
	const
{
	return itsFileBrowser;
}

inline JXPathInput*
JXCSFDialogBase::GetPathInput()
	const
{
	return itsPathInput;
}

inline JXInputField*
JXCSFDialogBase::GetFilterInput()
	const
{
	return itsFilterInput;
}

inline JXNewDirButton*
JXCSFDialogBase::GetNewDirButton()
	const
{
	return itsNewDirButton;
}

#endif
