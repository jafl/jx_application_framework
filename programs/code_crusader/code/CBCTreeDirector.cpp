/******************************************************************************
 CBCTreeDirector.cpp

	BASE CLASS = CBTreeDirector

	Copyright © 1996-99 by John Lindal.

 ******************************************************************************/

#include "CBCTreeDirector.h"
#include "CBCTree.h"
#include "CBEditCPPMacroDialog.h"
#include "CBProjectDocument.h"
#include "cbActionDefs.h"
#include "cbGlobals.h"
#include <JXTextMenu.h>
#include <JXToolBar.h>
#include <jAssert.h>

#include "jcc_c_tree_window.xpm"

// Tree menu

static const JUtf8Byte* kTreeMenuStr =
	"    Configure C preprocessor...                            %i" kCBConfigureCTreeCPPAction
	"  | Add classes...                                         %i" kCBEditSearchPathsAction
	"  | Update                 %k Meta-U                       %i" kCBUpdateClassTreeAction
	"  | Minimize MI link lengths now                           %i" kCBMinimizeMILinkLengthAction
	"%l| Open source            %k Left-dbl-click or Return     %i" kCBOpenSelectedFilesAction
	"  | Open header            %k Middle-dbl-click or Control-Tab %i" kCBOpenComplFilesAction
	"  | Open function list     %k Right-dbl-click              %i" kCBOpenClassFnListAction
	"%l| Create derived class...                                %i" kCBCreateDerivedClassAction
	"%l| Collapse               %k Meta-<                       %i" kCBCollapseClassesAction
	"  | Expand                 %k Meta->                       %i" kCBExpandClassesAction
	"  | Expand all                                             %i" kCBExpandAllClassesAction
	"%l| Select parents                                         %i" kCBSelectParentClassAction
	"  | Select descendants                                     %i" kCBSelectDescendantClassAction
	"  | Copy selected names    %k Meta-C                       %i" kCBCopyClassNameAction
	"%l| Find function...       %k Meta-F                       %i" kCBFindFunctionAction;

enum
{
	kEditCPPMacrosCmd = 1, kEditSearchPathsCmd, kUpdateCurrentCmd,
	kForceMinMILinksCmd,
	kTreeOpenSourceCmd, kTreeOpenHeaderCmd, kTreeOpenFnListCmd,
	kCreateDerivedClassCmd,
	kTreeCollapseCmd, kTreeExpandCmd, kTreeExpandAllCmd,
	kTreeSelParentsCmd, kTreeSelDescendantsCmd, kCopySelNamesCmd,
	kFindFnCmd
};

/******************************************************************************
 Constructor

 ******************************************************************************/

CBCTreeDirector::CBCTreeDirector
	(
	CBProjectDocument* supervisor
	)
	:
	CBTreeDirector(supervisor, NewCTree, "WindowTitleSuffix::CBCTreeDirector",
				   "CBCTreeHelp", jcc_c_tree_window,
				   kTreeMenuStr, "CBCTreeDirector",
				   kCBCTreeToolBarID, InitCTreeToolBar)
{
	CBCTreeDirectorX();
}

CBCTreeDirector::CBCTreeDirector
	(
	std::istream&		projInput,
	const JFileVersion	projVers,
	std::istream*		setInput,
	const JFileVersion	setVers,
	std::istream*		symInput,
	const JFileVersion	symVers,
	CBProjectDocument*	supervisor,
	const bool		subProject,
	CBDirList*			dirList
	)
	:
	CBTreeDirector(projInput, projVers, setInput, setVers, symInput, symVers,
				   supervisor, subProject, StreamInCTree,
				   "WindowTitleSuffix::CBCTreeDirector",
				   "CBCTreeHelp", jcc_c_tree_window,
				   kTreeMenuStr, "CBCTreeDirector",
				   kCBCTreeToolBarID, InitCTreeToolBar,
				   dirList, true)
{
	CBCTreeDirectorX();
}

// private

