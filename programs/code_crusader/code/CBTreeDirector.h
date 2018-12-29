/******************************************************************************
 CBTreeDirector.h

	Copyright (C) 1996-99 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBTreeDirector
#define _H_CBTreeDirector

#include <JXWindowDirector.h>
#include <JPrefObject.h>
#include <JFAID.h>
#include "CBPrefsManager.h"		// need definition of FileTypesChanged

struct JXPM;
class JXTextMenu;
class JXToolBar;
class JXScrollbarSet;
class JXPSPrinter;
class JXEPSPrinter;
class JXGetStringDialog;
class CBDirList;
class CBClass;
class CBTree;
class CBTreeWidget;
class CBFnListDirector;
class CBEditTreePrefsDialog;
class CBProjectDocument;
class CBCommandMenu;
class CBSymbolUpdatePG;

class CBTreeDirector;
typedef CBTree* (CBTreeCreateFn)(CBTreeDirector* director, const JSize marginWidth);
typedef CBTree* (CBTreeStreamInFn)(std::istream& projInput, const JFileVersion projVers,
								   std::istream* setInput, const JFileVersion setVers,
								   std::istream* symInput, const JFileVersion symVers,
								   CBTreeDirector* director,
								   const JSize marginWidth, CBDirList* dirList);
typedef void (CBTreeInitToolBarFn)(JXToolBar* toolBar, JXTextMenu* treeMenu);

class CBTreeDirector : public JXWindowDirector, public JPrefObject
{
public:

	CBTreeDirector(CBProjectDocument* supervisor, CBTreeCreateFn* createTreeFn,
				   const JString& windowTitleSuffix,
				   const JUtf8Byte* windowHelpName,
				   const JXPM& windowIcon,
				   const JString& treeMenuTitle, const JString& treeMenuItems,
				   const JString& treeMenuNamespace,
				   const JIndex toolBarPrefID, CBTreeInitToolBarFn* initToolBarFn);
	CBTreeDirector(std::istream& projInput, const JFileVersion projVers,
				   std::istream* setInput, const JFileVersion setVers,
				   std::istream* symInput, const JFileVersion symVers,
				   CBProjectDocument* supervisor, const JBoolean subProject,
				   CBTreeStreamInFn* streamInTreeFn,
				   const JString& windowTitleSuffix,
				   const JUtf8Byte* windowHelpName,
				   const JXPM& windowIcon,
				   const JString& treeMenuTitle, const JString& treeMenuItems,
				   const JString& treeMenuNamespace,
				   const JIndex toolBarPrefID, CBTreeInitToolBarFn* initToolBarFn,
				   CBDirList* dirList, const JBoolean readCompileRunDialogs);

	virtual ~CBTreeDirector();

	CBProjectDocument*	GetProjectDoc() const;
	CBTreeWidget*		GetTreeWidget() const;
	CBTree*				GetTree() const;

	void		AskForFunctionToFind();
	JBoolean	ShowInheritedFns() const;

	void		ViewFunctionList(const CBClass* theClass);
	JBoolean	HasFunctionBrowsers() const;
	void		CloseFunctionBrowsers();
	void		FnBrowserCreated(CBFnListDirector* dir);

	virtual void	StreamOut(std::ostream& projOutput, std::ostream* setOutput,
							  std::ostream* symOutput, const CBDirList* dirList) const;

	// for loading updated symbols

	virtual void	ReloadSetup(std::istream& input, const JFileVersion vers);

	// called by CBProjectDocument

	void	FileTypesChanged(const CBPrefsManager::FileTypesChanged& info);

	// called by CBFileListTable

	void		PrepareForTreeUpdate(const JBoolean reparseAll);
	JBoolean	TreeUpdateFinished(const JArray<JFAID_t>& deadFileList);

	// called by CBProjectDocument

	void	SetTreePrefs(const JSize fontSize, const JBoolean showInheritedFns,
						 const JBoolean autoMinMILinks, const JBoolean drawMILinksOnTop,
						 const JBoolean raiseWhenSingleMatch);

protected:

	JXTextMenu*		GetTreeMenu() const;
	virtual void	UpdateTreeMenu() = 0;
	virtual void	HandleTreeMenu(const JIndex index) = 0;

	virtual void	ReadPrefs(std::istream& input) override;
	virtual void	WritePrefs(std::ostream& output) const override;

	virtual void	DirectorClosed(JXDirector* theDirector) override;
	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveWithFeedback(JBroadcaster* sender, Message* message) override;

private:

	CBProjectDocument*	itsProjDoc;
	CBTreeWidget*		itsTreeWidget;
	CBTree*				itsTree;

	JXPSPrinter*		itsPSPrinter;
	JXEPSPrinter*		itsEPSPrinter;
	JXPSPrinter*		itsFnListPrinter;		// shared by all CBFnListDirectors

	JPtrArray<CBFnListDirector>*	itsFnBrowsers;			// contents not owned
	JBoolean						itsShowInheritedFnsFlag;

	JXTextMenu*		itsFileMenu;
	JXTextMenu*		itsTreeMenu;
	JXTextMenu*		itsProjectMenu;
	CBCommandMenu*	itsCmdMenu;
	JXTextMenu*		itsPrefsMenu;
	JXTextMenu*		itsHelpMenu;

	JXGetStringDialog*	itsFindFnDialog;	// usually nullptr

	const JString		itsWindowTitleSuffix;
	const JUtf8Byte*	itsWindowHelpName;

// begin JXLayout

	JXToolBar* itsToolBar;

// end JXLayout

private:

	JXScrollbarSet*	CBTreeDirectorX(CBProjectDocument* doc, const JXPM& windowIcon,
									const JString& treeMenuTitle, const JString& treeMenuItems,
									const JString& treeMenuNamespace,
									const JIndex toolBarPrefID, CBTreeInitToolBarFn* initToolBarFn);
	JXScrollbarSet*	BuildWindow(const JXPM& windowIcon,
								const JString& treeMenuTitle, const JString& treeMenuItems,
								const JString& treeMenuNamespace,
								const JIndex toolBarPrefID, CBTreeInitToolBarFn* initToolBarFn);
	void			AdjustWindowTitle();
	void			ReconnectFunctionBrowsers();

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	UpdateProjectMenu();
	void	HandleProjectMenu(const JIndex index);

	void	UpdatePrefsMenu();
	void	HandlePrefsMenu(const JIndex index);
	void	EditTreePrefs();

	// not allowed

	CBTreeDirector(const CBTreeDirector& source);
	const CBTreeDirector& operator=(const CBTreeDirector& source);
};


/******************************************************************************
 GetTree

 ******************************************************************************/

