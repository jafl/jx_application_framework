/******************************************************************************
 CBJavaTreeDirector.cpp

	BASE CLASS = CBTreeDirector

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#include "CBJavaTreeDirector.h"
#include "CBJavaTree.h"
#include "CBProjectDocument.h"
#include "cbActionDefs.h"
#include "cbGlobals.h"
#include <JXTextMenu.h>
#include <JXToolBar.h>
#include <jAssert.h>

#include "jcc_java_tree_window.xpm"

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

CBJavaTreeDirector::CBJavaTreeDirector
	(
	CBProjectDocument* supervisor
	)
	:
	CBTreeDirector(supervisor, NewJavaTree, "WindowTitleSuffix::CBJavaTreeDirector",
				   "CBJavaTreeHelp", jcc_java_tree_window,
				   kTreeMenuStr, "CBJavaTreeDirector",
				   kCBJavaTreeToolBarID, InitJavaTreeToolBar)
{
	CBJavaTreeDirectorX();
}

CBJavaTreeDirector::CBJavaTreeDirector
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
				   supervisor, subProject, StreamInJavaTree,
				   "WindowTitleSuffix::CBJavaTreeDirector",
				   "CBJavaTreeHelp", jcc_java_tree_window,
				   kTreeMenuStr, "CBJavaTreeDirector",
				   kCBJavaTreeToolBarID, InitJavaTreeToolBar,
				   nullptr, false)
{
	CBJavaTreeDirectorX();
}

// private

void
CBJavaTreeDirector::CBJavaTreeDirectorX()
{
	itsJavaTree = dynamic_cast<CBJavaTree*>(GetTree());
	assert( itsJavaTree != nullptr );
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

	bool hasSelection, canCollapse, canExpand;
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
		itsJavaTree->CollapseExpandSelectedClasses(true);
		}
	else if (index == kTreeExpandCmd)
		{
		itsJavaTree->CollapseExpandSelectedClasses(false);
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

	auto* javaTreeDir = static_cast<CBJavaTreeDirector*>(director);
	assert( javaTreeDir != nullptr );

	auto* tree = jnew CBJavaTree(javaTreeDir, marginWidth);
	assert( tree != nullptr );
	return tree;
}

/******************************************************************************
 StreamInJavaTree (static private)

 ******************************************************************************/

CBTree*
CBJavaTreeDirector::StreamInJavaTree
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

	auto* javaTreeDir = static_cast<CBJavaTreeDirector*>(director);
	assert( javaTreeDir != nullptr );

	auto* tree = jnew CBJavaTree(projInput, projVers,
									  setInput, setVers, symInput, symVers,
									  javaTreeDir, marginWidth, dirList);
	assert( tree != nullptr );
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
