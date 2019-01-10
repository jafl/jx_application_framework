/******************************************************************************
 JXMenuData.cpp

	Stores the item information that all menus need.

	Shortcuts can only be characters, because nothing else can be
	easily indicated. (and because it would be overkill)

	BASE CLASS = JContainer

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXMenuData.h"
#include <algorithm>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXMenuData::JXMenuData()
	:
	JContainer()
{
	itsBaseItemData = jnew JArray<BaseItemData>;
	assert( itsBaseItemData != nullptr );

	InstallCollection(itsBaseItemData);
}

/******************************************************************************
 Destructor

	DeleteAll() must be called in the derived class dtor.

 ******************************************************************************/

JXMenuData::~JXMenuData()
{
	jdelete itsBaseItemData;
}

/******************************************************************************
 InsertItem (protected)

 ******************************************************************************/

void
JXMenuData::InsertItem
	(
	const JIndex			index,
	const JXMenu::ItemType	type,
	const JString&			shortcuts,
	const JString&			id
	)
{
	BaseItemData itemData;
	itemData.type = type;

	if (!shortcuts.IsEmpty())
		{
		itemData.shortcuts = jnew JString(shortcuts);
		assert( itemData.shortcuts != nullptr );
		}

	if (!id.IsEmpty())
		{
		itemData.id = jnew JString(id);
		assert( itemData.id != nullptr );
		}

	itsBaseItemData->InsertElementAtIndex(index, itemData);
}

/******************************************************************************
 DeleteItem (virtual protected)

 ******************************************************************************/

void
JXMenuData::DeleteItem
	(
	const JIndex index
	)
{
	BaseItemData itemData = itsBaseItemData->GetElement(index);
	CleanOutBaseItem(&itemData);
	itsBaseItemData->RemoveElement(index);
}

/******************************************************************************
 DeleteAll (virtual protected)

 ******************************************************************************/

void
JXMenuData::DeleteAll()
{
	for (BaseItemData itemData : *itsBaseItemData)
		{
		CleanOutBaseItem(&itemData);
		}
	itsBaseItemData->RemoveAll();
}

/******************************************************************************
 CleanOutBaseItem (private)

 ******************************************************************************/

void
JXMenuData::CleanOutBaseItem
	(
	BaseItemData* itemData
	)
{
	jdelete (itemData->shortcuts);
	itemData->shortcuts = nullptr;

	jdelete (itemData->id);
	itemData->id = nullptr;

	if (itemData->submenu != nullptr)
		{
		assert( (itemData->submenu)->itsMenuBar == nullptr );
		(itemData->submenu)->itsOwner = nullptr;
		jdelete (itemData->submenu);
		itemData->submenu = nullptr;
		}
}

/******************************************************************************
 HasCheckboxes

 ******************************************************************************/

JBoolean
JXMenuData::HasCheckboxes()
	const
{
	return JI2B(
		std::any_of(begin(*itsBaseItemData), end(*itsBaseItemData),
			[] (const BaseItemData& itemData)
				{ return (itemData.type == JXMenu::kCheckboxType ||
						  itemData.type == JXMenu::kRadioType); }));
}

/******************************************************************************
 HasSubmenus

 ******************************************************************************/

JBoolean
JXMenuData::HasSubmenus()
	const
{
	return JI2B(
		std::any_of(begin(*itsBaseItemData), end(*itsBaseItemData),
			[] (const BaseItemData& itemData)
				{ return itemData.submenu != nullptr; }));
}

/******************************************************************************
 SetItemShortcuts (protected)

 ******************************************************************************/

