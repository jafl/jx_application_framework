/******************************************************************************
 CBGoTreeDirector.cpp

	BASE CLASS = CBTreeDirector

	Copyright © 2021 by John Lindal.

 ******************************************************************************/

#include "CBGoTreeDirector.h"
#include "CBGoTree.h"
#include "CBProjectDocument.h"
#include "cbActionDefs.h"
#include "cbGlobals.h"
#include <JXTextMenu.h>
#include <JXToolBar.h>
#include <jAssert.h>

#include "jcc_go_tree_window.xpm"

// Tree menu

static const JUtf8Byte* kTreeMenuStr =
	"    Add structs/interfaces...                              %i" kCBEditSearchPathsAction
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

CBGoTreeDirector::CBGoTreeDirector
	(
	CBProjectDocument* supervisor
	)
	:
	CBTreeDirector(supervisor, NewGoTree, "WindowTitleSuffix::CBGoTreeDirector",
				   "CBGoTreeHelp", jcc_go_tree_window,
				   kTreeMenuStr, "CBGoTreeDirector",
				   kCBGoTreeToolBarID, InitGoTreeToolBar)
{
	CBGoTreeDirectorX();
}

CBGoTreeDirector::CBGoTreeDirector
	(
	std::istream&			projInput,
	const JFileVersion	projVers,
	std::istream*			setInput,
	const JFileVersion	setVers,
	std::istream*			symInput,
	const JFileVersion	symVers,
	CBProjectDocument*	supervisor,
	const bool		subProject
	)
	:
	CBTreeDirector(projInput, projVers, setInput, setVers, symInput, symVers,
				   supervisor, subProject, StreamInGoTree,
				   "WindowTitleSuffix::CBGoTreeDirector",
				   "CBGoTreeHelp", jcc_go_tree_window,
				   kTreeMenuStr, "CBGoTreeDirector",
				   kCBGoTreeToolBarID, InitGoTreeToolBar,
				   nullptr, false)
{
	CBGoTreeDirectorX();
}

// private

void
CBGoTreeDirector::CBGoTreeDirectorX()
{
	itsGoTree = dynamic_cast<CBGoTree*>(GetTree());
	assert( itsGoTree != nullptr );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBGoTreeDirector::~CBGoTreeDirector()
{
}

/******************************************************************************
 UpdateTreeMenu (virtual protected)

 ******************************************************************************/

void
CBGoTreeDirector::UpdateTreeMenu()
{
	JXTextMenu* treeMenu = GetTreeMenu();

	treeMenu->EnableItem(kEditSearchPathsCmd);
	treeMenu->EnableItem(kUpdateCurrentCmd);

	if (!itsGoTree->IsEmpty())
		{
		treeMenu->EnableItem(kFindFnCmd);
		treeMenu->EnableItem(kTreeExpandAllCmd);

		if (itsGoTree->NeedsMinimizeMILinks())
			{
			treeMenu->EnableItem(kForceMinMILinksCmd);
			}
		}

	bool hasSelection, canCollapse, canExpand;
	itsGoTree->GetMenuInfo(&hasSelection, &canCollapse, &canExpand);
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
CBGoTreeDirector::HandleTreeMenu
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
		itsGoTree->ForceMinimizeMILinks();
		}

	else if (index == kTreeOpenSourceCmd)
		{
		itsGoTree->ViewSelectedSources();
		}
	else if (index == kTreeOpenFnListCmd)
		{
		itsGoTree->ViewSelectedFunctionLists();
		}

	else if (index == kTreeCollapseCmd)
		{
		itsGoTree->CollapseExpandSelectedClasses(true);
		}
	else if (index == kTreeExpandCmd)
		{
		itsGoTree->CollapseExpandSelectedClasses(false);
		}
	else if (index == kTreeExpandAllCmd)
		{
		itsGoTree->ExpandAllClasses();
		}

	else if (index == kTreeSelParentsCmd)
		{
		itsGoTree->SelectParents();
		}
	else if (index == kTreeSelDescendantsCmd)
		{
		itsGoTree->SelectDescendants();
		}
	else if (index == kCopySelNamesCmd)
		{
		itsGoTree->CopySelectedClassNames();
		}

	else if (index == kFindFnCmd)
		{
		AskForFunctionToFind();
		}
}

/******************************************************************************
 NewGoTree (static private)

 ******************************************************************************/

CBTree*
CBGoTreeDirector::NewGoTree
	(
	CBTreeDirector*	director,
	const JSize		marginWidth
	)
{
	// dynamic_cast<> doesn't work because object is not fully constructed

	auto* goTreeDir = static_cast<CBGoTreeDirector*>(director);
	assert( goTreeDir != nullptr );

	auto* tree = jnew CBGoTree(goTreeDir, marginWidth);
	assert( tree != nullptr );
	return tree;
}

/******************************************************************************
 StreamInGoTree (static private)

 ******************************************************************************/

CBTree*
CBGoTreeDirector::StreamInGoTree
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

	auto* goTreeDir = static_cast<CBGoTreeDirector*>(director);
	assert( goTreeDir != nullptr );

	auto* tree = jnew CBGoTree(projInput, projVers,
								   setInput, setVers, symInput, symVers,
								   goTreeDir, marginWidth, dirList);
	assert( tree != nullptr );
	return tree;
}

/******************************************************************************
 InitGoTreeToolBar (static private)

 ******************************************************************************/

void
CBGoTreeDirector::InitGoTreeToolBar
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
