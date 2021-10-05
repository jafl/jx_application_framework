/******************************************************************************
 JXToolBar.cpp

	Implements a user-configurable button toolbar.  The basic idea is that
	one creates a JXToolBar below the JXMenuBar, and places one's own widgets
	inside the JXWidgetSet returned by JXToolBar::GetWidgetEnclosure().
	(This is similar to the way JXScrollbarSet works.)  Since the space
	required by the buttons changes its height when the window's width changes,
	one has to provide the minimum window size (excluding the toolbar's
	height) so the toolbar can call JXWindow::SetMinSize() to keep anything
	from shrinking to zero.

	After creating JXToolBar, one should call LoadPrefs().  If IsEmpty()
	returns true after this, then one should use AppendButton() and NewGroup()
	to set up a default toolbar.  A menu item should be provided to call
	Edit() so the user can change the configuration.

	Every menu item accessible from the given JXMenuBar that has an ID
	will be included in the editor dialog.  If the menu item has an image,
	the "show image" and "show text" checkboxes will control what is displayed
	on the button.  Otherwise, the menu item's text will always be displayed.

	BASE CLASS = public JXWidgetSet, JPrefObject

	Copyright (C) 1998 by Glenn W. Bach.

 *****************************************************************************/

#include "jx-af/jx/JXToolBar.h"
#include "jx-af/jx/JXToolBarEditDir.h"
#include "jx-af/jx/JXToolBarNode.h"
#include "jx-af/jx/JXAdjustToolBarGeometryTask.h"

#include <jx-af/jcore/JTree.h>
#include <jx-af/jcore/JNamedTreeNode.h>

#include "jx-af/jx/JXFontManager.h"
#include "jx-af/jx/JXTextMenu.h"
#include "jx-af/jx/JXTextMenuData.h"
#include "jx-af/jx/JXImage.h"
#include "jx-af/jx/JXTimerTask.h"
#include "jx-af/jx/JXApplication.h"
#include "jx-af/jx/JXDisplay.h"
#include "jx-af/jx/JXWindow.h"
#include "jx-af/jx/JXDirector.h"
#include "jx-af/jx/JXMenuBar.h"
#include "jx-af/jx/jXGlobals.h"

#include <jx-af/jcore/JString.h>
#include <sstream>
#include <algorithm>
#include <jx-af/jcore/jAssert.h>

const JCoordinate kButConBuffer = 5;

const JSize kTimerDelay = 1000;

const JSize kSmallButtonHeight = 24;
const JSize kMedButtonHeight   = 30;

const JFileVersion kCurrentPrefsVersion	= 2;

// JBroadcaster message types

const JUtf8Byte* JXToolBar::kWantsToDrop = "WantsToDrop::JXToolBar";
const JUtf8Byte* JXToolBar::kHandleDrop  = "HandleDrop::JXToolBar";

/******************************************************************************
 Constructor

 *****************************************************************************/

