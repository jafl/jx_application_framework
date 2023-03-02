/******************************************************************************
 JXCSFDialogBase.h

	Interface for the JXCSFDialogBase class

	Copyright (C) 1996-99 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXCSFDialogBase
#define _H_JXCSFDialogBase

#include "JXModalDialogDirector.h"
#include "JXCSFSelectPrevDirTask.h"

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
class JXIdleTask;

class JXCSFDialogBase : public JXModalDialogDirector
{
public:

	~JXCSFDialogBase() override;

	void	Activate() override;
	bool	Deactivate() override;

	const JString&	GetPath() const;
	const JString&	GetFilter() const;
	bool			HiddenVisible() const;

	JDirInfo*	GetDirInfo() const;

	static const JString&	GetState();
	static void				SetState(const JString& state);
	static void				ReadOldState(std::istream& input);

	static bool	IsCharacterInWord(const JUtf8Character& c);

protected:

	JXCSFDialogBase(const JString& fileFilter);

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

	JXDirTable*		GetFileBrowser() const;
	JXPathInput*	GetPathInput() const;
	JXInputField*	GetFilterInput() const;
	JXNewDirButton*	GetNewDirButton() const;

	bool	GoToItsPath();
	void	AdjustFilter();

	void	RestoreState();
	void	SaveState() const;

	void	DoNotSaveCurrentPath();

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JDirInfo*	itsDirInfo;
	JString		itsPrevPath;
	JString		itsPrevFilterString;
	bool		itsDeactCancelFlag;
	bool		itsSavePathFlag;

	JXDirTable*				itsFileBrowser;
	JXPathInput*			itsPathInput;
	JXPathHistoryMenu*		itsPathHistory;
	JXInputField*			itsFilterInput;
	JXStringHistoryMenu*	itsFilterHistory;
	JXTextButton*			itsEnterButton;
	JXTextButton*			itsUpButton;
	JXTextButton*			itsHomeButton;
	JXTextButton*			itsDesktopButton;
	JXNewDirButton*			itsNewDirButton;		// can be nullptr
	JXTextCheckbox*			itsShowHiddenCB;
	JXCurrentPathMenu*		itsCurrPathMenu;

	JXCSFSelectPrevDirTask*	itsSelectPrevDirTask;	// nullptr unless queued

	static JString	theState;
	static JString	thePath;

private:

	void	DisplayMessage(const JString& message, JXScrollbarSet* scrollbarSet,
						   JXStaticText* pathLabel, JXPathHistoryMenu* pathHistory,
						   JXStaticText* filterLabel, JXStringHistoryMenu* filterHistory);

	void	CreateNewDirectory();

	void	SelectPrevDirectory();
};


/******************************************************************************
 GetDirInfo

 ******************************************************************************/

inline JDirInfo*
JXCSFDialogBase::GetDirInfo()
	const
{
	return itsDirInfo;
}

/******************************************************************************
 State (static)

 ******************************************************************************/

inline const JString&
JXCSFDialogBase::GetState()
{
	return theState;
}

inline void
JXCSFDialogBase::SetState
	(
	const JString& state
	)
{
	theState = state;
}

/******************************************************************************
 Access to objects (protected)

 ******************************************************************************/

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

inline void
JXCSFDialogBase::DoNotSaveCurrentPath()
{
	itsSavePathFlag = false;
}

#endif
