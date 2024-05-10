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

#include "JXWDManager.h"
#include "JXWindowDirector.h"
#include "JXWDMenu.h"
#include "JXUrgentFunctionTask.h"
#include "JXDisplay.h"
#include "JXImage.h"
#include "JXColorManager.h"
#include "JXFunctionTask.h"
#include "jXGlobals.h"
#include <jx-af/jcore/jAssert.h>

const JInteger kFirstShortcut = 0;
const JInteger kLastShortcut  = 9;

static const JUtf8Byte kShortcutChar[] =
{
	'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'
};

static JSize kFirstDirectorOffset;

// JBroadcaster message types

const JUtf8Byte* JXWDManager::kWDMenuNeedsUpdate = "WDMenuNeedsUpdate::JXWDManager";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXWDManager::JXWDManager
	(
	JXDisplay*	display,
	const bool	wantShortcuts
	)
	:
	itsWantShortcutFlag(wantShortcuts),
	itsUpdateWDMenuTask(nullptr)
{
	itsPermWindowList = jnew JArray<WindowInfo>;

	itsWindowList = jnew JArray<WindowInfo>;
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

	// cannot delete itsUpdateWDMenuTask
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
		info.itemID = jnew JString(id);
	}

	// insert the new window -- can't sort until later

	windowList->AppendItem(info);

	// ensure menu not empty; update the menu shortcuts

	WDMenusNeedUpdate();
}

/******************************************************************************
 ShortcutUsed (static)

 ******************************************************************************/

bool
JXWDManager::ShortcutUsed
	(
	const JArray<WindowInfo>&	windowList,
	const JInteger				shortcutIndex
	)
{
	return std::any_of(begin(windowList), end(windowList),
			[shortcutIndex] (const WindowInfo& info)
			{ return (info.shortcutStr   == nullptr &&
						  info.shortcutIndex == shortcutIndex); });
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
	if (!CalledByDirectorDeleted(itsWindowList, dir))	// much more frequent
	{
		CalledByDirectorDeleted(itsPermWindowList, dir);
	}
}

// private -- returns true if found dir in windowList

bool
JXWDManager::CalledByDirectorDeleted
	(
	JArray<WindowInfo>*	windowList,
	JXWindowDirector*	dir
	)
{
	const JSize count = windowList->GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		WindowInfo info = windowList->GetItem(i);
		if (info.dir == dir)
		{
			jdelete info.shortcutStr;
			jdelete info.itemID;
			windowList->RemoveItem(i);

			// assign the shortcut to the first item that doesn't have a shortcut
			// (only need to check itsWindowList, since itsPermWindowList shouldn't change)

			if (info.shortcutIndex != kNoShortcutForDir)
			{
				const JSize count = itsWindowList->GetItemCount();
				for (JIndex j=1; j<=count; j++)
				{
					WindowInfo info1 = itsWindowList->GetItem(j);
					if (info1.shortcutStr   == nullptr &&
						info1.shortcutIndex == kNoShortcutForDir)
					{
						info1.shortcutIndex = info.shortcutIndex;
						itsWindowList->SetItem(j, info1);
						break;
					}
				}
			}

			// update the menu shortcuts

			if (itsWantShortcutFlag)
			{
				WDMenusNeedUpdate();
			}

			return true;
		}
	}

	return false;
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

	for (const auto& info : *windowList)
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
		itsUpdateWDMenuTask = jnew JXUrgentFunctionTask(this, [this]()
		{
			itsUpdateWDMenuTask = nullptr;
			UpdateAllWDMenus();
		},
		"JXWDManager::WDMenusNeedUpdate");
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

#include "JXWDManager-Windows.h"

void
JXWDManager::UpdateWDMenu
	(
	JXWDMenu* menu
	)
{
	if (menu->IsOpen())
	{
		auto* task = jnew JXFunctionTask(100, [this, menu]()
		{
			UpdateWDMenu(menu);
		},
		"JXWDManager::UpdateWDMenu",
		true);
		task->Start();
		return;
	}

	menu->SetMenuItems(kWindowsMenuStr);
	ConfigureWindowsMenu(menu);
	kFirstDirectorOffset = menu->GetItemCount();

	// It almost always is sorted, so we only pay O(N) instead of O(N^2).
	// (but we can't sort when document is created!)
	// (don't sort itsPermWindowList, because we assume there is a logical ordering)

	if (!itsWindowList->IsSorted())
	{
		itsWindowList->Sort();
	}

	// build menu

	CalledByUpdateWDMenu(*itsPermWindowList, menu);
	menu->ShowSeparatorAfter(menu->GetItemCount());
	CalledByUpdateWDMenu(*itsWindowList, menu);
}

/******************************************************************************
 CalledByUpdateWDMenu (private)

 ******************************************************************************/

void
JXWDManager::CalledByUpdateWDMenu
	(
	const JArray<WindowInfo>&	windowList,
	JXWDMenu*					menu
	)
{
	for (const auto& info : windowList)
	{
		const JString& name = info.dir->GetName();

		menu->AppendItem(name);
		const JIndex menuIndex = menu->GetItemCount();

		if (info.itemID != nullptr)
		{
			menu->SetItemID(menuIndex, *(info.itemID));
		}

		if (info.dir->NeedsSave())
		{
			menu->SetItemFontStyle(menuIndex, JColorManager::GetDarkRedColor());
		}

		const JXImage* icon;
		if (info.dir->GetMenuIcon(&icon) &&
			icon->GetDisplay() == menu->GetDisplay())
		{
			menu->SetItemImage(menuIndex, const_cast<JXImage*>(icon), false);
		}

		if (info.shortcutStr != nullptr)
		{
			menu->SetItemNMShortcut(menuIndex, *(info.shortcutStr));
		}
		else if (itsWantShortcutFlag &&
				 kFirstShortcut <= info.shortcutIndex && info.shortcutIndex <= kLastShortcut)
		{
			JString nmShortcut("Ctrl-");
			nmShortcut.Append(JUtf8Character(kShortcutChar[ info.shortcutIndex ]));
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
		menu->GetDisplay()->RaiseAllWindows();
	}
	else if (index == kCloseAllCmd)
	{
		menu->GetDisplay()->CloseAllOtherWindows(menu->GetWindow());
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
	const JSize permCount = itsPermWindowList->GetItemCount();
	if (itsPermWindowList->IndexValid(index))
	{
		const WindowInfo info = itsPermWindowList->GetItem(index);
		info.dir->Activate();
	}
	else if (itsWindowList->IndexValid(index - permCount))
	{
		const WindowInfo info = itsWindowList->GetItem(index - permCount);
		info.dir->Activate();
	}
}

/******************************************************************************
 CompareWindowNames (static private)

	Process the given message from the given sender.  This function is not
	pure virtual because not all classes will want to implement it.

 ******************************************************************************/

std::weak_ordering
JXWDManager::CompareWindowNames
	(
	const WindowInfo& w1,
	const WindowInfo& w2
	)
{
	return JCompareStringsCaseInsensitive(const_cast<JString*>(&w1.dir->GetName()),
										  const_cast<JString*>(&w2.dir->GetName()));
}