JXToolBar::JXToolBar
	(
	JPrefsManager*		prefsMgr,
	const JPrefID&		id,
	JXMenuBar*			menuBar,
	JXContainer*		enclosure,
	const HSizingOption hSizing,
	const VSizingOption vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXWidgetSet(enclosure, hSizing, vSizing, x, y, w, h),
	JPrefObject(prefsMgr, id),
	itsNextButtonPosition(kButConBuffer),
	itsInNewGroupMode(true),
	itsEditDialog(nullptr),
	itsMenuTree(nullptr),
	itsMenuBar(menuBar),
	itsCurrentButtonHeight(kSmallButtonHeight),
	itsIsShowingButtons(true),
	itsWasShowingButtons(false),
	itsButtonType(JXToolBarButton::kImage),
	itsLoadedPrefs(false)
{
	assert(h >= 40);

	itsButtons = jnew JPtrArray<JXToolBarButton>(JPtrArrayT::kForgetAll);
	assert(itsButtons != nullptr);

	itsMenus = jnew JPtrArray<JXMenu>(JPtrArrayT::kForgetAll);
	assert(itsMenus != nullptr);

	const JCoordinate barHeight = itsCurrentButtonHeight + 2* kButConBuffer;

	itsToolBarSet =
		jnew JXWidgetSet(this,
			JXWidget::kHElastic, JXWidget::kFixedTop,
			0,0, w,barHeight);
	assert(itsToolBarSet != nullptr);

	itsToolBarEnclosure =
		jnew JXWidgetSet(this,
			JXWidget::kHElastic, JXWidget::kVElastic,
			0,barHeight, w,h-barHeight);
	assert(itsToolBarEnclosure != nullptr);

	itsGroupStarts = jnew JArray<bool>;
	assert(itsGroupStarts != nullptr);

	itsTimerTask = jnew JXTimerTask(kTimerDelay);
	assert(itsTimerTask != nullptr);
	itsTimerTask->Start();
	ListenTo(itsTimerTask);

	itsAdjustTask = jnew JXAdjustToolBarGeometryTask(this);
	assert( itsAdjustTask != nullptr );
	itsAdjustTask->Go();

	ListenTo(prefsMgr);
	ListenTo(GetWindow());

	SetNeedsInternalFTC();
	itsToolBarEnclosure->SetNeedsInternalFTC();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JXToolBar::~JXToolBar()
{
	jdelete itsGroupStarts;
	jdelete itsMenuTree;
	jdelete itsTimerTask;
	jdelete itsAdjustTask;
	jdelete itsButtons;
	jdelete itsMenus;
}

/******************************************************************************
 LoadPrefs (public)

 ******************************************************************************/

void
JXToolBar::LoadPrefs()
{
	JPrefObject::ReadPrefs();
}

/******************************************************************************
 AppendButton

	This should only be called by the client at startup if there are
	no preferences to read in.

 ******************************************************************************/

void
JXToolBar::AppendButton
	(
	JXTextMenu*		menu,
	const JIndex	index
	)
{
	const JString* itemID = nullptr;
	if (menu->GetItemID(index, &itemID))
	{
		itsGroupStarts->AppendElement(itsInNewGroupMode);

		itsInNewGroupMode = false;

		auto* butcon =
			jnew JXToolBarButton(this, menu, *itemID, itsButtonType, itsToolBarSet,
								kFixedLeft, kFixedTop,
								itsNextButtonPosition, kButConBuffer,
								itsCurrentButtonHeight);
		assert(butcon != nullptr);
		ListenTo(butcon);
		itsButtons->Append(butcon);
		itsNextButtonPosition += butcon->GetFrameWidth();

		JIndex findex;
		if (!itsMenus->Find(menu, &findex))
		{
			itsMenus->Append(menu);
		}
	}
}

void
JXToolBar::AppendButton
	(
	JXTextMenu*		menu,
	const JString&	id
	)
{
	JIndex index;
	if (menu->ItemIDToIndex(id, &index))
	{
		AppendButton(menu, index);
	}
}

/******************************************************************************
 NewGroup

 ******************************************************************************/

void
JXToolBar::NewGroup()
{
	if (itsInNewGroupMode)
	{
		return;
	}
	itsNextButtonPosition  += kButConBuffer;
	itsInNewGroupMode		= true;
}

/******************************************************************************
 Receive

 ******************************************************************************/

void
JXToolBar::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	bool propagate = true;

	JPrefsManager* prefsMgr;
	JIndex prefID;
	const bool hasPrefs = GetPrefInfo(&prefsMgr, &prefID);

	if (message.Is(JXButton::kPushed))
	{
		auto* button = dynamic_cast<JXToolBarButton*>(sender);
		if (itsButtons->Includes(button))
		{
			JXTextMenu* menu = button->GetMenu();
			if (menu->IsActive())
			{
				menu->PrepareToOpenMenu(true);
				JIndex itemIndex;
				if (button->GetMenuItemIndex(&itemIndex) &&
					menu->IsEnabled(itemIndex))
				{
					JXDisplay* display = GetDisplay();	// need local copy, since we might be deleted
					Display* xDisplay  = *display;
					Window xWindow     = GetWindow()->GetXWindow();

					menu->BroadcastSelection(itemIndex, false);

					if (!JXDisplay::WindowExists(display, xDisplay, xWindow))
					{
						// we have been deleted
						return;
					}

					UpdateButtons();
				}
			}
			propagate = false;
		}
	}

	else if (sender == itsTimerTask && message.Is(JXTimerTask::kTimerWentOff))
	{
		UpdateButtons();
		propagate = false;
	}

	else if (sender == itsEditDialog && message.Is(JXDialogDirector::kDeactivated))
	{
		const auto* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert(info != nullptr);
		if (info->Successful())
		{
			ExtractChanges();
		}
		jdelete itsMenuTree;
		itsMenuTree = nullptr;
		itsEditDialog = nullptr;
		propagate = false;
	}
	else if (hasPrefs &&
			 sender == prefsMgr && message.Is(JPrefsManager::kDataChanged))
	{
		const auto* info =
			dynamic_cast<const JPrefsManager::DataChanged*>(&message);
		assert(info != nullptr);
		if (info->GetID() == prefID)
		{
			JPrefObject::ReadPrefs();
			propagate = false;
		}
	}

	else if (sender == GetWindow() && message.Is(JXWindow::kIconified))
	{
		itsTimerTask->Stop();
	}
	else if (itsIsShowingButtons &&
			 sender == GetWindow() && message.Is(JXWindow::kDeiconified))
	{
		itsTimerTask->Start();
	}

	// If nobody else handled it, pass it to the base class.

	if (propagate)
	{
		JXWidgetSet::Receive(sender, message);
	}
}

/******************************************************************************
 Edit (public)

 ******************************************************************************/

void
JXToolBar::Edit()
{
	assert(itsEditDialog == nullptr);

	BuildTree();

	bool small = itsCurrentButtonHeight == kSmallButtonHeight;

	itsEditDialog =
		jnew JXToolBarEditDir(itsMenuTree, itsIsShowingButtons,
							 small, itsButtonType,
							 this->GetWindow()->GetDirector());
	assert(itsEditDialog != nullptr);

	if (!itsDialogPrefs.IsEmpty())
	{
		const std::string s(itsDialogPrefs.GetBytes(), itsDialogPrefs.GetByteCount());
		std::istringstream is(s);
		itsEditDialog->ReadSetup(is);
	}
	itsEditDialog->BeginDialog();
	ListenTo(itsEditDialog);
}

/******************************************************************************
 ExtractChanges (private)

 ******************************************************************************/

void
JXToolBar::ExtractChanges()
{
	itsButtons->DeleteAll();
	itsGroupStarts->RemoveAll();

	SetButtonType(itsEditDialog->GetType());
	UseSmallButtons(itsEditDialog->UseSmallButtons());

	itsInNewGroupMode		= true;
	itsNextButtonPosition	= kButConBuffer;
	JTreeNode* base = itsMenuTree->GetRoot();
	JSize menuCount = base->GetChildCount();
	for (JIndex i=1; i<=menuCount; i++)
	{
		JTreeNode* node = base->GetChild(i);
		ExtractItemNodes(node);
	}

	AdjustToolBarGeometry();
	ShowToolBar(itsEditDialog->ShowToolBar());

	std::ostringstream data;
	itsEditDialog->WriteSetup(data);
	itsDialogPrefs = data.str();

	JPrefObject::WritePrefs();
}

/******************************************************************************
 ExtractItemNodes (public)

 ******************************************************************************/

void
JXToolBar::ExtractItemNodes
	(
	JTreeNode* menuNode
	)
{
	NewGroup();

	const JSize itemCount = menuNode->GetChildCount();
	for (JIndex i=1; i<=itemCount; i++)
	{
		JTreeNode* child = menuNode->GetChild(i);
		if (child->IsOpenable())
		{
			ExtractItemNodes(child);
			NewGroup();
		}
		else
		{
			auto* item	= dynamic_cast<JXToolBarNode*>(child);
			assert(item != nullptr);
			if (item->IsChecked())
			{
				AppendButton(item->GetMenu(), item->GetIndex());
			}
			if (item->HasSeparator())
			{
				NewGroup();
			}
		}
	}
}

/******************************************************************************
 BuildTree (private)

 ******************************************************************************/

void
JXToolBar::BuildTree()
{
	assert(itsMenuTree == nullptr);
	auto* base = jnew JNamedTreeNode(nullptr, JString("BASE", JString::kNoCopy));
	assert(base != nullptr);
	itsMenuTree = jnew JTree(base);
	assert(itsMenuTree != nullptr);

	const JSize count = itsMenuBar->GetMenuCount();
	for (JIndex i=1; i<=count; i++)
	{
		JXMenu* menu		= itsMenuBar->GetMenu(i);
		auto* tmenu	= dynamic_cast<JXTextMenu*>(menu);
		assert(tmenu != nullptr);
		AddMenuToTree(tmenu, base, tmenu->GetTitleText());
	}
}

/******************************************************************************
 AddMenuToTree (private)

 ******************************************************************************/

void
JXToolBar::AddMenuToTree
	(
	JXTextMenu*		menu,
	JNamedTreeNode*	parent,
	const JString&	name
	)
{
	auto* mnode = jnew JNamedTreeNode(parent->GetTree(), name);
	assert(mnode != nullptr);
	parent->Append(mnode);
	JSize itemCount = menu->GetItemCount();
	for (JIndex i=1; i<=itemCount; i++)
	{
		const JString& name1 = menu->GetItemText(i);
		const JXMenu* sub;
		if (menu->GetSubmenu(i, &sub))
		{
			const auto* temp	= dynamic_cast<const JXTextMenu*>(sub);
			auto* tsub = const_cast<JXTextMenu*>(temp);
			AddMenuToTree(tsub, mnode, name1);
		}
		else
		{
			const bool separator = menu->HasSeparatorAfter(i);
			const bool checked   = ItemIsUsed(menu, i);

			const JString* id;
			if (menu->GetItemID(i, &id))
			{
				auto* tbnode =
					jnew JXToolBarNode(menu, i, separator, checked,
									  itsMenuTree, mnode, name1);
				assert(tbnode != nullptr);
			}
		}
	}

	if (mnode->GetChildCount() == 0)
	{
		jdelete mnode;
	}
}

/******************************************************************************
 ItemIsUsed (private)

 ******************************************************************************/

bool
JXToolBar::ItemIsUsed
	(
	JXTextMenu*		menu,
	const JIndex	index
	)
{
	return std::any_of(begin(*itsButtons), end(*itsButtons),
			[menu,index] (JXToolBarButton* button)
		{
				JIndex j;
				return (button->GetMenu() == menu &&
						button->GetMenuItemIndex(&j) && j == index);
		});
}

/******************************************************************************
 ReadPrefs

 ******************************************************************************/

void
JXToolBar::ReadPrefs
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers > kCurrentPrefsVersion)
	{
		return;
	}

	itsLoadedPrefs = true;
	itsButtons->DeleteAll();
	itsGroupStarts->RemoveAll();

	bool show;
	input >> JBoolFromString(show);
	ShowToolBar(show);
	bool useSmall;
	input >> JBoolFromString(useSmall);
	UseSmallButtons(useSmall);
	if (vers == 1)
	{
		bool textOnly;
		input >> JBoolFromString(textOnly);
		if (textOnly)
		{
			SetButtonType(JXToolBarButton::kText);
		}
	}
	else
	{
		JIndex type;
		input >> type;
		SetButtonType((JXToolBarButton::Type)type);
	}

	JSize count;
	input >> count;

	for (JIndex i = 1; i <= count; i++)
	{
		JString id;
		input >> id;
		bool group;
		input >> JBoolFromString(group);
		if (!id.IsEmpty())
		{
			if (group)
			{
				NewGroup();
			}
			FindItemAndAdd(id);
		}
	}

	input >> itsDialogPrefs;

	AdjustToolBarGeometry();
}

