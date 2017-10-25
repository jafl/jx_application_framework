/******************************************************************************
 CBJavaTreeDirector.cpp

	BASE CLASS = CBTreeDirector

	Copyright (C) 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBJavaTreeDirector.h"
#include "CBJavaTree.h"
#include "CBProjectDocument.h"
#include "cbHelpText.h"
#include "cbActionDefs.h"
#include "cbGlobals.h"
#include <JXTextMenu.h>
#include <JXToolBar.h>
#include <jAssert.h>

#include "jcc_java_tree_window.xpm"

static const JCharacter* kWindowTitleSuffix = " Java Inheritance";

// Tree menu

static const JCharacter* kTreeMenuTitleStr = "Tree";
static const JCharacter* kTreeMenuStr =
	"    Add classes...                                         %i" kCBEditSearchPathsAction
	"  | Update                 %k Meta-U                       %i" kCBUpdateClassTreeAction
	"  | Minimize MI link lengths now                           %i" kCBMinimizeMILinkLengthAction
	"%l| Open source            %k Left-dbl-click or Return     %i" kCBOpenSelectedFilesAction
	"  | Open function list     %k Right-dbl-click              %i" kCBOpenClassFnListAction
	"%l| Collapse               %k Meta-<                       %i" kCBCollapseClassesAction
	"  | Expand                 %k Meta->                       %i" kCBExpandClassesAction
	"  | Expand all                                             %i" kCBExpandAllClassesAction
	"%l| Select parents                                         %i" kCBSelectParentClassAction
	"  | Select descendants                                     %i" kCBSelectDescendantClassAction
	"  | Copy selected names    %k Meta-C                       %i" kCBCopyClassNameAction
	"%l| Find function...       %k Meta-F                       %i" kCBFindFunctionAction
	"  | Close function windows                                 %i" kCBCloseAllClassFnListAction;

enum
{
	kEditSearchPathsCmd = 1, kUpdateCurrentCmd,
	kForceMinMILinksCmd,
	kTreeOpenSourceCmd, kTreeOpenFnListCmd,
	kTreeCollapseCmd, kTreeExpandCmd, kTreeExpandAllCmd,
	kTreeSelParentsCmd, kTreeSelDescendantsCmd, kCopySelNamesCmd,
	kFindFnCmd, kCloseFnWindCmd
};

/******************************************************************************
 Constructor

 ******************************************************************************/

CBJavaTreeDirector::CBJavaTreeDirector
	(
	CBProjectDocument* supervisor
	)
	:
	CBTreeDirector(supervisor, NewJavaTree, kWindowTitleSuffix, kCBJavaTreeHelpName,
				   jcc_java_tree_window, kTreeMenuTitleStr, kTreeMenuStr,
				   "CBJavaTreeDirector",
				   kCBJavaTreeToolBarID, InitJavaTreeToolBar)
{
	CBJavaTreeDirectorX();
}

CBJavaTreeDirector::CBJavaTreeDirector
	(
	std::istream&			projInput,
	const JFileVersion	projVers,
	std::istream*			setInput,
	const JFileVersion	setVers,
	std::istream*			symInput,
	const JFileVersion	symVers,
	CBProjectDocument*	supervisor,
	const JBoolean		subProject
	)
	:
	CBTreeDirector(projInput, projVers, setInput, setVers, symInput, symVers,
				   supervisor, subProject, StreamInJavaTree,
				   kWindowTitleSuffix, kCBJavaTreeHelpName, jcc_java_tree_window,
				   kTreeMenuTitleStr, kTreeMenuStr, "CBJavaTreeDirector",
				   kCBJavaTreeToolBarID, InitJavaTreeToolBar,
				   NULL, kJFalse)
{
	CBJavaTreeDirectorX();
}

// private

