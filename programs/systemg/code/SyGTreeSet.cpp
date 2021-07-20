/******************************************************************************
 SyGTreeSet.cpp

	Maintains an enclosure for all the objects required by a CBCodeEditor.

	BASE CLASS = JXWidgetSet

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "SyGTreeSet.h"

#include "SyGHeaderWidget.h"
#include "SyGFileTreeTable.h"
#include "SyGFileTreeList.h"
#include "SyGFileTree.h"
#include "SyGFileTreeNode.h"
#include "SyGFilterInput.h"
#include "SyGPathInput.h"
#include "SyGPrefsMgr.h"
#include "SyGGlobals.h"

#include <JXDisplay.h>
#include <JXWindowDirector.h>
#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXScrollbarSet.h>
#include <JXStringHistoryMenu.h>
#include <JXCurrentPathMenu.h>
#include <JXCloseDirectorTask.h>
#include <JXFontManager.h>

#include <JDirInfo.h>
#include <jDirUtil.h>
#include <jAssert.h>

const JCoordinate kEmptyButtonWidth = 50;

/******************************************************************************
 Constructor

 ******************************************************************************/

SyGTreeSet::SyGTreeSet
	(
	JXMenuBar*			menuBar,
	const JString&		pathName,
	SyGPathInput*		pathInput,
	JXCurrentPathMenu*	pathMenu,
	SyGTrashButton*		trashButton,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXWidgetSet(enclosure, hSizing, vSizing, x,y, w,h)
{
	SyGTreeSetX(menuBar, pathName, pathInput, pathMenu, trashButton, w,h);
	itsTable->LoadPrefs();
	ShowFilter((SyGGetPrefsMgr())->GetFilterStatePref());
	itsTable->Focus();
}

SyGTreeSet::SyGTreeSet
	(
	std::istream&		input,
	const JFileVersion	vers,
	JXMenuBar*			menuBar,
	const JString&		pathName,
	SyGPathInput*		pathInput,
	JXCurrentPathMenu*	pathMenu,
	SyGTrashButton*		trashButton,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXWidgetSet(enclosure, hSizing, vSizing, x,y, w,h)
{
	if (vers < 2)
		{
		JFileVersion v;
		input >> v;

		bool showHidden;
		input >> JBoolFromString(showHidden);
		}

	SyGTreeSetX(menuBar, pathName, pathInput, pathMenu, trashButton, w,h);

	JString filter;
	input >> filter;
	itsFilterInput->GetText()->SetText(filter);

	itsFilterHistory->ReadSetup(input);
	itsTable->LoadPrefs(input, vers);

	bool showFilter;
	input >> JBoolFromString(showFilter);
	ShowFilter(showFilter);
	itsTable->Focus();

	itsTable->RestoreDirState(input, vers);

	// must be after directories have been opened

	if (vers >= 3)
		{
		itsTable->ReadScrollSetup(input);
		}
}

void SyGTreeSet::SyGTreeSetX
	(
	JXMenuBar*			menuBar,
	const JString&		pathName,
	SyGPathInput*		pathInput,
	JXCurrentPathMenu*	pathMenu,
	SyGTrashButton*		trashButton,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	itsMenuBar     = menuBar;
	itsEmptyButton = nullptr;

	const JFont& font              = JFontManager::GetDefaultMonospaceFont();
	const JCoordinate filterHeight = font.GetLineHeight(GetFontManager());
	const JCoordinate headerHeight = SyGHeaderWidget::GetPreferredHeight(GetFontManager());

	// file list -- created first so it gets focus by default

	itsScrollbarSet =
		jnew JXScrollbarSet(this, kHElastic, kVElastic,
							0, filterHeight, w, h - filterHeight);
	assert( itsScrollbarSet != nullptr );

	JString path = pathName;
	if (!JFSFileTreeNode::CanHaveChildren(path))
		{
		path = JGetRootDirectory();
		JXCloseDirectorTask::Close(GetWindow()->GetDirector());
		}

	auto* entry = jnew JDirEntry(path);
	assert( entry != nullptr && JFSFileTreeNode::CanHaveChildren(*entry) );
	auto* root = jnew SyGFileTreeNode(entry);
	assert( root != nullptr );
	itsFileTree = jnew SyGFileTree(root);
	assert( itsFileTree != nullptr );
	auto* treeList = jnew SyGFileTreeList(itsFileTree);
	assert( treeList != nullptr );

	JXContainer* encl = itsScrollbarSet->GetScrollEnclosure();
	JRect enclApG     = encl->GetApertureGlobal();

	itsTable = jnew SyGFileTreeTable(itsMenuBar, itsFileTree, treeList,
									 this, trashButton,
									 itsScrollbarSet, encl, kHElastic, kVElastic,
									 0, headerHeight,
									 enclApG.width(), enclApG.height()-headerHeight);
	assert( itsTable != nullptr );
	ListenTo(itsFileTree->GetRootDirInfo());

	auto* colHeader =
		jnew SyGHeaderWidget(itsTable, itsScrollbarSet, encl,
							 kHElastic, kFixedTop,
							 0,0, enclApG.width(),
							 headerHeight);
	assert(colHeader != nullptr);

	// header:  filter

	itsFilterLabel =
		jnew JXStaticText(JGetString("FilterLabel::SyGTreeSet"), this, kFixedLeft, kFixedTop,
						  5,0, 40, filterHeight);
	assert( itsFilterLabel != nullptr );
	itsFilterLabel->SetToLabel();

	itsFilterHistory =
		jnew JXStringHistoryMenu(10, JString::empty, this, kFixedRight, kFixedTop,
								 0,0, 30, filterHeight);
	assert( itsFilterHistory != nullptr );
	ListenTo(itsFilterHistory);
	itsFilterHistory->Place(w - itsFilterHistory->GetFrameWidth(), 0);
	itsFilterHistory->SetDefaultFont(font, true);

	itsFilterInput =
		jnew SyGFilterInput(itsTable, this, kHElastic, kFixedTop,
							45,0, w - 45 - itsFilterHistory->GetFrameWidth(), filterHeight);
	assert( itsFilterInput != nullptr );
	itsFilterInput->SetFont(font);

	// footer:  path input, drag source

	itsPathInput = pathInput;
	itsPathInput->SetDirList(itsTable);
	itsPathInput->GetText()->SetText(path);

	itsPathMenu = pathMenu;

	// after all instance variables initialized

	ListenTo(itsFilterInput);
	ListenTo(itsPathInput);
	ListenTo(itsPathMenu);

	// share Edit menu

	JXTEBase* te = itsTable->GetEditMenuHandler();
	JXTextMenu* menu;
	if (te->GetEditMenu(&menu))
		{
		itsPathInput->ShareEditMenu(te);
		itsFilterInput->ShareEditMenu(te);
		}

	// filter starts out hidden

	ShowFilter(false);
	UpdateDisplay(path);

	FitToEnclosure();	// must be called before ReadScrollSetup()

	SetNeedsInternalFTC();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SyGTreeSet::~SyGTreeSet()
{
	jdelete itsFileTree;
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
SyGTreeSet::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsPathInput && message.Is(JXWidget::kLostFocus))
		{
		GoToItsPath();
		}
	else if (sender == itsPathMenu && message.Is(JXMenu::kItemSelected))
		{
		itsTable->GoTo(itsPathMenu->GetPath(message),
					   (GetDisplay()->GetLatestKeyModifiers()).meta());
		}

	else if (sender == itsFilterInput && message.Is(JXWidget::kLostFocus))
		{
		SetWildcardFilter(itsFilterInput->GetText()->GetText());
		}
	else if (sender == itsFilterHistory && message.Is(JXMenu::kItemSelected))
		{
		SetWildcardFilter(itsFilterHistory->GetItemText(message));
		}

	else if (sender == itsEmptyButton && message.Is(JXButton::kPushed))
		{
		SyGEmptyTrashDirectory();
		}

	else if (sender == itsFileTree->GetRootDirInfo() &&
			 message.Is(JDirInfo::kPathChanged))
		{
		UpdateDisplay(itsFileTree->GetDirectory());
		}

	else
		{
		JXWidgetSet::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateDisplay

 ******************************************************************************/

void
SyGTreeSet::UpdateDisplay
	(
	const JString& path
	)
{
	// window title

	JString s = path, name;
	JStripTrailingDirSeparator(&s);

	const bool isTrashDir = SyGIsTrashDirectory(s);

	if (JIsRootDirectory(s))
		{
		name = s;
		}
	else if (isTrashDir)
		{
		name = JGetString("TrashName::SyGGlobals");
		}
	else
		{
		JString p;
		JSplitPathAndName(s, &p, &name);
		}
	GetWindow()->SetTitle(name);

	// path input

	const JString p = JConvertToHomeDirShortcut(path);
	itsPathInput->GetText()->SetText(p);

	// Empty button (for trash)

	JXContainer* encl   = itsMenuBar->GetEnclosure();
	const JCoordinate w = encl->GetBoundsWidth();
	if (isTrashDir && itsEmptyButton == nullptr)
		{
		itsMenuBar->SetSize(w - kEmptyButtonWidth, kJXDefaultMenuBarHeight);

		itsEmptyButton =
			jnew JXTextButton(JGetString("EmptyTrashLabel::SyGTreeSet"), encl, kFixedRight, kFixedTop,
							 w - kEmptyButtonWidth, 0,
							 kEmptyButtonWidth, kJXDefaultMenuBarHeight);
		assert( itsEmptyButton != nullptr );
		ListenTo(itsEmptyButton);
		}
	else if (!isTrashDir && itsEmptyButton != nullptr)
		{
		itsMenuBar->SetSize(w, kJXDefaultMenuBarHeight);

		jdelete itsEmptyButton;
		itsEmptyButton = nullptr;
		}
}

/******************************************************************************
 GoToItsPath

 ******************************************************************************/

void
SyGTreeSet::GoToItsPath()
{
	JString path;
	if (itsPathInput->GetPath(&path) &&
		!JSameDirEntry(path, itsFileTree->GetDirectory()))
		{
		const JError err = (itsFileTree->GetSyGRoot())->GoTo(path);
		err.ReportIfError();
		}
}

/******************************************************************************
 SetWildcardFilter

 ******************************************************************************/

void
SyGTreeSet::SetWildcardFilter
	(
	const JString& filter
	)
{
	const JString f = filter;		// use copy in case filter is reference from input or menu
	itsFilterInput->GetText()->SetText(f);
	itsFilterHistory->AddString(f);
	itsFileTree->SetWildcardFilter(f);
}

/******************************************************************************
 SavePreferences

 ******************************************************************************/

void
SyGTreeSet::SavePreferences
	(
	std::ostream& os
	)
{
	os << ' ' << itsFilterInput->GetText()->GetText();

	os << ' ';
	itsFilterHistory->WriteSetup(os);

	os << ' ';
	itsTable->SavePrefs(os);

	os << ' ' << JBoolToString(itsFilterInput->IsVisible());

	os << ' ';
	itsTable->SaveDirState(os);

	os << ' ';
	itsTable->WriteScrollSetup(os);

	os << ' ';
}

/******************************************************************************
 FilterVisible

 ******************************************************************************/

bool
SyGTreeSet::FilterVisible()
	const
{
	return itsFilterInput->IsVisible();
}

/******************************************************************************
 ShowFilter

 ******************************************************************************/

void
SyGTreeSet::ShowFilter
	(
	const bool show
	)
{
	const JCoordinate filterHeight = itsFilterInput->GetFrameHeight();

	if (show && !itsFilterInput->WouldBeVisible())
		{
		itsFilterInput->Show();
		itsFilterInput->Focus();
		itsFilterLabel->Show();
		itsFilterHistory->Show();
		itsScrollbarSet->Place(0, filterHeight);
		itsScrollbarSet->AdjustSize(0, -filterHeight);
		SetWildcardFilter(itsFilterInput->GetText()->GetText());
		}
	else if (!show && itsFilterInput->WouldBeVisible())
		{
		if (itsFilterInput->HasFocus())
			{
			itsTable->Focus();
			}

		itsFilterInput->Hide();
		itsFilterLabel->Hide();
		itsFilterHistory->Hide();
		itsScrollbarSet->Place(0, 0);
		itsScrollbarSet->AdjustSize(0,filterHeight);
		itsFileTree->ClearWildcardFilter();
		}
}

/******************************************************************************
 ToggleFilter

 ******************************************************************************/

void
SyGTreeSet::ToggleFilter()
{
	ShowFilter(!itsFilterInput->IsVisible());
}

/******************************************************************************
 SaveFilterPref

 ******************************************************************************/

void
SyGTreeSet::SaveFilterPref()
{
	SyGGetPrefsMgr()->SaveFilterStatePref(itsFilterInput->IsVisible());
}