inline CBTree*
CBTreeDirector::GetTree()
	const
{
	return itsTree;
}

/******************************************************************************
 GetTreeWidget

 ******************************************************************************/

inline CBTreeWidget*
CBTreeDirector::GetTreeWidget()
	const
{
	return itsTreeWidget;
}

/******************************************************************************
 GetTreeMenu

 ******************************************************************************/

inline JXTextMenu*
CBTreeDirector::GetTreeMenu()
	const
{
	return itsTreeMenu;
}

/******************************************************************************
 GetProjectDoc

 ******************************************************************************/

inline CBProjectDocument*
CBTreeDirector::GetProjectDoc()
	const
{
	return itsProjDoc;
}

/******************************************************************************
 ShowInheritedFns

 ******************************************************************************/

inline JBoolean
CBTreeDirector::ShowInheritedFns()
	const
{
	return itsShowInheritedFnsFlag;
}

/******************************************************************************
 HasFunctionBrowsers

 ******************************************************************************/

inline JBoolean
CBTreeDirector::HasFunctionBrowsers()
	const
{
	return !itsFnBrowsers->IsEmpty();
}

/******************************************************************************
 FnBrowserCreated

 ******************************************************************************/

inline void
CBTreeDirector::FnBrowserCreated
	(
	CBFnListDirector* dir
	)
{
	itsFnBrowsers->Append(dir);
}

#endif
