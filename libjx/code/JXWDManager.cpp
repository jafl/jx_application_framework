/******************************************************************************
 JXWDManager.cpp

	Singleton class that provides the services for JXWDMenu, a menu of all
	registered windows.  JXWindowDirectors must register themselves because
	not all windows belong on the menu.  JXDocuments automatically register
	themselves.

	JXWindowDirectors must override GetMenuIcon() in order to display an
	icon on JXWDMenu.  They can also override GetName() if they want to
	display a name other than their window title.  JXDocuments can override
	GetMenuIcon() if they want something other than the default plain file
	icon.

	BASE CLASS = virtual JBroadcaster

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include <JXWDManager.h>
#include <JXWindowDirector.h>
#include <JXWDMenu.h>
#include <JXUpdateWDMenuTask.h>
#include <JXDisplay.h>
#include <JXImage.h>
#include <JXColorManager.h>
#include <jXGlobals.h>
#include <jXActionDefs.h>
#include <JStringIterator.h>
#include <jAssert.h>

const JInteger kFirstShortcut = 0;
const JInteger kLastShortcut  = 9;

static const JUtf8Byte kShortcutChar[] =
{
	'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'
};

// Windows menu

static const JUtf8Byte* kMacWDMenuStr =
	"    Bring all windows to front                 %i" kJXRaiseAllWindowsAction
	"  | Close all other windows    %k Meta-Shift-W %i" kJXCloseAllOtherWindowsAction
	"%l";

static const JUtf8Byte* kWinWDMenuStr =
	"    Bring all windows to front                 %i" kJXRaiseAllWindowsAction
	"  | Close all other windows    %k Ctrl-Shift-W %i" kJXCloseAllOtherWindowsAction
	"%l";

enum
{
	kRaiseAllCmd = 1,
	kCloseAllCmd		// = kFirstDirectorOffset
};

const JSize kFirstDirectorOffset = kCloseAllCmd;

// JBroadcaster message types

const JUtf8Byte* JXWDManager::kWDMenuNeedsUpdate = "WDMenuNeedsUpdate::JXWDManager";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXWDManager::JXWDManager
	(
	JXDisplay*		display,
	const JBoolean	wantShortcuts
	)
	:
	itsWantShortcutFlag(wantShortcuts),
	itsUpdateWDMenuTask(nullptr)
{
	itsPermWindowList = jnew JArray<WindowInfo>;
	assert( itsPermWindowList != nullptr );

	itsWindowList = jnew JArray<WindowInfo>;
	assert( itsWindowList != nullptr );
	itsWindowList->SetCompareFunction(CompareWindowNames);
	itsWindowList->SetSortOrder(JListT::kSortAscending);

	display->SetWDManager(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXWDManager::~JXWDManager()
{
	assert( itsPermWindowList->IsEmpty() );
	jdelete itsPermWindowList;

	assert( itsWindowList->IsEmpty() );
	jdelete itsWindowList;

	jdelete itsUpdateWDMenuTask;
}

/******************************************************************************
 PermanentDirectorCreated

 ******************************************************************************/

void
JXWDManager::PermanentDirectorCreated
	(
	JXWindowDirector*	dir,
	const JString&		shortcut,
	const JUtf8Byte*	id
	)
{
	DirectorCreated(itsPermWindowList, dir, shortcut, id);
}

/******************************************************************************
 DirectorCreated

 ******************************************************************************/

void
JXWDManager::DirectorCreated
	(
	JXWindowDirector* dir
	)
{
	DirectorCreated(itsWindowList, dir, JString::empty, nullptr);
}

/******************************************************************************
 DirectorCreated (private)

	If shortcut is nullptr, it gets a numeric shortcut.
	If shortcut is JString::empty, then it doesn't have a shortcut.

 ******************************************************************************/

