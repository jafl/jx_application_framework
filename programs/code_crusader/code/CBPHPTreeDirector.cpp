/******************************************************************************
 CBPHPTreeDirector.cpp

	BASE CLASS = CBTreeDirector

	Copyright © 2014 by John Lindal.

 ******************************************************************************/

#include "CBPHPTreeDirector.h"
#include "CBPHPTree.h"
#include "CBProjectDocument.h"
#include "cbActionDefs.h"
#include "cbGlobals.h"
#include <JXTextMenu.h>
#include <JXToolBar.h>
#include <jAssert.h>

#include "jcc_php_tree_window.xpm"

// Tree menu

static const JUtf8Byte* kTreeMenuStr =
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
	"%l| Find function...       %k Meta-F                       %i" kCBFindFunctionAction;

enum
{
	kEditSearchPathsCmd = 1, kUpdateCurrentCmd,
	kForceMinMILinksCmd,
	kTreeOpenSourceCmd, kTreeOpenFnListCmd,
	kTreeCollapseCmd, kTreeExpandCmd, kTreeExpandAllCmd,
	kTreeSelParentsCmd, kTreeSelDescendantsCmd, kCopySelNamesCmd,
	kFindFnCmd
};

/******************************************************************************
 Constructor

 ******************************************************************************/

CBPHPTreeDirector::CBPHPTreeDirector
	(
	CBProjectDocument* supervisor
	)
	:
	CBTreeDirector(supervisor, NewPHPTree, "WindowTitleSuffix::CBPHPTreeDirector",
				   "CBPHPTreeHelp", jcc_php_tree_window,
				   kTreeMenuStr, "CBPHPTreeDirector",
				   kCBPHPTreeToolBarID, InitPHPTreeToolBar)
{
	CBPHPTreeDirectorX();
}

CBPHPTreeDirector::CBPHPTreeDirector
	(
	std::istream&		projInput,
	const JFileVersion	projVers,
	std::istream*		setInput,
	const JFileVersion	setVers,
	std::istream*		symInput,
	const JFileVersion	symVers,
	CBProjectDocument*	supervisor,
	const bool		subProject
	)
	:
	CBTreeDirector(projInput, projVers, setInput, setVers, symInput, symVers,
				   supervisor, subProject, StreamInPHPTree,
				   "WindowTitleSuffix::CBPHPTreeDirector",
				   "CBPHPTreeHelp", jcc_php_tree_window,
				   kTreeMenuStr, "CBPHPTreeDirector",
				   kCBPHPTreeToolBarID, InitPHPTreeToolBar,
				   nullptr, false)
{
	CBPHPTreeDirectorX();
}

// private

void
CBPHPTreeDirector::CBPHPTreeDirectorX()
{
	itsPHPTree = dynamic_cast<CBPHPTree*>(GetTree());
	assert( itsPHPTree != nullptr );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBPHPTreeDirector::~CBPHPTreeDirector()
{
}

/******************************************************************************
 UpdateTreeMenu (virtual protected)

 ******************************************************************************/

void
CBPHPTreeDirector::UpdateTreeMenu()
{
	JXTextMenu* treeMenu = GetTreeMenu();

	treeMenu->EnableItem(kEditSearchPathsCmd);
	treeMenu->EnableItem(kUpdateCurrentCmd);

	if (!itsPHPTree->IsEmpty())
		{
		treeMenu->EnableItem(kFindFnCmd);
		treeMenu->EnableItem(kTreeExpandAllCmd);

		if (itsPHPTree->NeedsMinimizeMILinks())
			{
			treeMenu->EnableItem(kForceMinMILinksCmd);
			}
		}

	bool hasSelection, canCollapse, canExpand;
	itsPHPTree->GetMenuInfo(&hasSelection, &canCollapse, &canExpand);
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
}

/******************************************************************************
 HandleTreeMenu (virtual protected)

 ******************************************************************************/

void
CBPHPTreeDirector::HandleTreeMenu
	(
	const JIndex index
	)
{
	CBGetDocumentManager()->SetActiveProjectDocument(GetProjectDoc());

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
		itsPHPTree->ForceMinimizeMILinks();
		}

	else if (index == kTreeOpenSourceCmd)
		{
		itsPHPTree->ViewSelectedSources();
		}
	else if (index == kTreeOpenFnListCmd)
		{
		itsPHPTree->ViewSelectedFunctionLists();
		}

	else if (index == kTreeCollapseCmd)
		{
		itsPHPTree->CollapseExpandSelectedClasses(true);
		}
	else if (index == kTreeExpandCmd)
		{
		itsPHPTree->CollapseExpandSelectedClasses(false);
		}
	else if (index == kTreeExpandAllCmd)
		{
		itsPHPTree->ExpandAllClasses();
		}

	else if (index == kTreeSelParentsCmd)
		{
		itsPHPTree->SelectParents();
		}
	else if (index == kTreeSelDescendantsCmd)
		{
		itsPHPTree->SelectDescendants();
		}
	else if (index == kCopySelNamesCmd)
		{
		itsPHPTree->CopySelectedClassNames();
		}

	else if (index == kFindFnCmd)
		{
		AskForFunctionToFind();
		}
}

/******************************************************************************
 NewPHPTree (static private)

 ******************************************************************************/

CBTree*
CBPHPTreeDirector::NewPHPTree
	(
	CBTreeDirector*	director,
	const JSize		marginWidth
	)
{
	// dynamic_cast<> doesn't work because object is not fully constructed

	auto* phpTreeDir = static_cast<CBPHPTreeDirector*>(director);
	assert( phpTreeDir != nullptr );

	auto* tree = jnew CBPHPTree(phpTreeDir, marginWidth);
	assert( tree != nullptr );
	return tree;
}

/******************************************************************************
 StreamInPHPTree (static private)

 ******************************************************************************/

CBTree*
CBPHPTreeDirector::StreamInPHPTree
	(
	std::istream&		projInput,
	const JFileVersion	projVers,
	std::istream*		setInput,
	const JFileVersion	setVers,
	std::istream*		symInput,
	const JFileVersion	symVers,
	CBTreeDirector*		director,
	const JSize			marginWidth,
	CBDirList*			dirList
	)
{
	assert( dirList == nullptr );

	// dynamic_cast<> doesn't work because object is not fully constructed

	auto* phpTreeDir = static_cast<CBPHPTreeDirector*>(director);
	assert( phpTreeDir != nullptr );

	auto* tree = jnew CBPHPTree(projInput, projVers,
									setInput, setVers, symInput, symVers,
									phpTreeDir, marginWidth, dirList);
	assert( tree != nullptr );
	return tree;
}

/******************************************************************************
 InitPHPTreeToolBar (static private)

 ******************************************************************************/

void
CBPHPTreeDirector::InitPHPTreeToolBar
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
