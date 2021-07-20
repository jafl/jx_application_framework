/******************************************************************************
 CBDTreeDirector.cpp

	BASE CLASS = CBTreeDirector

	Copyright © 2021 by John Lindal.

 ******************************************************************************/

#include "CBDTreeDirector.h"
#include "CBDTree.h"
#include "CBProjectDocument.h"
#include "cbActionDefs.h"
#include "cbGlobals.h"
#include <JXTextMenu.h>
#include <JXToolBar.h>
#include <jAssert.h>

#include "jcc_d_tree_window.xpm"

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

CBDTreeDirector::CBDTreeDirector
	(
	CBProjectDocument* supervisor
	)
	:
	CBTreeDirector(supervisor, NewDTree, "WindowTitleSuffix::CBDTreeDirector",
				   "CBDTreeHelp", jcc_d_tree_window,
				   kTreeMenuStr, "CBDTreeDirector",
				   kCBDTreeToolBarID, InitDTreeToolBar)
{
	CBDTreeDirectorX();
}

CBDTreeDirector::CBDTreeDirector
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
				   supervisor, subProject, StreamInDTree,
				   "WindowTitleSuffix::CBDTreeDirector",
				   "CBDTreeHelp", jcc_d_tree_window,
				   kTreeMenuStr, "CBDTreeDirector",
				   kCBDTreeToolBarID, InitDTreeToolBar,
				   nullptr, false)
{
	CBDTreeDirectorX();
}

// private

void
CBDTreeDirector::CBDTreeDirectorX()
{
	itsDTree = dynamic_cast<CBDTree*>(GetTree());
	assert( itsDTree != nullptr );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBDTreeDirector::~CBDTreeDirector()
{
}

/******************************************************************************
 UpdateTreeMenu (virtual protected)

 ******************************************************************************/

void
CBDTreeDirector::UpdateTreeMenu()
{
	JXTextMenu* treeMenu = GetTreeMenu();

	treeMenu->EnableItem(kEditSearchPathsCmd);
	treeMenu->EnableItem(kUpdateCurrentCmd);

	if (!itsDTree->IsEmpty())
		{
		treeMenu->EnableItem(kFindFnCmd);
		treeMenu->EnableItem(kTreeExpandAllCmd);

		if (itsDTree->NeedsMinimizeMILinks())
			{
			treeMenu->EnableItem(kForceMinMILinksCmd);
			}
		}

	bool hasSelection, canCollapse, canExpand;
	itsDTree->GetMenuInfo(&hasSelection, &canCollapse, &canExpand);
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
CBDTreeDirector::HandleTreeMenu
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
		itsDTree->ForceMinimizeMILinks();
		}

	else if (index == kTreeOpenSourceCmd)
		{
		itsDTree->ViewSelectedSources();
		}
	else if (index == kTreeOpenFnListCmd)
		{
		itsDTree->ViewSelectedFunctionLists();
		}

	else if (index == kTreeCollapseCmd)
		{
		itsDTree->CollapseExpandSelectedClasses(true);
		}
	else if (index == kTreeExpandCmd)
		{
		itsDTree->CollapseExpandSelectedClasses(false);
		}
	else if (index == kTreeExpandAllCmd)
		{
		itsDTree->ExpandAllClasses();
		}

	else if (index == kTreeSelParentsCmd)
		{
		itsDTree->SelectParents();
		}
	else if (index == kTreeSelDescendantsCmd)
		{
		itsDTree->SelectDescendants();
		}
	else if (index == kCopySelNamesCmd)
		{
		itsDTree->CopySelectedClassNames();
		}

	else if (index == kFindFnCmd)
		{
		AskForFunctionToFind();
		}
}

/******************************************************************************
 NewDTree (static private)

 ******************************************************************************/

CBTree*
CBDTreeDirector::NewDTree
	(
	CBTreeDirector*	director,
	const JSize		marginWidth
	)
{
	// dynamic_cast<> doesn't work because object is not fully constructed

	auto* dTreeDir = static_cast<CBDTreeDirector*>(director);
	assert( dTreeDir != nullptr );

	auto* tree = jnew CBDTree(dTreeDir, marginWidth);
	assert( tree != nullptr );
	return tree;
}

/******************************************************************************
 StreamInDTree (static private)

 ******************************************************************************/

CBTree*
CBDTreeDirector::StreamInDTree
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
	// dynamic_cast<> doesn't work because object is not fully constructed

	auto* dTreeDir = static_cast<CBDTreeDirector*>(director);
	assert( dTreeDir != nullptr );

	auto* tree = jnew CBDTree(projInput, projVers,
								setInput, setVers, symInput, symVers,
								dTreeDir, marginWidth, dirList);
	assert( tree != nullptr );
	return tree;
}

/******************************************************************************
 InitDTreeToolBar (static private)

 ******************************************************************************/

void
CBDTreeDirector::InitDTreeToolBar
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