/******************************************************************************
 WritePrefs

 ******************************************************************************/

void
JXToolBar::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentPrefsVersion;
	output << ' ' << JBoolToString(itsIsShowingButtons)
				  << JBoolToString(itsCurrentButtonHeight == kSmallButtonHeight);
	output << ' ' << (JIndex) itsButtonType;

	const JSize count = itsButtons->GetElementCount();
	output << ' ' << count;

	for (JIndex i=1; i<=count; i++)
	{
		output << ' ' << (itsButtons->GetElement(i))->GetMenuItemID();
		output << ' ' << JBoolToString(itsGroupStarts->GetElement(i));
	}

	output << ' ' << itsDialogPrefs;
}

/******************************************************************************
 FindItemAndAdd (private)

 ******************************************************************************/

void
JXToolBar::FindItemAndAdd
	(
	const JString& id
	)
{
	const JSize count = itsMenuBar->GetMenuCount();
	for (JIndex i = 1; i <= count; i++)
	{
		JXMenu* menu		= itsMenuBar->GetMenu(i);
		auto* tmenu	= dynamic_cast<JXTextMenu*>(menu);
		assert(tmenu != nullptr);
		FindItemAndAdd(tmenu, id);
	}
}

void
JXToolBar::FindItemAndAdd
	(
	JXTextMenu*	menu,
	const JString&	id
	)
{
	const JString* testID;

	const JSize count = menu->GetItemCount();
	for (JIndex i = 1; i <= count; i++)
	{
		const JXMenu* sub;
		if (menu->GetSubmenu(i, &sub))
		{
			const auto* temp	= dynamic_cast<const JXTextMenu*>(sub);
			auto* tsub = const_cast<JXTextMenu*>(temp);
			FindItemAndAdd(tsub, id);
		}
		else if (menu->GetItemID(i, &testID) && *testID == id)
		{
			AppendButton(menu, i);
			return;
		}
	}
}