void
JXWDManager::DirectorCreated
	(
	JArray<WindowInfo>*	windowList,
	JXWindowDirector*	dir,
	const JString&		shortcut,
	const JUtf8Byte*	id
	)
{
	WindowInfo info(dir);

	if (!shortcut.IsEmpty())
		{
		info.shortcutStr = jnew JString(shortcut);
		assert( info.shortcutStr != nullptr );
		}
	else
		{
		// find the first unused shortcut, if any

		for (JInteger i=kFirstShortcut; i<=kLastShortcut; i++)
			{
			if (!ShortcutUsed(*itsPermWindowList, i) &&
				!ShortcutUsed(*itsWindowList, i))
				{
				info.shortcutIndex = i;
				break;
				}
			}
		}

	if (!JString::IsEmpty(id))
		{
		info.itemID = jnew JString(id, 0);
		assert( info.itemID != nullptr );
		}

	// insert the new window -- can't sort until later

	windowList->AppendElement(info);

	// ensure menu not empty; update the menu shortcuts

	WDMenusNeedUpdate();
}

/******************************************************************************
 ShortcutUsed (static)

 ******************************************************************************/

JBoolean
JXWDManager::ShortcutUsed
	(
	const JArray<WindowInfo>&	windowList,
	const JInteger				shortcutIndex
	)
{
	return JI2B(
		std::any_of(begin(windowList), end(windowList),
			[shortcutIndex] (const WindowInfo& info)
				{ return (info.shortcutStr   == nullptr &&
						  info.shortcutIndex == shortcutIndex); }));
}

/******************************************************************************
 DirectorDeleted

	Automatically called by ~JXWindowDirector.

 ******************************************************************************/

void
JXWDManager::DirectorDeleted
	(
	JXWindowDirector* dir
	)
{
	if (!DirectorDeleted1(itsWindowList, dir))	// much more frequent
		{
		DirectorDeleted1(itsPermWindowList, dir);
		}
}

// private -- returns kJTrue if found dir in windowList

JBoolean
JXWDManager::DirectorDeleted1
	(
	JArray<WindowInfo>*	windowList,
	JXWindowDirector*	dir
	)
{
	const JSize count = windowList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		WindowInfo info = windowList->GetElement(i);
		if (info.dir == dir)
			{
			jdelete info.shortcutStr;
			jdelete info.itemID;
			windowList->RemoveElement(i);

			// assign the shortcut to the first item that doesn't have a shortcut
			// (only need to check itsWindowList, since itsPermWindowList shouldn't change)

			if (info.shortcutIndex != kNoShortcutForDir)
				{
				const JSize count = itsWindowList->GetElementCount();
				for (JIndex j=1; j<=count; j++)
					{
					WindowInfo info1 = itsWindowList->GetElement(j);
					if (info1.shortcutStr   == nullptr &&
						info1.shortcutIndex == kNoShortcutForDir)
						{
						info1.shortcutIndex = info.shortcutIndex;
						itsWindowList->SetElement(j, info1);
						break;
						}
					}
				}

			// update the menu shortcuts

			if (itsWantShortcutFlag)
				{
				WDMenusNeedUpdate();
				}

			return kJTrue;
			}
		}

	return kJFalse;
}

/******************************************************************************
 GetDirectors (private)

 ******************************************************************************/

void
JXWDManager::GetDirectors
	(
	JArray<WindowInfo>*				windowList,
	JPtrArray<JXWindowDirector>*	directorList
	)
	const
{
	directorList->CleanOut();
	directorList->SetCleanUpAction(JPtrArrayT::kForgetAll);

	for (const WindowInfo& info : *windowList)
		{
		directorList->Append(info.dir);
		}
}

/******************************************************************************
 WDMenusNeedUpdate (private)

 ******************************************************************************/

void
JXWDManager::WDMenusNeedUpdate()
{
	if (itsUpdateWDMenuTask == nullptr)
		{
		itsUpdateWDMenuTask = jnew JXUpdateWDMenuTask(this);
		assert( itsUpdateWDMenuTask != nullptr );
		itsUpdateWDMenuTask->Go();
		}
}

/******************************************************************************
 UpdateAllWDMenus (private)

	This must be called via an UrgentTask because we can't call GetName()
	for a new JXDocument until long after DirectorCreated() is called.

 ******************************************************************************/

void
JXWDManager::UpdateAllWDMenus()
{
	Broadcast(WDMenuNeedsUpdate());
}

/******************************************************************************
 UpdateWDMenu

 ******************************************************************************/

