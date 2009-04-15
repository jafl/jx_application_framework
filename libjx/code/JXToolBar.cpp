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
	returns kJTrue after this, then one should use AppendButton() and NewGroup()
	to set up a default toolbar.  A menu item should be provided to call
	Edit() so the user can change the configuration.

	Every menu item accessible from the given JXMenuBar that has an ID
	will be included in the editor dialog.  If the menu item has an image,
	the "show image" and "show text" checkboxes will control what is displayed
	on the button.  Otherwise, the menu item's text will always be displayed.

	BASE CLASS = public JXWidgetSet, JPrefObject

	Copyright © 1998 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#include <JXStdInc.h>
#include <JXToolBar.h>
#include <JXToolBarEditDir.h>
#include <JXToolBarNode.h>
#include <JXAdjustToolBarGeometryTask.h>

#include <JTree.h>
#include <JNamedTreeNode.h>

#include <JXFontManager.h>
#include <JXTextMenu.h>
#include <JXTextMenuData.h>
#include <JXImage.h>
#include <JXTimerTask.h>
#include <JXApplication.h>
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXDirector.h>
#include <JXMenuBar.h>
#include <jXGlobals.h>

#include <JString.h>
#include <sstream>
#include <jAssert.h>

const JCoordinate kButConBuffer = 5;
const JCoordinate kLabelBuffer  = 10;

const JSize kTimerDelay = 1000;

const JSize kSmallButtonHeight = 24;
const JSize kMedButtonHeight   = 30;

const JFileVersion kCurrentPrefsVersion	= 2;

// JBroadcaster message types

const JCharacter* JXToolBar::kWantsToDrop = "WantsToDrop::JXToolBar";
const JCharacter* JXToolBar::kHandleDrop  = "HandleDrop::JXToolBar";

/******************************************************************************
 Constructor

 *****************************************************************************/