void
CBJavaTreeDirector::CBJavaTreeDirectorX()
{
	itsJavaTree = dynamic_cast<CBJavaTree*>(GetTree());
	assert( itsJavaTree != NULL );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBJavaTreeDirector::~CBJavaTreeDirector()
{
}

/******************************************************************************
 UpdateTreeMenu (virtual protected)

 ******************************************************************************/

void
CBJavaTreeDirector::UpdateTreeMenu()
{
	JXTextMenu* treeMenu = GetTreeMenu();

	treeMenu->EnableItem(kEditSearchPathsCmd);
	treeMenu->EnableItem(kUpdateCurrentCmd);

	if (!itsJavaTree->IsEmpty())
		{
		treeMenu->EnableItem(kFindFnCmd);
		treeMenu->EnableItem(kTreeExpandAllCmd);

		if (itsJavaTree->NeedsMinimizeMILinks())
			{
			treeMenu->EnableItem(kForceMinMILinksCmd);
			}
		}

	JBoolean hasSelection, canCollapse, canExpand;
	itsJavaTree->GetMenuInfo(&hasSelection, &canCollapse, &canExpand);
	if (hasSelection)
		{
		treeMenu->EnableItem(kTreeOpenSourceCmd);
		treeMenu->EnableItem(kTreeOpenFnListCmd);
		treeMenu->EnableItem(kTreeSelParentsCmd);
		treeMenu->EnableItem(kTreeSelDescendantsCmd);
		treeMenu->EnableItem(kCopySelNamesCmd);
		}
	if (canCollapse)
		{
		treeMenu->EnableItem(kTreeCollapseCmd);
		}
	if (canExpand)
		{
		treeMenu->EnableItem(kTreeExpandCmd);
		}

	if (HasFunctionBrowsers())
		{
		treeMenu->EnableItem(kCloseFnWindCmd);
		}
}

/******************************************************************************
 HandleTreeMenu (virtual protected)

 ******************************************************************************/

void
CBJavaTreeDirector::HandleTreeMenu
	(
	const JIndex index
	)
{
	(CBGetDocumentManager())->SetActiveProjectDocument(GetProjectDoc());

	if (index == kEditSearchPathsCmd)
		{
		GetProjectDoc()->EditSearchPaths(this);
		}
	else if (index == kUpdateCurrentCmd)
		{
		GetProjectDoc()->UpdateSymbolDatabase();
		}
	else if (index == kForceMinMILinksCmd)
		{
		itsJavaTree->ForceMinimizeMILinks();
		}

	else if (index == kTreeOpenSourceCmd)
		{
		itsJavaTree->ViewSelectedSources();
		}
	else if (index == kTreeOpenFnListCmd)
		{
		itsJavaTree->ViewSelectedFunctionLists();
		}

	else if (index == kTreeCollapseCmd)
		{
		itsJavaTree->CollapseExpandSelectedClasses(kJTrue);
		}
	else if (index == kTreeExpandCmd)
		{
		itsJavaTree->CollapseExpandSelectedClasses(kJFalse);
		}
	else if (index == kTreeExpandAllCmd)
		{
		itsJavaTree->ExpandAllClasses();
		}

	else if (index == kTreeSelParentsCmd)
		{
		itsJavaTree->SelectParents();
		}
	else if (index == kTreeSelDescendantsCmd)
		{
		itsJavaTree->SelectDescendants();
		}
	else if (index == kCopySelNamesCmd)
		{
		itsJavaTree->CopySelectedClassNames();
		}

	else if (index == kFindFnCmd)
		{
		AskForFunctionToFind();
		}
	else if (index == kCloseFnWindCmd)
		{
		CloseFunctionBrowsers();
		}
}

/******************************************************************************
 NewJavaTree (static private)

 ******************************************************************************/

CBTree*
CBJavaTreeDirector::NewJavaTree
	(
	CBTreeDirector*	director,
	const JSize		marginWidth
	)
{
	// dynamic_cast<> doesn't work because object is not fully constructed

	CBJavaTreeDirector* javaTreeDir = static_cast<CBJavaTreeDirector*>(director);
	assert( javaTreeDir != NULL );

	CBJavaTree* tree = jnew CBJavaTree(javaTreeDir, marginWidth);
	assert( tree != NULL );
	return tree;
}

/******************************************************************************
 StreamInJavaTree (static private)

 ******************************************************************************/

CBTree*
CBJavaTreeDirector::StreamInJavaTree
	(
	std::istream&			projInput,
	const JFileVersion	projVers,
	std::istream*			setInput,
	const JFileVersion	setVers,
	std::istream*			symInput,
	const JFileVersion	symVers,
	CBTreeDirector*		director,
	const JSize			marginWidth,
	CBDirList*			dirList
	)
{
	assert( dirList == NULL );

	// dynamic_cast<> doesn't work because object is not fully constructed

	CBJavaTreeDirector* javaTreeDir = static_cast<CBJavaTreeDirector*>(director);
	assert( javaTreeDir != NULL );

	CBJavaTree* tree = jnew CBJavaTree(projInput, projVers,
									  setInput, setVers, symInput, symVers,
									  javaTreeDir, marginWidth, dirList);
	assert( tree != NULL );
	return tree;
}

/******************************************************************************
 InitJavaTreeToolBar (static private)

 ******************************************************************************/

void
CBJavaTreeDirector::InitJavaTreeToolBar
	(
	JXToolBar*	toolBar,
	JXTextMenu*	treeMenu
	)
{
	toolBar->AppendButton(treeMenu, kEditSearchPathsCmd);
	toolBar->NewGroup();
	toolBar->AppendButton(treeMenu, kTreeCollapseCmd);
	toolBar->AppendButton(treeMenu, kTreeExpandCmd);
}