void
JXMenuData::SetItemShortcuts
	(
	const JIndex	index,
	const JString&	shortcuts
	)
{
	BaseItemData itemData = itsBaseItemData->GetElement(index);

	JBoolean changed = kJFalse;
	if (!shortcuts.IsEmpty())
		{
		if (itemData.shortcuts == nullptr)
			{
			itemData.shortcuts = jnew JString(shortcuts);
			assert( itemData.shortcuts != nullptr );
			itsBaseItemData->SetElement(index, itemData);
			}
		else
			{
			*(itemData.shortcuts) = shortcuts;
			}
		(itemData.shortcuts)->ToLower();
		changed = kJTrue;
		}
	else if (itemData.shortcuts != nullptr)
		{
		jdelete (itemData.shortcuts);
		itemData.shortcuts = nullptr;
		itsBaseItemData->SetElement(index, itemData);
		changed = kJTrue;
		}

	if (changed)
		{
		ItemShortcutsChanged(index, itemData.shortcuts);
		}
}

/******************************************************************************
 ItemShortcutsChanged (virtual protected)

	Derived classes can override this to update underlining, etc.

	Note that shortcuts can be nullptr.

 ******************************************************************************/

void
JXMenuData::ItemShortcutsChanged
	(
	const JIndex	index,
	const JString*	shortcuts
	)
{
}

/******************************************************************************
 ShortcutToIndex

	Returns kJTrue if the given character is a shortcut for one of our
	menu items.

 ******************************************************************************/

JBoolean
JXMenuData::ShortcutToIndex
	(
	const JUtf8Character&	c,
	JIndex*					index
	)
	const
{
	*index = 0;

	const JIndex count = GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const BaseItemData itemData = itsBaseItemData->GetElement(i);
		if (itemData.enabled && itemData.shortcuts != nullptr &&
			(itemData.shortcuts)->Contains(c))
			{
			*index = i;
			return kJTrue;
			}
		}

	return kJFalse;
}

/******************************************************************************
 SetItemID (protected)

 ******************************************************************************/

void
JXMenuData::SetItemID
	(
	const JIndex	index,
	const JString&	id
	)
{
	BaseItemData itemData = itsBaseItemData->GetElement(index);

	if (!id.IsEmpty())
		{
		if (itemData.id == nullptr)
			{
			itemData.id = jnew JString(id);
			assert( itemData.id != nullptr );
			itsBaseItemData->SetElement(index, itemData);
			}
		else
			{
			*(itemData.id) = id;
			}
		}
	else if (itemData.id != nullptr)
		{
		jdelete (itemData.id);
		itemData.id = nullptr;
		itsBaseItemData->SetElement(index, itemData);
		}
}

/******************************************************************************
 EnableAll (protected)

 ******************************************************************************/

void
JXMenuData::EnableAll()
{
	const JSize count = GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		EnableItem(i);
		}
}

/******************************************************************************
 DisableAll (protected)

 ******************************************************************************/

void
JXMenuData::DisableAll()
{
	const JSize count = GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		DisableItem(i);
		}
}

/******************************************************************************
 SetItemEnable (virtual protected)

 ******************************************************************************/

void
JXMenuData::SetItemEnable
	(
	const JIndex	index,
	const JBoolean	enabled
	)
{
	BaseItemData itemData = itsBaseItemData->GetElement(index);
	if (itemData.submenu == nullptr && itemData.enabled != enabled)
		{
		itemData.enabled = enabled;
		itsBaseItemData->SetElement(index, itemData);
		}
	else if (itemData.submenu != nullptr)
		{
		if (enabled && !(itemData.submenu)->itsShouldBeActiveFlag)
			{
			(itemData.submenu)->Activate();
			}
		else if (!enabled && (itemData.submenu)->itsShouldBeActiveFlag)
			{
			(itemData.submenu)->Deactivate();
			}
		itemData.enabled = (itemData.submenu)->IsActive();
		itsBaseItemData->SetElement(index, itemData);
		}
}

/******************************************************************************
 CheckItem (private)

 ******************************************************************************/

void
JXMenuData::CheckItem
	(
	const JIndex index
	)
{
	BaseItemData itemData = itsBaseItemData->GetElement(index);
	assert( itemData.type == JXMenu::kCheckboxType || itemData.type == JXMenu::kRadioType );
	if (!itemData.isChecked)
		{
		itemData.isChecked = kJTrue;
		itsBaseItemData->SetElement(index, itemData);
		}
}

