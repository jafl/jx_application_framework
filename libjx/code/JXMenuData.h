/******************************************************************************
 JXMenuData.h

	Interface for the JXMenuData class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXMenuData
#define _H_JXMenuData

#include <jx-af/jcore/JContainer.h>
#include "JXMenu.h"		// for ItemType, UpdateAction
#include <jx-af/jcore/JArray.h>

class JString;

class JXMenuData : public JContainer
{
	friend class JXMenu;

public:

	JXMenuData();

	~JXMenuData() override;

	bool	HasCheckboxes() const;
	bool	HasSubmenus() const;

	bool				IsEnabled(const JIndex index) const;
	JXMenu::ItemType	GetType(const JIndex index) const;
	bool				IsChecked(const JIndex index) const;

	bool	HasSubmenu(const JIndex index) const;
	bool	GetSubmenu(const JIndex index, JXMenu** menu) const;

	bool	ShortcutToIndex(const JUtf8Character& c, JIndex* index) const;

protected:

	void			InsertItem(const JIndex index,
							   const JXMenu::ItemType type = JXMenu::kPlainType,
							   const JString& shortcuts = JString::empty,
							   const JString& id = JString::empty);
	void			PrependItem(const JXMenu::ItemType type = JXMenu::kPlainType,
								const JString& shortcuts = JString::empty,
								const JString& id = JString::empty);
	void			AppendItem(const JXMenu::ItemType type = JXMenu::kPlainType,
							   const JString& shortcuts = JString::empty,
							   const JString& id = JString::empty);
	virtual void	DeleteItem(const JIndex index);		// must call inherited
	virtual void	DeleteAll();						// must call inherited

	bool			GetItemShortcuts(const JIndex index, const JString** shortcuts) const;
	void			SetItemShortcuts(const JIndex index, const JString& shortcuts);
	virtual void	ItemShortcutsChanged(const JIndex index, const JString* shortcuts);

	bool	GetItemID(const JIndex index, const JString** id) const;
	void	SetItemID(const JIndex index, const JString& id);

	void	EnableItem(const JIndex index);
	void	EnableAll();
	void	DisableItem(const JIndex index);
	void	DisableAll();
	void	SetItemEnabled(const JIndex index, const bool enabled);

	// must call inherited

	virtual void	PrepareToOpenMenu(const JXMenu::UpdateAction updateAction);

private:

	struct BaseItemData
	{
		bool				enabled;
		JString*			shortcuts;	// can be nullptr
		JString*			id;			// can be nullptr

		JXMenu*				submenu;	// this overrides isCheckbox; we own it; can be nullptr

		JXMenu::ItemType	type;
		bool				isChecked;	// only used when menu is visible

		BaseItemData()
			:
			enabled( true ),
			shortcuts( nullptr ),
			id( nullptr ),
			submenu( nullptr ),
			type( JXMenu::kPlainType ),
			isChecked( false )
		{ };
	};

private:

	JArray<BaseItemData>*	itsBaseItemData;

private:

	void	CheckItem(const JIndex index);

	void	AttachSubmenu(const JIndex index, JXMenu* submenu);
	bool	RemoveSubmenu(const JIndex index, JXMenu** theMenu);
	void	RemoveSubmenu(JXMenu* theMenu);
	void	DeleteSubmenu(const JIndex index);
	bool	FindSubmenu(JXMenu* theMenu, JIndex* index) const;

	void	CleanOutBaseItem(BaseItemData* itemData);

	// not allowed

	JXMenuData(const JXMenuData&) = delete;
	JXMenuData& operator=(const JXMenuData&) = delete;
};


/******************************************************************************
 New item (protected)

 ******************************************************************************/

inline void
JXMenuData::PrependItem
	(
	const JXMenu::ItemType	type,
	const JString&			shortcuts,
	const JString&			id
	)
{
	InsertItem(1, type, shortcuts, id);
}

inline void
JXMenuData::AppendItem
	(
	const JXMenu::ItemType	type,
	const JString&			shortcuts,
	const JString&			id
	)
{
	InsertItem(GetElementCount()+1, type, shortcuts, id);
}

/******************************************************************************
 IsEnabled

 ******************************************************************************/

inline bool
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
	SetItemEnabled(index, true);
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
	SetItemEnabled(index, false);
}

/******************************************************************************
 GetType

 ******************************************************************************/

inline JXMenu::ItemType
JXMenuData::GetType
	(
	const JIndex index
	)
	const
{
	const BaseItemData itemData = itsBaseItemData->GetElement(index);
	return itemData.type;
}

/******************************************************************************
 IsChecked

 ******************************************************************************/

inline bool
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

inline bool
JXMenuData::HasSubmenu
	(
	const JIndex index
	)
	const
{
	const BaseItemData itemData = itsBaseItemData->GetElement(index);
	return itemData.submenu != nullptr;
}

/******************************************************************************
 GetSubmenu

 ******************************************************************************/

inline bool
JXMenuData::GetSubmenu
	(
	const JIndex	index,
	JXMenu**		menu
	)
	const
{
	const BaseItemData itemData = itsBaseItemData->GetElement(index);
	*menu = itemData.submenu;
	return itemData.submenu != nullptr;
}

/******************************************************************************
 GetItemShortcuts (protected)

 ******************************************************************************/

inline bool
JXMenuData::GetItemShortcuts
	(
	const JIndex	index,
	const JString**	shortcuts
	)
	const
{
	*shortcuts = (itsBaseItemData->GetElement(index)).shortcuts;
	return *shortcuts != nullptr;
}

/******************************************************************************
 GetItemID (protected)

 ******************************************************************************/

inline bool
JXMenuData::GetItemID
	(
	const JIndex	index,
	const JString**	id
	)
	const
{
	*id = (itsBaseItemData->GetElement(index)).id;
	return *id != nullptr;
}

#endif