/******************************************************************************
 IsEmpty (public)

 ******************************************************************************/

bool
JXToolBar::IsEmpty()
	const
{
	return itsButtons->IsEmpty();
}

/******************************************************************************
 AdjustGeometryIfNeeded (private)

	Called by JXAdjustToolBarGeometryTask.

 ******************************************************************************/

void
JXToolBar::AdjustGeometryIfNeeded()
{
	if (!itsLoadedPrefs)
	{
		AdjustToolBarGeometry();
	}
}

/******************************************************************************
 AdjustToolBarGeometry (public)

 ******************************************************************************/

void
JXToolBar::AdjustToolBarGeometry()
{
	if (itsIsShowingButtons)
	{
		RevertBar();

		itsNextButtonPosition	= kButConBuffer;
		itsCurrentLineY			= kButConBuffer;

		JArray<JIndexRange> groups;
		GetGroups(&groups);
		for (const auto& r : groups)
		{
			PlaceGroup(r);
		}

		AdjustWindowMinSize();

		itsToolBarEnclosure->Place(0,itsToolBarSet->GetBoundsHeight());
		itsToolBarEnclosure->SetSize(GetBoundsWidth(), GetBoundsHeight() - itsToolBarSet->GetBoundsHeight());
	}
}

/******************************************************************************
 GetGroups (private)

 ******************************************************************************/

