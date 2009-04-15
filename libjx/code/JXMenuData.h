/******************************************************************************
 JXMenuData.h

	Interface for the JXMenuData class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXMenuData
#define _H_JXMenuData

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JContainer.h>
#include <JXMenu.h>		// need definition of JXMenu::UpdateAction
#include <JArray.h>

class JString;

class JXMenuData : public JContainer
{
	friend class JXMenu;

public:

	JXMenuData();

	virtual ~JXMenuData();

	JBoolean	HasCheckboxes() const;
	JBoolean	HasSubmenus() const;

	JBoolean	IsEnabled(const JIndex index) const;
	JBoolean	IsCheckbox(const JIndex index, JBoolean* isRadio,
						   JBoolean* isChecked) const;
	JBoolean	IsChecked(const JIndex index) const;

	JBoolean	HasSubmenu(const JIndex index) const;
	JBoolean	GetSubmenu(const JIndex index, JXMenu** menu) const;

	JBoolean	ShortcutToIndex(const JCharacter c, JIndex* index) const;

protected:

	void			InsertItem(const JIndex index,
							   const JBoolean isCheckbox = kJFalse,
							   const JBoolean isRadio = kJFalse,
							   const JCharacter* shortcuts = NULL,
							   const JCharacter* id = NULL);
	void			PrependItem(const JBoolean isCheckbox = kJFalse,
								const JBoolean isRadio = kJFalse,
								const JCharacter* shortcuts = NULL,
								const JCharacter* id = NULL);
	void			AppendItem(const JBoolean isCheckbox = kJFalse,
							   const JBoolean isRadio = kJFalse,
							   const JCharacter* shortcuts = NULL,
							   const JCharacter* id = NULL);
	virtual void	DeleteItem(const JIndex index);		// must call inherited
	virtual void	DeleteAll();						// must call inherited

	JBoolean		GetItemShortcuts(const JIndex index, const JString** shortcuts) const;
	void			SetItemShortcuts(const JIndex index, const JCharacter* shortcuts);
	virtual void	ItemShortcutsChanged(const JIndex index, const JString* shortcuts);

	JBoolean	GetItemID(const JIndex index, const JString** id) const;
	void		SetItemID(const JIndex index, const JCharacter* id);

	void	EnableItem(const JIndex index);
	void	EnableAll();
	void	DisableItem(const JIndex index);
	void	DisableAll();
	void	SetItemEnable(const JIndex index, const JBoolean enabled);

	// must call inherited

	virtual void	PrepareToOpenMenu(const JXMenu::UpdateAction updateAction);

private:

	struct BaseItemData
	{
		JBoolean	enabled;
		JString*	shortcuts;		// can be NULL
		JString*	id;				// can be NULL

		JXMenu*		submenu;		// this overrides isCheckbox; we own it; can be NULL

		JBoolean	isCheckbox;
		JBoolean	isRadio;		// implies isCheckbox
		JBoolean	isChecked;		// only used when menu is visible

		BaseItemData()
			:
			enabled( kJTrue ),
			shortcuts( NULL ),
			id( NULL ),
			submenu( NULL ),
			isCheckbox( kJFalse ),
			isRadio( kJFalse ),
			isChecked( kJFalse )
		{ };
	};

private:

	JArray<BaseItemData>*	itsBaseItemData;

private:

	void	CheckItem(const JIndex index);

	void		AttachSubmenu(const JIndex index, JXMenu* submenu);
	JBoolean	RemoveSubmenu(const JIndex index, JXMenu** theMenu);
	void		RemoveSubmenu(JXMenu* theMenu);
	void		DeleteSubmenu(const JIndex index);
	JBoolean	FindSubmenu(JXMenu* theMenu, JIndex* index) const;

	void	CleanOutBaseItem(BaseItemData* itemData);

	// not allowed

	JXMenuData(const JXMenuData& source);
	const JXMenuData& operator=(const JXMenuData& source);
};


/******************************************************************************
 New item (protected)

 ******************************************************************************/

inline void
JXMenuData::PrependItem
	(
	const JBoolean		isCheckbox,
	const JBoolean		isRadio,
	const JCharacter*	shortcuts,
	const JCharacter*	id
	)
{
	InsertItem(1, isCheckbox, isRadio, shortcuts, id);
}

inline void
JXMenuData::AppendItem
	(
	const JBoolean		isCheckbox,
	const JBoolean		isRadio,
	const JCharacter*	shortcuts,
	const JCharacter*	id
	)
{
	InsertItem(GetElementCount()+1, isCheckbox, isRadio, shortcuts, id);
}

/******************************************************************************
 IsEnabled

 ******************************************************************************/

inline JBoolean
JXMenuData::IsEnabled
	(
	const JIndex index
	)
	const
{
	const BaseItemData itemData = itsBaseItemData->GetElement(index);
	return itemData.enabled;
}

/******************************************************************************
 EnableItem (protected)

 ******************************************************************************/

inline void
JXMenuData::EnableItem
	(
	const JIndex index
	)
{
	SetItemEnable(index, kJTrue);
}

/******************************************************************************
 DisableItem (protected)

 ******************************************************************************/

inline void
JXMenuData::DisableItem
	(
	const JIndex index
	)
{
	SetItemEnable(index, kJFalse);
}

/******************************************************************************
 IsCheckbox

 ******************************************************************************/

inline JBoolean
JXMenuData::IsCheckbox
	(
	const JIndex	index,
	JBoolean*		isRadio,
	JBoolean*		isChecked
	)
	const
{
	const BaseItemData itemData = itsBaseItemData->GetElement(index);
	*isRadio   = itemData.isRadio;
	*isChecked = itemData.isChecked;
	return itemData.isCheckbox;
}

/******************************************************************************
 IsChecked

 ******************************************************************************/

inline JBoolean
JXMenuData::IsChecked
	(
	const JIndex index
	)
	const
{
	const BaseItemData itemData = itsBaseItemData->GetElement(index);
	return itemData.isChecked;
}

/******************************************************************************
 HasSubmenu

 ******************************************************************************/

inline JBoolean
JXMenuData::HasSubmenu
	(
	const JIndex index
	)
	const
{
	const BaseItemData itemData = itsBaseItemData->GetElement(index);
	return JConvertToBoolean( itemData.submenu != NULL );
}

/******************************************************************************
 GetSubmenu

 ******************************************************************************/

inline JBoolean
JXMenuData::GetSubmenu
	(
	const JIndex	index,
	JXMenu**		menu
	)
	const
{
	const BaseItemData itemData = itsBaseItemData->GetElement(index);
	*menu = itemData.submenu;
	return JConvertToBoolean( itemData.submenu != NULL );
}

/******************************************************************************
 GetItemShortcuts (protected)

 ******************************************************************************/

inline JBoolean
JXMenuData::GetItemShortcuts
	(
	const JIndex	index,
	const JString**	shortcuts
	)
	const
{
	*shortcuts = (itsBaseItemData->GetElement(index)).shortcuts;
	return JI2B(*shortcuts != NULL);
}

/******************************************************************************
 GetItemID (protected)

 ******************************************************************************/

inline JBoolean
JXMenuData::GetItemID
	(
	const JIndex	index,
	const JString**	id
	)
	const
{
	*id = (itsBaseItemData->GetElement(index)).id;
	return JI2B(*id != NULL);
}

#endif
