/******************************************************************************
 CBApp.h

	Interface for the CBApp class

	Copyright (C) 1996-98 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBApp
#define _H_CBApp

#include <JXApplication.h>
#include <JPrefObject.h>

class JXMenuBar;
class JXTextMenu;
class JXToolBar;
class CBDirInfoList;

class CBApp : public JXApplication, public JPrefObject
{
public:

	CBApp(int* argc, char* argv[], const JBoolean useMDI,
		  JBoolean* displayAbout, JString* prevVersStr);

	virtual ~CBApp();

	virtual void	Quit();

	static void					InitStrings();
	static const JCharacter*	GetAppSignature();

	void	DisplayAbout(const JCharacter* prevVersStr = nullptr,
						 const JBoolean init = kJFalse);
	void	EditMiscPrefs();

	JXTextMenu*	CreateHelpMenu(JXMenuBar* menuBar, const JCharacter* idNamespace);
	void		AppendHelpMenuToToolBar(JXToolBar* toolBar, JXTextMenu* menu);
	void		UpdateHelpMenu(JXTextMenu* menu);
	void		HandleHelpMenu(JXTextMenu* menu, const JCharacter* windowSectionName,
							   const JIndex index);

	JBoolean	FindFile(const JCharacter* fileName, const JBoolean caseSensitive,
						 JString* fullName) const;
	JBoolean	FindAndViewFile(const JCharacter* fileName,
								const JIndexRange lineRange = JIndexRange(),
								const JBoolean caseSensitive = kJTrue) const;

	// called by CBEditGenPrefsDialog

	void	GetWarnings(JBoolean* warnBeforeQuit) const;
	void	SetWarnings(const JBoolean warnBeforeQuit);

protected:

	virtual void	ReadPrefs(std::istream& input);
	virtual void	WritePrefs(std::ostream& output) const;

	virtual JBoolean	Close();
	virtual void		CleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

private:

	JBoolean			itsWarnBeforeQuitFlag;
	JPtrArray<JString>*	itsSystemIncludeDirs;

private:

	void	GetSystemIncludeDirectories();
	void	CollectSearchPaths(CBDirInfoList* searchPaths) const;

	// not allowed

	CBApp(const CBApp& source);
	const CBApp& operator=(const CBApp& source);
};


/******************************************************************************
 Warnings

 ******************************************************************************/

inline void
CBApp::GetWarnings
	(
	JBoolean* warnBeforeQuit
	)
	const
{
	*warnBeforeQuit = itsWarnBeforeQuitFlag;
}

inline void
CBApp::SetWarnings
	(
	const JBoolean warnBeforeQuit
	)
{
	itsWarnBeforeQuitFlag = warnBeforeQuit;
}

#endif