void
JXToolBar::GetGroups
	(
	JArray<JIndexRange>* groups
	)
{
	if (IsEmpty())
	{
		return;
	}
	JIndexRange range;
	range.first = 1;
	JSize count = itsButtons->GetElementCount();
	for (JSize i = 2; i <= count; i++)
	{
		if (itsGroupStarts->GetElement(i))
		{
			range.last = i - 1;
			groups->AppendElement(range);
			range.first = i;
		}
	}
	range.last = count;
	groups->AppendElement(range);
}

/******************************************************************************
 PlaceGroup (private)

 ******************************************************************************/

void
JXToolBar::PlaceGroup
	(
	const JIndexRange& range
	)
{
	JCoordinate groupWidth = GetGroupWidth(range);
	if (groupWidth <= GetBoundsWidth())
	{
		JCoordinate groupEnd = itsNextButtonPosition + groupWidth;
		if (groupEnd > GetBoundsWidth())
		{
			NewLine();
		}
		for (JSize i = range.first; i <= range.last; i++)
		{
			PlaceButton(i);
		}
	}
	else
	{
		for (JSize i = range.first; i <= range.last; i++)
		{
			JXToolBarButton* button = itsButtons->GetElement(i);
			JCoordinate butWidth	= button->GetFrameWidth();
			JCoordinate totalWidth	= GetBoundsWidth();
			if (itsNextButtonPosition + butWidth <= totalWidth)
			{
				PlaceButton(i);
			}
			else if (butWidth > totalWidth)
			{
				if (itsNextButtonPosition != kButConBuffer)
				{
					NewLine();
				}
				PlaceButton(i);
			}
			else
			{
				NewLine();
				PlaceButton(i);
			}
		}
	}
	itsNextButtonPosition += kButConBuffer;
}

/******************************************************************************
 PlaceButton (private)

 ******************************************************************************/

void
JXToolBar::PlaceButton
	(
	const JIndex index
	)
{
	JXToolBarButton* button = itsButtons->GetElement(index);
	button->Place(itsNextButtonPosition, itsCurrentLineY);
	itsNextButtonPosition += button->GetFrameWidth();
}

/******************************************************************************
 GetGroupWidth (private)

 ******************************************************************************/

JSize
JXToolBar::GetGroupWidth
	(
	const JIndexRange& range
	)
{
	JSize w = 0;
	for (JSize index = range.first; index <= range.last; index++)
	{
		JXToolBarButton* button = itsButtons->GetElement(index);
		w += button->GetFrameWidth();
	}
	return w;
}

/******************************************************************************
 RevertBar (private)

 ******************************************************************************/

void
JXToolBar::RevertBar()
{
	const JCoordinate delta = itsToolBarSet->GetBoundsHeight()
							  - itsCurrentButtonHeight - 2*kButConBuffer;
	itsToolBarSet->AdjustSize(0, -delta);
//	itsToolBarEnclosure->Move(0, -delta);
//	itsToolBarEnclosure->AdjustSize(0, delta);
}

/******************************************************************************
 NewLine (private)

 ******************************************************************************/

