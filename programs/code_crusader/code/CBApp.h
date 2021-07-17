/******************************************************************************
 CBApp.h

	Interface for the CBApp class

	Copyright © 1996-98 by John Lindal.

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

	CBApp(int* argc, char* argv[], const bool useMDI,
		  bool* displayAbout, JString* prevVersStr);

	virtual ~CBApp();

	virtual void	Quit();

	static void				InitStrings();
	static const JUtf8Byte*	GetAppSignature();

	void	DisplayAbout(const JString& prevVersStr = JString::empty,
						 const bool init = false);
	void	EditMiscPrefs();

	JXTextMenu*	CreateHelpMenu(JXMenuBar* menuBar, const JUtf8Byte* idNamespace);
	void		AppendHelpMenuToToolBar(JXToolBar* toolBar, JXTextMenu* menu);
	void		UpdateHelpMenu(JXTextMenu* menu);
	void		HandleHelpMenu(JXTextMenu* menu, const JUtf8Byte* windowSectionName,
							   const JIndex index);

	bool	FindFile(const JString& fileName, const JString::Case caseSensitive,
					 JString* fullName) const;
	bool	FindAndViewFile(const JString& fileName,
							const JIndexRange lineRange = JIndexRange(),
							const JString::Case caseSensitive = JString::kCompareCase) const;

	// called by CBEditGenPrefsDialog

	void	GetWarnings(bool* warnBeforeQuit) const;
	void	SetWarnings(const bool warnBeforeQuit);

protected:

	virtual void	ReadPrefs(std::istream& input);
	virtual void	WritePrefs(std::ostream& output) const;

	virtual bool	Close();
	virtual void	CleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

private:

	bool				itsWarnBeforeQuitFlag;
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
	bool* warnBeforeQuit
	)
	const
{
	*warnBeforeQuit = itsWarnBeforeQuitFlag;
}

inline void
CBApp::SetWarnings
	(
	const bool warnBeforeQuit
	)
{
	itsWarnBeforeQuitFlag = warnBeforeQuit;
}

#endif
