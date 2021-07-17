/******************************************************************************
 CBTreeDirector.h

	Copyright © 1996-99 by John Lindal.

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
				   const JUtf8Byte* windowTitleSuffixID,
				   const JUtf8Byte* windowHelpName,
				   const JXPM& windowIcon,
				   const JUtf8Byte* treeMenuItems, const JUtf8Byte* treeMenuNamespace,
				   const JIndex toolBarPrefID, CBTreeInitToolBarFn* initToolBarFn);
	CBTreeDirector(std::istream& projInput, const JFileVersion projVers,
				   std::istream* setInput, const JFileVersion setVers,
				   std::istream* symInput, const JFileVersion symVers,
				   CBProjectDocument* supervisor, const bool subProject,
				   CBTreeStreamInFn* streamInTreeFn,
				   const JUtf8Byte* windowTitleSuffixID,
				   const JUtf8Byte* windowHelpName,
				   const JXPM& windowIcon,
				   const JUtf8Byte* treeMenuItems, const JUtf8Byte* treeMenuNamespace,
				   const JIndex toolBarPrefID, CBTreeInitToolBarFn* initToolBarFn,
				   CBDirList* dirList, const bool readCompileRunDialogs);

	virtual ~CBTreeDirector();

	CBProjectDocument*	GetProjectDoc() const;
	CBTreeWidget*		GetTreeWidget() const;
	CBTree*				GetTree() const;

	void		AskForFunctionToFind();
	bool	ShowInheritedFns() const;

	void		ViewFunctionList(const CBClass* theClass);

	virtual void	StreamOut(std::ostream& projOutput, std::ostream* setOutput,
							  std::ostream* symOutput, const CBDirList* dirList) const;

	// for loading updated symbols

	virtual void	ReloadSetup(std::istream& input, const JFileVersion vers);

	// called by CBProjectDocument

	void	FileTypesChanged(const CBPrefsManager::FileTypesChanged& info);

	// called by CBFileListTable

	void		PrepareForTreeUpdate(const bool reparseAll);
	bool	TreeUpdateFinished(const JArray<JFAID_t>& deadFileList);

	// called by CBProjectDocument

	void	SetTreePrefs(const JSize fontSize, const bool showInheritedFns,
						 const bool autoMinMILinks, const bool drawMILinksOnTop,
						 const bool raiseWhenSingleMatch);

protected:

	JXTextMenu*		GetTreeMenu() const;
	virtual void	UpdateTreeMenu() = 0;
	virtual void	HandleTreeMenu(const JIndex index) = 0;

	virtual void	ReadPrefs(std::istream& input) override;
	virtual void	WritePrefs(std::ostream& output) const override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveWithFeedback(JBroadcaster* sender, Message* message) override;

private:

	CBProjectDocument*	itsProjDoc;
	CBTreeWidget*		itsTreeWidget;
	CBTree*				itsTree;
	bool			itsShowInheritedFnsFlag;

	JXPSPrinter*		itsPSPrinter;
	JXEPSPrinter*		itsEPSPrinter;
	JXPSPrinter*		itsFnListPrinter;		// shared by all CBFnListDirectors

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
									const JUtf8Byte* treeMenuItems,
									const JUtf8Byte* treeMenuNamespace,
									const JIndex toolBarPrefID, CBTreeInitToolBarFn* initToolBarFn);
	JXScrollbarSet*	BuildWindow(const JXPM& windowIcon,
								const JUtf8Byte* treeMenuItems,
								const JUtf8Byte* treeMenuNamespace,
								const JIndex toolBarPrefID, CBTreeInitToolBarFn* initToolBarFn);
	void			AdjustWindowTitle();

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

inline bool
CBTreeDirector::ShowInheritedFns()
	const
{
	return itsShowInheritedFnsFlag;
}

#endif
