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

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXWDManager.h>
#include <JXWindowDirector.h>
#include <JXWDMenu.h>
#include <JXUpdateWDMenuTask.h>
#include <JXDisplay.h>
#include <JXImage.h>
#include <JXColormap.h>
#include <jXGlobals.h>
#include <jXActionDefs.h>
#include <jAssert.h>

const JInteger kFirstShortcut = 0;
const JInteger kLastShortcut  = 9;

static const JCharacter kShortcutChar[] =
{
	'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'
};

// Windows menu

static const JCharacter* kMacWDMenuStr =
	"    Bring all windows to front                 %i" kJXRaiseAllWindowsAction
	"  | Close all other windows    %k Meta-Shift-W %i" kJXCloseAllOtherWindowsAction
	"%l";

static const JCharacter* kWinWDMenuStr =
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

const JCharacter* JXWDManager::kWDMenuNeedsUpdate =
	"WDMenuNeedsUpdate::JXWDManager";

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
	itsUpdateWDMenuTask(NULL)
{
	itsPermWindowList = new JArray<WindowInfo>;
	assert( itsPermWindowList != NULL );

	itsWindowList = new JArray<WindowInfo>;
	assert( itsWindowList != NULL );
	itsWindowList->SetCompareFunction(CompareWindowNames);
	itsWindowList->SetSortOrder(JOrderedSetT::kSortAscending);

	display->SetWDManager(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXWDManager::~JXWDManager()
{
	assert( itsPermWindowList->IsEmpty() );
	delete itsPermWindowList;

	assert( itsWindowList->IsEmpty() );
	delete itsWindowList;

	delete itsUpdateWDMenuTask;
}

/******************************************************************************
 PermanentDirectorCreated

 ******************************************************************************/

void
JXWDManager::PermanentDirectorCreated
	(
	JXWindowDirector*	dir,
	const JCharacter*	shortcut,
	const JCharacter*	id
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
	DirectorCreated(itsWindowList, dir, NULL, NULL);
}

/******************************************************************************
 DirectorCreated (private)

	If shortcut is NULL, it gets a numeric shortcut.
	If shortcut is "", then it doesn't have a shortcut.

 ******************************************************************************/

void
JXWDManager::DirectorCreated
	(
	JArray<WindowInfo>*	windowList,
	JXWindowDirector*	dir,
	const JCharacter*	shortcut,
	const JCharacter*	id
	)
{
	WindowInfo info(dir);

	JIndex insertionIndex = 0;
	if (shortcut != NULL)
		{
		info.shortcutStr = new JString(shortcut);
		assert( info.shortcutStr != NULL );
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

	if (!JStringEmpty(id))
		{
		info.itemID = new JString(id);
		assert( info.itemID != NULL );
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
	const JSize count = windowList.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const WindowInfo info = windowList.GetElement(i);
		if (info.shortcutStr   == NULL &&
			info.shortcutIndex == shortcutIndex)
			{
			return kJTrue;
			}
		}

	return kJFalse;
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
			delete info.shortcutStr;
			delete info.itemID;
			windowList->RemoveElement(i);

			// assign the shortcut to the first item that doesn't have a shortcut
			// (only need to check itsWindowList, since itsPermWindowList shouldn't change)

			if (info.shortcutIndex != kNoShortcutForDir)
				{
				const JSize count = itsWindowList->GetElementCount();
				for (JIndex j=1; j<=count; j++)
					{
					WindowInfo info1 = itsWindowList->GetElement(j);
					if (info1.shortcutStr   == NULL &&
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

	const JSize count = windowList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		directorList->Append(windowList->GetElement(i).dir);
		}
}

/******************************************************************************
 WDMenusNeedUpdate (private)

 ******************************************************************************/

void
JXWDManager::WDMenusNeedUpdate()
{
	if (itsUpdateWDMenuTask == NULL)
		{
		itsUpdateWDMenuTask = new JXUpdateWDMenuTask(this);
		assert( itsUpdateWDMenuTask != NULL );
		(JXGetApplication())->InstallUrgentTask(itsUpdateWDMenuTask);
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

	const JSize count = windowList.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		WindowInfo info     = windowList.GetElement(i);
		const JString& name = (info.dir)->GetName();

		menu->AppendItem(name);
		const JIndex menuIndex = menu->GetItemCount();

		if (info.itemID != NULL)
			{
			menu->SetItemID(menuIndex, *(info.itemID));
			}

		if ((info.dir)->NeedsSave())
			{
			menu->SetItemFontStyle(menuIndex, (menu->GetColormap())->GetDarkRedColor());
			}

		const JXImage* icon;
		if ((info.dir)->GetMenuIcon(&icon) &&
			icon->GetDisplay()  == menu->GetDisplay() &&
			icon->GetColormap() == menu->GetColormap())
			{
			menu->SetItemImage(menuIndex, const_cast<JXImage*>(icon), kJFalse);
			}

		if (info.shortcutStr != NULL)
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
			nmShortcut.SetCharacter(nmShortcut.GetLength(),
									kShortcutChar [ info.shortcutIndex ]);
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

JOrderedSetT::CompareResult
JXWDManager::CompareWindowNames
	(
	const WindowInfo& w1,
	const WindowInfo& w2
	)
{
	return JCompareStringsCaseInsensitive(const_cast<JString*>(&((w1.dir)->GetName())),
										  const_cast<JString*>(&((w2.dir)->GetName())));
}

#define JTemplateType JXWDManager::WindowInfo
#include <JArray.tmpls>
#undef JTemplateType

#define JTemplateType JXWindowDirector
#include <JPtrArray.tmpls>
#undef JTemplateType