void
JXToolBar::NewLine()
{
	itsNextButtonPosition	= kButConBuffer;
	const JCoordinate delta = kButConBuffer + itsCurrentButtonHeight;
	itsCurrentLineY			= itsCurrentLineY + delta;
	itsToolBarSet->AdjustSize(0, delta);
//	itsToolBarEnclosure->Move(0, delta);
//	itsToolBarEnclosure->AdjustSize(0, -delta);
}

/******************************************************************************
 UseSmallButtons (public)

	This should only be called by the client at startup if there are
	no preferences to read in. It should be called before adding any
	buttons.

 ******************************************************************************/

void
JXToolBar::UseSmallButtons
	(
	const bool useSmall
	)
{
	if (useSmall)
	{
		itsCurrentButtonHeight = kSmallButtonHeight;
	}
	else
	{
		itsCurrentButtonHeight = kMedButtonHeight;
	}
}

bool
JXToolBar::IsUsingSmallButtons()
	const
{
	return itsCurrentButtonHeight == kSmallButtonHeight;
}

/******************************************************************************
 ShowToolBar (public)

 ******************************************************************************/

void
JXToolBar::ShowToolBar
	(
	const bool show
	)
{
	if (show && !itsIsShowingButtons)
	{
		itsToolBarSet->Show();
		itsIsShowingButtons = true;
		AdjustWindowMinSize();

		itsToolBarEnclosure->Place(0,itsToolBarSet->GetBoundsHeight());
		itsToolBarEnclosure->SetSize(GetBoundsWidth(), GetBoundsHeight() - itsToolBarSet->GetBoundsHeight());

		if (!GetWindow()->IsIconified())
		{
			itsTimerTask->Start();
		}
	}
	else if (!show && itsIsShowingButtons)
	{
		itsToolBarSet->Hide();
		itsIsShowingButtons = false;
		AdjustWindowMinSize();

		itsToolBarEnclosure->Place(0,0);
		itsToolBarEnclosure->SetSize(GetBoundsWidth(), GetBoundsHeight());

		itsTimerTask->Stop();
	}
}

/******************************************************************************
 ApertureResized (protected)

 ******************************************************************************/

void
JXToolBar::ApertureResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXWidgetSet::ApertureResized(dw, dh);
	AdjustToolBarGeometry();
}

/******************************************************************************
 SetButtonType (public)

	Must be called before adding any buttons.

 ******************************************************************************/

void
JXToolBar::SetButtonType
	(
	const JXToolBarButton::Type type
	)
{
	itsButtonType = type;
}

JXToolBarButton::Type
JXToolBar::GetButtonType()
	const
{
	return itsButtonType;
}

/******************************************************************************
 UpdateButtons (private)

 ******************************************************************************/

void
JXToolBar::UpdateButtons()
{
	for (auto* menu : *itsMenus)
	{
		if (menu->IsActive())
		{
			menu->PrepareToOpenMenu(true);
		}
	}

	bool needsAdjustment = false;
	const JSize count = itsButtons->GetElementCount();
	for (JIndex i=count; i>=1; i--)
	{
		JXToolBarButton* button = itsButtons->GetElement(i);
		JIndex itemIndex;
		if (button->GetMenuItemIndex(&itemIndex) &&
			button->GetMenu()->IsActive()        &&
			button->GetMenu()->IsEnabled(itemIndex))
		{
			button->Activate();
		}
		else
		{
			button->Deactivate();
		}

		bool invalid;
		if (button->NeedsGeometryAdjustment(&invalid))
		{
			needsAdjustment = true;
		}

		if (invalid)
		{
			itsButtons->DeleteElement(i);
			needsAdjustment = true;
		}
	}

	if (needsAdjustment)
	{
		AdjustToolBarGeometry();
	}
}

/******************************************************************************
 AdjustWindowMinSize (private)

 ******************************************************************************/

void
JXToolBar::AdjustWindowMinSize()
{
	JXWindow* w = GetWindow();
	JPoint p    = w->GetMinSize();

	bool changed = false;
	if (itsIsShowingButtons && !itsWasShowingButtons)
	{
		p.y    += itsToolBarSet->GetFrameHeight();
		changed = true;
	}
	else if (!itsIsShowingButtons && itsWasShowingButtons)
	{
		p.y    -= itsToolBarSet->GetFrameHeight();
		changed = true;
	}

	itsWasShowingButtons = itsIsShowingButtons;
	if (changed)
	{
		w->SetMinSize(p.x, p.y);	// causes recursion
	}
}