void
JXWDManager::UpdateWDMenu
	(
	JXWDMenu* menu
	)
{
	if (menu->IsOpen())
		{
		WDMenusNeedUpdate();
		return;
		}

	const JXMenu::Style style = JXMenu::GetDefaultStyle();
	if (style == JXMenu::kWindowsStyle)
		{
		menu->SetMenuItems(kWinWDMenuStr, "JXWDManager");
		}
	else
		{
		assert( style == JXMenu::kMacintoshStyle );
		menu->SetMenuItems(kMacWDMenuStr, "JXWDManager");
		}

	// It almost always is sorted, so we only pay O(N) instead of O(N^2).
	// (but we can't sort when document is created!)
	// (don't sort itsPermWindowList, because we assume there is a logical ordering)

	if (!itsWindowList->IsSorted())
		{
		itsWindowList->Sort();
		}

	// build menu

	UpdateWDMenu1(*itsPermWindowList, menu);
	menu->ShowSeparatorAfter(menu->GetItemCount());
	UpdateWDMenu1(*itsWindowList, menu);
}

/******************************************************************************
 UpdateWDMenu1 (private)

 ******************************************************************************/

void
JXWDManager::UpdateWDMenu1
	(
	const JArray<WindowInfo>&	windowList,
	JXWDMenu*					menu
	)
{
	const JXMenu::Style style = JXMenu::GetDefaultStyle();

	for (const WindowInfo& info : windowList)
		{
		const JString& name = (info.dir)->GetName();

		menu->AppendItem(name);
		const JIndex menuIndex = menu->GetItemCount();

		if (info.itemID != nullptr)
			{
			menu->SetItemID(menuIndex, *(info.itemID));
			}

		if ((info.dir)->NeedsSave())
			{
			menu->SetItemFontStyle(menuIndex, JColorManager::GetDarkRedColor());
			}

		const JXImage* icon;
		if ((info.dir)->GetMenuIcon(&icon) &&
			icon->GetDisplay() == menu->GetDisplay())
			{
			menu->SetItemImage(menuIndex, const_cast<JXImage*>(icon), kJFalse);
			}

		if (info.shortcutStr != nullptr)
			{
			menu->SetItemNMShortcut(menuIndex, *(info.shortcutStr));
			}
		else if (itsWantShortcutFlag &&
				 kFirstShortcut <= info.shortcutIndex && info.shortcutIndex <= kLastShortcut)
			{
			JString nmShortcut;
			if (style == JXMenu::kWindowsStyle)
				{
				nmShortcut = "Ctrl-0";
				}
			else
				{
				assert( style == JXMenu::kMacintoshStyle );
				nmShortcut = "Meta-0";
				}

			JStringIterator iter(&nmShortcut, kJIteratorStartAtEnd);
			iter.SetPrev(kShortcutChar [ info.shortcutIndex ]);
			menu->SetItemNMShortcut(menuIndex, nmShortcut);
			}
		}
}

/******************************************************************************
 HandleWDMenu

 ******************************************************************************/

void
JXWDManager::HandleWDMenu
	(
	JXWDMenu*		menu,
	const JIndex	index
	)
{
	if (index == kRaiseAllCmd)
		{
		(menu->GetDisplay())->RaiseAllWindows();
		}
	else if (index == kCloseAllCmd)
		{
		(menu->GetDisplay())->CloseAllOtherWindows(menu->GetWindow());
		}
	else
		{
		ActivateDirector(index - kFirstDirectorOffset);
		}
}

/******************************************************************************
 ActivateDirector (private)

	We check IndexValid() because window might close while menu is open.

 ******************************************************************************/

void
JXWDManager::ActivateDirector
	(
	const JIndex index
	)
{
	const JSize permCount = itsPermWindowList->GetElementCount();
	if (itsPermWindowList->IndexValid(index))
		{
		const WindowInfo info = itsPermWindowList->GetElement(index);
		(info.dir)->Activate();
		}
	else if (itsWindowList->IndexValid(index - permCount))
		{
		const WindowInfo info = itsWindowList->GetElement(index - permCount);
		(info.dir)->Activate();
		}
}

/******************************************************************************
 CompareWindowNames (static private)

	Process the given message from the given sender.  This function is not
	pure virtual because not all classes will want to implement it.

 ******************************************************************************/

JListT::CompareResult
JXWDManager::CompareWindowNames
	(
	const WindowInfo& w1,
	const WindowInfo& w2
	)
{
	return JCompareStringsCaseInsensitive(const_cast<JString*>(&((w1.dir)->GetName())),
										  const_cast<JString*>(&((w2.dir)->GetName())));
}