void
CBCTreeDirector::CBCTreeDirectorX()
{
	itsCTree = dynamic_cast<CBCTree*>(GetTree());
	assert( itsCTree != nullptr );

	itsEditCPPDialog = nullptr;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBCTreeDirector::~CBCTreeDirector()
{
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBCTreeDirector::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsEditCPPDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const auto* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful() &&
			itsEditCPPDialog->UpdateMacros(itsCTree->GetCPreprocessor()))
			{
			itsCTree->NextUpdateMustReparseAll();
			GetProjectDoc()->UpdateSymbolDatabase();
			}
		itsEditCPPDialog = nullptr;
		}

	else
		{
		CBTreeDirector::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateTreeMenu (virtual protected)

 ******************************************************************************/

void
CBCTreeDirector::UpdateTreeMenu()
{
	JXTextMenu* treeMenu = GetTreeMenu();

	treeMenu->EnableItem(kEditCPPMacrosCmd);
	treeMenu->EnableItem(kEditSearchPathsCmd);
	treeMenu->EnableItem(kUpdateCurrentCmd);

	if (!itsCTree->IsEmpty())
		{
		treeMenu->EnableItem(kFindFnCmd);
		treeMenu->EnableItem(kTreeExpandAllCmd);

		if (itsCTree->NeedsMinimizeMILinks())
			{
			treeMenu->EnableItem(kForceMinMILinksCmd);
			}
		}

	bool hasSelection, canCollapse, canExpand;
	itsCTree->GetMenuInfo(&hasSelection, &canCollapse, &canExpand);
	if (hasSelection)
		{
		treeMenu->EnableItem(kTreeOpenSourceCmd);
		treeMenu->EnableItem(kTreeOpenHeaderCmd);
		treeMenu->EnableItem(kTreeOpenFnListCmd);
		treeMenu->EnableItem(kCreateDerivedClassCmd);
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
CBCTreeDirector::HandleTreeMenu
	(
	const JIndex index
	)
{
	CBGetDocumentManager()->SetActiveProjectDocument(GetProjectDoc());

	if (index == kEditCPPMacrosCmd)
		{
		assert( itsEditCPPDialog == nullptr );
		itsEditCPPDialog =
			jnew CBEditCPPMacroDialog(this, *(itsCTree->GetCPreprocessor()));
		assert( itsEditCPPDialog != nullptr );
		itsEditCPPDialog->BeginDialog();
		ListenTo(itsEditCPPDialog);
		}
	else if (index == kEditSearchPathsCmd)
		{
		GetProjectDoc()->EditSearchPaths(this);
		}
	else if (index == kUpdateCurrentCmd)
		{
		GetProjectDoc()->UpdateSymbolDatabase();
		}
	else if (index == kForceMinMILinksCmd)
		{
		itsCTree->ForceMinimizeMILinks();
		}

	else if (index == kTreeOpenSourceCmd)
		{
		itsCTree->ViewSelectedSources();
		}
	else if (index == kTreeOpenHeaderCmd)
		{
		itsCTree->ViewSelectedHeaders();
		}
	else if (index == kTreeOpenFnListCmd)
		{
		itsCTree->ViewSelectedFunctionLists();
		}

	else if (index == kCreateDerivedClassCmd)
		{
		itsCTree->DeriveFromSelected();
		}

	else if (index == kTreeCollapseCmd)
		{
		itsCTree->CollapseExpandSelectedClasses(true);
		}
	else if (index == kTreeExpandCmd)
		{
		itsCTree->CollapseExpandSelectedClasses(false);
		}
	else if (index == kTreeExpandAllCmd)
		{
		itsCTree->ExpandAllClasses();
		}

	else if (index == kTreeSelParentsCmd)
		{
		itsCTree->SelectParents();
		}
	else if (index == kTreeSelDescendantsCmd)
		{
		itsCTree->SelectDescendants();
		}
	else if (index == kCopySelNamesCmd)
		{
		itsCTree->CopySelectedClassNames();
		}

	else if (index == kFindFnCmd)
		{
		AskForFunctionToFind();
		}
}

/******************************************************************************
 NewCTree (static private)

 ******************************************************************************/

CBTree*
CBCTreeDirector::NewCTree
	(
	CBTreeDirector*	director,
	const JSize		marginWidth
	)
{
	// dynamic_cast<> doesn't work because object is not fully constructed

	auto* cTreeDir = static_cast<CBCTreeDirector*>(director);
	assert( cTreeDir != nullptr );

	auto* tree = jnew CBCTree(cTreeDir, marginWidth);
	assert( tree != nullptr );
	return tree;
}

/******************************************************************************
 StreamInCTree (static private)

 ******************************************************************************/

CBTree*
CBCTreeDirector::StreamInCTree
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

	auto* cTreeDir = static_cast<CBCTreeDirector*>(director);
	assert( cTreeDir != nullptr );

	auto* tree = jnew CBCTree(projInput, projVers,
								setInput, setVers, symInput, symVers,
								cTreeDir, marginWidth, dirList);
	assert( tree != nullptr );
	return tree;
}

/******************************************************************************
 InitCTreeToolBar (static private)

 ******************************************************************************/

void
CBCTreeDirector::InitCTreeToolBar
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
