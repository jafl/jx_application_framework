/******************************************************************************
 JXMenuData.cpp

	Stores the item information that all menus need.

	Shortcuts can only be characters, because nothing else can be
	easily indicated. (and because it would be overkill)

	BASE CLASS = JContainer

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXMenuData.h>
#include <JString.h>
#include <ctype.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXMenuData::JXMenuData()
	:
	JContainer()
{
	itsBaseItemData = new JArray<BaseItemData>;
	assert( itsBaseItemData != NULL );

	InstallOrderedSet(itsBaseItemData);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXMenuData::~JXMenuData()
{
	DeleteAll();
	delete itsBaseItemData;
}

/******************************************************************************
 InsertItem (protected)

 ******************************************************************************/

void
JXMenuData::InsertItem
	(
	const JIndex		index,
	const JBoolean		isCheckbox,
	const JBoolean		isRadio,
	const JCharacter*	shortcuts,
	const JCharacter*	id
	)
{
	BaseItemData itemData;
	itemData.isCheckbox = JConvertToBoolean(isCheckbox || isRadio);
	itemData.isRadio    = isRadio;

	if (!JStringEmpty(shortcuts))
		{
		itemData.shortcuts = new JString(shortcuts);
		assert( itemData.shortcuts != NULL );
		}

	if (!JStringEmpty(id))
		{
		itemData.id = new JString(id);
		assert( itemData.id != NULL );
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
	const JSize itemCount = GetElementCount();
	for (JIndex i=1; i<=itemCount; i++)
		{
		BaseItemData itemData = itsBaseItemData->GetElement(i);
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
	delete (itemData->shortcuts);
	itemData->shortcuts = NULL;

	delete (itemData->id);
	itemData->id = NULL;

	if (itemData->submenu != NULL)
		{
		assert( (itemData->submenu)->itsMenuBar == NULL );
		(itemData->submenu)->itsOwner = NULL;
		delete (itemData->submenu);
		itemData->submenu = NULL;
		}
}

/******************************************************************************
 HasCheckboxes

 ******************************************************************************/

JBoolean
JXMenuData::HasCheckboxes()
	const
{
	const JIndex count = GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const BaseItemData itemData = itsBaseItemData->GetElement(i);
		if (itemData.isCheckbox)
			{
			return kJTrue;
			}
		}

	return kJFalse;
}

/******************************************************************************
 HasSubmenus

 ******************************************************************************/

JBoolean
JXMenuData::HasSubmenus()
	const
{
	const JIndex count = GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const BaseItemData itemData = itsBaseItemData->GetElement(i);
		if (itemData.submenu != NULL)
			{
			return kJTrue;
			}
		}

	return kJFalse;
}

/******************************************************************************
 SetItemShortcuts (protected)

 ******************************************************************************/

void
JXMenuData::SetItemShortcuts
	(
	const JIndex		index,
	const JCharacter*	shortcuts
	)
{
	BaseItemData itemData = itsBaseItemData->GetElement(index);

	JBoolean changed = kJFalse;
	if (!JStringEmpty(shortcuts))
		{
		if (itemData.shortcuts == NULL)
			{
			itemData.shortcuts = new JString(shortcuts);
			assert( itemData.shortcuts != NULL );
			itsBaseItemData->SetElement(index, itemData);
			}
		else
			{
			*(itemData.shortcuts) = shortcuts;
			}
		(itemData.shortcuts)->ToLower();
		changed = kJTrue;
		}
	else if (itemData.shortcuts != NULL)
		{
		delete (itemData.shortcuts);
		itemData.shortcuts = NULL;
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

	Note that shortcuts can be NULL.

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
	const JCharacter	c,
	JIndex*				index
	)
	const
{
	*index = 0;
	const JCharacter s[2] = { tolower(c), '\0' };

	const JIndex count = GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const BaseItemData itemData = itsBaseItemData->GetElement(i);
		if (itemData.enabled && itemData.shortcuts != NULL &&
			(itemData.shortcuts)->Contains(s))
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
	const JIndex		index,
	const JCharacter*	id
	)
{
	BaseItemData itemData = itsBaseItemData->GetElement(index);

	if (!JStringEmpty(id))
		{
		if (itemData.id == NULL)
			{
			itemData.id = new JString(id);
			assert( itemData.id != NULL );
			itsBaseItemData->SetElement(index, itemData);
			}
		else
			{
			*(itemData.id) = id;
			}
		}
	else if (itemData.id != NULL)
		{
		delete (itemData.id);
		itemData.id = NULL;
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
	if (itemData.submenu == NULL && itemData.enabled != enabled)
		{
		itemData.enabled = enabled;
		itsBaseItemData->SetElement(index, itemData);
		}
	else if (itemData.submenu != NULL)
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
	assert( itemData.isCheckbox );
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
	assert( submenu != NULL );

	BaseItemData itemData = itsBaseItemData->GetElement(index);
	if (itemData.submenu != NULL)
		{
		assert( (itemData.submenu)->itsMenuBar == NULL );
		(itemData.submenu)->itsOwner = NULL;
		delete (itemData.submenu);
		}
	itemData.submenu    = submenu;
	itemData.enabled    = submenu->IsActive();
	itemData.isCheckbox = kJFalse;
	itemData.isRadio    = kJFalse;
	itemData.isChecked  = kJFalse;
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
	if (*theMenu != NULL)
		{
		(**theMenu).itsOwner = NULL;		// SetOwner() would cause infinite loop
		(**theMenu).AdjustAppearance();

		itemData.submenu = NULL;
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
	assert( theMenu != NULL );

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
	if (itemData.submenu != NULL)
		{
		assert( (itemData.submenu)->itsMenuBar == NULL );
		(itemData.submenu)->itsOwner = NULL;
		delete (itemData.submenu);

		itemData.submenu = NULL;
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
			 (itemData.submenu == NULL && updateAction == JXMenu::kDisableSingles)))
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

#define JTemplateType JXMenuData::BaseItemData
#include <JArray.tmpls>
#undef JTemplateType