JXToolBar::JXToolBar
	(
	JPrefsManager*		prefsMgr,
	const JPrefID&		id,
	JXMenuBar*			menuBar,
	const JCoordinate	minWidth,
	const JCoordinate	minHeight,
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
	itsInNewGroupMode(kJTrue),
	itsEditDialog(NULL),
	itsMenuTree(NULL),
	itsMenuBar(menuBar),
	itsCurrentButtonHeight(kSmallButtonHeight),
	itsIsShowingButtons(kJTrue),
	itsButtonType(JXToolBarButton::kImage),
	itsLoadedPrefs(kJFalse),
	itsWindowMinWidth(minWidth),
	itsWindowMinHeight(minHeight)
{
	assert(h >= 40);

	itsButtons = new JPtrArray<JXToolBarButton>(JPtrArrayT::kForgetAll);
	assert(itsButtons != NULL);

	itsMenus = new JPtrArray<JXMenu>(JPtrArrayT::kForgetAll);
	assert(itsMenus != NULL);

	JCoordinate barHeight = itsCurrentButtonHeight + 2* kButConBuffer;

	itsToolBarSet =
		new JXWidgetSet(this,
			JXWidget::kHElastic, JXWidget::kFixedTop,
			0,0,w, barHeight);
	assert(itsToolBarSet != NULL);

	itsToolBarEnclosure =
		new JXWidgetSet(this,
			JXWidget::kHElastic, JXWidget::kVElastic,
			0,barHeight,w,h - barHeight);
	assert(itsToolBarEnclosure != NULL);

	itsGroupStarts = new JArray<JBoolean>;
	assert(itsGroupStarts != NULL);

	itsTimerTask = new JXTimerTask(kTimerDelay);
	assert(itsTimerTask != NULL);
	JXGetApplication()->InstallIdleTask(itsTimerTask);
	ListenTo(itsTimerTask);

	itsAdjustTask = new JXAdjustToolBarGeometryTask(this);
	assert( itsAdjustTask != NULL );
	JXGetApplication()->InstallUrgentTask(itsAdjustTask);

	ListenTo(prefsMgr);
	ListenTo(GetWindow());
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JXToolBar::~JXToolBar()
{
	delete itsGroupStarts;
	delete itsMenuTree;
	delete itsTimerTask;
	delete itsAdjustTask;
	delete itsButtons;
	delete itsMenus;
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
	const JString* itemID = NULL;
	if (menu->GetItemID(index, &itemID))
		{
		itsGroupStarts->AppendElement(itsInNewGroupMode);

		itsInNewGroupMode = kJFalse;

		JXToolBarButton* butcon =
			new JXToolBarButton(this, menu, *itemID, itsButtonType, itsToolBarSet,
								kFixedLeft, kFixedTop,
								itsNextButtonPosition, kButConBuffer,
								itsCurrentButtonHeight);
		assert(butcon != NULL);
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
	JXTextMenu*			menu,
	const JCharacter*	id
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
	itsInNewGroupMode		= kJTrue;
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
	JBoolean propagate = kJTrue;

	JPrefsManager* prefsMgr;
	JIndex prefID;
	const JBoolean hasPrefs = GetPrefInfo(&prefsMgr, &prefID);

	if (message.Is(JXButton::kPushed))
		{
		const JSize count = itsButtons->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			JXToolBarButton* button = itsButtons->NthElement(i);
			if (sender == button)
				{
				JXTextMenu* menu = button->GetMenu();
				menu->PrepareToOpenMenu();
				JIndex itemIndex;
				if (button->GetMenuItemIndex(&itemIndex) &&
					menu->itsBaseItemData->IsEnabled(itemIndex))
					{
					JXDisplay* display = GetDisplay();	// need local copy, since we might be deleted
					Display* xDisplay  = *display;
					Window xWindow     = (GetWindow())->GetXWindow();

					menu->BroadcastSelection(itemIndex, kJFalse);

					if (!JXDisplay::WindowExists(display, xDisplay, xWindow))
						{
						// we have been deleted
						return;
						}

					UpdateButtons();
					}
				propagate = kJFalse;
				break;
				}
			}
		}

	else if (sender == itsTimerTask && message.Is(JXTimerTask::kTimerWentOff))
		{
		UpdateButtons();
		propagate = kJFalse;
		}

	else if (sender == itsEditDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast(const JXDialogDirector::Deactivated*, &message);
		assert(info != NULL);
		if (info->Successful())
			{
			ExtractChanges();
			}
		delete itsMenuTree;
		itsMenuTree = NULL;
		itsEditDialog = NULL;
		propagate = kJFalse;
		}
	else if (hasPrefs &&
			 sender == prefsMgr && message.Is(JPrefsManager::kDataChanged))
		{
		const JPrefsManager::DataChanged* info =
			dynamic_cast(const JPrefsManager::DataChanged*, &message);
		assert(info != NULL);
		if (info->GetID() == prefID)
			{
			JPrefObject::ReadPrefs();
			propagate = kJFalse;
			}
		}

	else if (sender == GetWindow() && message.Is(JXWindow::kIconified))
		{
		(JXGetApplication())->RemoveIdleTask(itsTimerTask);
		}
	else if (itsIsShowingButtons &&
			 sender == GetWindow() && message.Is(JXWindow::kDeiconified))
		{
		(JXGetApplication())->InstallIdleTask(itsTimerTask);
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
	assert(itsEditDialog == NULL);

	BuildTree();

	JBoolean small = JI2B(itsCurrentButtonHeight == kSmallButtonHeight);

	itsEditDialog =
		new JXToolBarEditDir(itsMenuTree, itsIsShowingButtons,
							 small, itsButtonType,
							 this->GetWindow()->GetDirector());
	assert(itsEditDialog != NULL);

	if (!itsDialogPrefs.IsEmpty())
		{
		const std::string s(itsDialogPrefs.GetCString(), itsDialogPrefs.GetLength());
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

	itsInNewGroupMode		= kJTrue;
	itsNextButtonPosition	= kButConBuffer;
	JTreeNode* base = itsMenuTree->GetRoot();
	JSize menuCount = base->GetChildCount();
	for (JSize i = 1; i <= menuCount; i++)
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
			JXToolBarNode* item	= dynamic_cast(JXToolBarNode*, child);
			assert(item != NULL);
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
	assert(itsMenuTree == NULL);
	JNamedTreeNode* base = new JNamedTreeNode(NULL, "BASE");
	assert(base != NULL);
	itsMenuTree = new JTree(base);
	assert(itsMenuTree != NULL);

	const JSize count = itsMenuBar->GetMenuCount();
	for (JIndex i=1; i<=count; i++)
		{
		JXMenu* menu		= itsMenuBar->GetMenu(i);
		JXTextMenu* tmenu	= dynamic_cast(JXTextMenu*, menu);
		assert(tmenu != NULL);
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
	const JCharacter*	name
	)
{
	JNamedTreeNode* mnode = new JNamedTreeNode(parent->GetTree(), name);
	assert(mnode != NULL);
	parent->Append(mnode);
	JSize itemCount = menu->GetItemCount();
	for (JIndex i=1; i<=itemCount; i++)
		{
		const JString& name = menu->GetItemText(i);
		const JXMenu* sub;
		if (menu->GetSubmenu(i, &sub))
			{
			const JXTextMenu* temp	= dynamic_cast(const JXTextMenu*, sub);
			JXTextMenu* tsub = const_cast<JXTextMenu*>(temp);
			AddMenuToTree(tsub, mnode, name);
			}
		else
			{
			const JBoolean separator = menu->HasSeparatorAfter(i);
			const JBoolean checked   = ItemIsUsed(menu, i);

			const JString* id;
			if (menu->GetItemID(i, &id))
				{
				JXToolBarNode* tbnode =
					new JXToolBarNode(menu, i, separator, checked,
									  itsMenuTree, mnode, name);
				assert(tbnode != NULL);
				}
			}
		}

	if (mnode->GetChildCount() == 0)
		{
		delete mnode;
		}
}

/******************************************************************************
 ItemIsUsed (private)

 ******************************************************************************/

JBoolean
JXToolBar::ItemIsUsed
	(
	JXTextMenu*		menu,
	const JIndex	index
	)
{
	const JSize count = itsButtons->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		JXToolBarButton* button = itsButtons->NthElement(i);
		JIndex j;
		if (button->GetMenu() == menu &&
			(button->GetMenuItemIndex(&j) && j == index))
			{
			return kJTrue;
			}
		}
	return kJFalse;
}

/******************************************************************************
 ReadPrefs

 ******************************************************************************/

void
JXToolBar::ReadPrefs
	(
	istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers > kCurrentPrefsVersion)
		{
		return;
		}

	itsLoadedPrefs = kJTrue;
	itsButtons->DeleteAll();
	itsGroupStarts->RemoveAll();

	JBoolean show;
	input >> show;
	ShowToolBar(show);
	JBoolean useSmall;
	input >> useSmall;
	UseSmallButtons(useSmall);
	if (vers == 1)
		{
		JBoolean textOnly;
		input >> textOnly;
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
		JBoolean group;
		input >> group;
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
	ostream& output
	)
	const
{
	output << kCurrentPrefsVersion;
	output << ' ' << itsIsShowingButtons;
	output << ' ' << JI2B(itsCurrentButtonHeight == kSmallButtonHeight);
	output << ' ' << (JIndex) itsButtonType;

	const JSize count = itsButtons->GetElementCount();
	output << ' ' << count;

	for (JIndex i=1; i<=count; i++)
		{
		output << ' ' << (itsButtons->NthElement(i))->GetMenuItemID();
		output << ' ' << itsGroupStarts->GetElement(i);
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
	JSize count = itsMenuBar->GetMenuCount();
	for (JSize i = 1; i <= count; i++)
		{
		JXMenu* menu		= itsMenuBar->GetMenu(i);
		JXTextMenu* tmenu	= dynamic_cast(JXTextMenu*, menu);
		assert(tmenu != NULL);
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
	JSize count = menu->GetItemCount();
	for (JSize i = 1; i <= count; i++)
		{
		const JXMenu* sub;
		if (menu->GetSubmenu(i, &sub))
			{
			const JXTextMenu* temp	= dynamic_cast(const JXTextMenu*, sub);
			JXTextMenu* tsub = const_cast<JXTextMenu*>(temp);
			FindItemAndAdd(tsub, id);
			}
		else
			{
			const JString* testID;
			if (menu->GetItemID(i, &testID))
				{
				if (*testID == id)
					{
					AppendButton(menu, i);
					return;
					}
				}
			}
		}
}

/******************************************************************************
 IsEmpty (public)

 ******************************************************************************/

JBoolean
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
		JSize count = groups.GetElementCount();
		for (JSize i = 1; i <= count; i++)
			{
			JIndexRange range = groups.GetElement(i);
			PlaceGroup(range);
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
			JXToolBarButton* button = itsButtons->NthElement(i);
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
	JXToolBarButton* button = itsButtons->NthElement(index);
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
		JXToolBarButton* button = itsButtons->NthElement(index);
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
	const JBoolean useSmall
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

JBoolean
JXToolBar::IsUsingSmallButtons()
	const
{
	return JI2B(itsCurrentButtonHeight == kSmallButtonHeight);
}

/******************************************************************************
 ShowToolBar (public)

 ******************************************************************************/

void
JXToolBar::ShowToolBar
	(
	const JBoolean show
	)
{
	if (show && !itsIsShowingButtons)
		{
		itsToolBarSet->Show();
		itsIsShowingButtons = kJTrue;
		AdjustWindowMinSize();

		itsToolBarEnclosure->Place(0,itsToolBarSet->GetBoundsHeight());
		itsToolBarEnclosure->SetSize(GetBoundsWidth(), GetBoundsHeight() - itsToolBarSet->GetBoundsHeight());

		if (!(GetWindow())->IsIconified())
			{
			(JXGetApplication())->InstallIdleTask(itsTimerTask);
			}
		}
	else if (!show && itsIsShowingButtons)
		{
		itsToolBarSet->Hide();
		itsIsShowingButtons = kJFalse;
		AdjustWindowMinSize();

		itsToolBarEnclosure->Place(0,0);
		itsToolBarEnclosure->SetSize(GetBoundsWidth(), GetBoundsHeight());

		(JXGetApplication())->RemoveIdleTask(itsTimerTask);
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
	JSize count = itsMenus->GetElementCount();
	JIndex i;
	for (i=1; i<=count; i++)
		{
		JXMenu* menu = itsMenus->NthElement(i);
		menu->PrepareToOpenMenu();
		}

	JBoolean needsAdjustment = kJFalse;
	count = itsButtons->GetElementCount();
	for (i=count; i>=1; i--)
		{
		JXToolBarButton* button = itsButtons->NthElement(i);
		JIndex itemIndex;
		if (button->GetMenuItemIndex(&itemIndex) &&
			button->GetMenu()->itsBaseItemData->IsEnabled(itemIndex))
			{
			button->Activate();
			}
		else
			{
			button->Deactivate();
			}

		JBoolean invalid;
		if (button->NeedsGeometryAdjustment(&invalid))
			{
			needsAdjustment = kJTrue;
			}

		if (invalid)
			{
			itsButtons->DeleteElement(i);
			count--;
			needsAdjustment = kJTrue;
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
	JCoordinate minHeight = itsWindowMinHeight;
	if (itsIsShowingButtons)
		{
		minHeight += itsToolBarSet->GetFrameHeight();
		}
	GetWindow()->SetMinSize(itsWindowMinWidth, minHeight);
}

#define JTemplateType JXToolBarButton
#include <JPtrArray.tmpls>
#undef JTemplateType