/******************************************************************************
 AttachSubmenu (private)

	Deletes any old menu that was attached and attaches the new one.

 ******************************************************************************/

void
JXMenuData::AttachSubmenu
	(
	const JIndex	index,
	JXMenu*			submenu
	)
{
	assert( submenu != nullptr );

	BaseItemData itemData = itsBaseItemData->GetElement(index);
	if (itemData.submenu != nullptr)
		{
		assert( (itemData.submenu)->itsMenuBar == nullptr );
		(itemData.submenu)->itsOwner = nullptr;
		jdelete (itemData.submenu);
		}
	itemData.submenu   = submenu;
	itemData.enabled   = submenu->IsActive();
	itemData.type      = JXMenu::kPlainType;
	itemData.isChecked = kJFalse;
	itsBaseItemData->SetElement(index, itemData);
}

/******************************************************************************
 RemoveSubmenu (private)

	Returns kJTrue if there was a submenu at the given index.
	The caller now owns the menu and is responsible for deleting it.

 ******************************************************************************/

JBoolean
JXMenuData::RemoveSubmenu
	(
	const JIndex	index,
	JXMenu**		theMenu
	)
{
	BaseItemData itemData = itsBaseItemData->GetElement(index);
	*theMenu              = itemData.submenu;
	if (*theMenu != nullptr)
		{
		(**theMenu).itsOwner = nullptr;		// SetOwner() would cause infinite loop
		(**theMenu).AdjustAppearance();

		itemData.submenu = nullptr;
		itsBaseItemData->SetElement(index, itemData);

		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

void
JXMenuData::RemoveSubmenu
	(
	JXMenu* theMenu
	)
{
	assert( theMenu != nullptr );

	JIndex index;
	if (FindSubmenu(theMenu, &index))
		{
		JXMenu* aMenu;
		RemoveSubmenu(index, &aMenu);
		}
}

/******************************************************************************
 FindSubmenu (private)

	Returns kJTrue if the given submenu is attached to this menu.

 ******************************************************************************/

JBoolean
JXMenuData::FindSubmenu
	(
	JXMenu*	theMenu,
	JIndex*	index
	)
	const
{
	const JSize count = GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		BaseItemData itemData = itsBaseItemData->GetElement(i);
		if (itemData.submenu == theMenu)
			{
			*index = i;
			return kJTrue;
			}
		}

	*index = 0;
	return kJFalse;
}

/******************************************************************************
 DeleteSubmenu (private)

 ******************************************************************************/

void
JXMenuData::DeleteSubmenu
	(
	const JIndex index
	)
{
	BaseItemData itemData = itsBaseItemData->GetElement(index);
	if (itemData.submenu != nullptr)
		{
		assert( (itemData.submenu)->itsMenuBar == nullptr );
		(itemData.submenu)->itsOwner = nullptr;
		jdelete (itemData.submenu);

		itemData.submenu = nullptr;
		itsBaseItemData->SetElement(index, itemData);
		}
}

/******************************************************************************
 PrepareToOpenMenu (virtual protected)

 ******************************************************************************/

void
JXMenuData::PrepareToOpenMenu
	(
	const JXMenu::UpdateAction updateAction
	)
{
	const JSize itemCount = GetElementCount();
	for (JIndex i=1; i<=itemCount; i++)
		{
		BaseItemData itemData = itsBaseItemData->GetElement(i);

		JBoolean changed = kJFalse;
		if (itemData.enabled &&
			(updateAction == JXMenu::kDisableAll ||
			 (itemData.submenu == nullptr && updateAction == JXMenu::kDisableSingles)))
			{
			itemData.enabled = kJFalse;
			changed          = kJTrue;
			}
		if (itemData.isChecked)
			{
			itemData.isChecked = kJFalse;
			changed            = kJTrue;
			}

		if (changed)
			{
			itsBaseItemData->SetElement(i, itemData);
			}
		}
}
