/******************************************************************************
 JXMenu.h

	Interface for the JXMenu class

	Copyright © 1996-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXMenu
#define _H_JXMenu

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWidget.h>
#include <JString.h>
#include <JFontStyle.h>

class JXMenuData;
class JXMenuBar;
class JXMenuManager;
class JXWindowDirector;
class JXMenuDirector;
class JXMenuTable;
class JXImage;

class JXToolBar;
class JXTextMenu;
class JXImageMenu;

class JXMenu : public JXWidget
{
	friend class JXMenuData;
	friend class JXMenuTable;
	friend class JXMenuBar;
	friend class JXMenuManager;

	friend class JXToolBar;

public:

	enum ItemType
	{
		kPlainType,
		kCheckboxType,
		kRadioType
	};

	enum UpdateAction
	{
		kDisableNone,
		kDisableSingles,	// disable items without submenus before update message (default)
		kDisableAll			// disable everything before update message
	};

	enum Style				// do not change values once assigned; stored in files
	{
		kMacintoshStyle = 0,
		kWindowsStyle,		// = kStyleMax

		kStyleMax = kWindowsStyle
	};

	enum ArrowPosition
	{
		kArrowAtLeft,
		kArrowAtRight		// default
	};

	enum ArrowDirection
	{
		kArrowPointsUp,
		kArrowPointsDown	// default
	};

public:

	JXMenu(const JCharacter* title, JXContainer* enclosure,
		   const HSizingOption hSizing, const VSizingOption vSizing,
		   const JCoordinate x, const JCoordinate y,
		   const JCoordinate w, const JCoordinate h);

	JXMenu(JXImage* image, const JBoolean menuOwnsImage, JXContainer* enclosure,
		   const HSizingOption hSizing, const VSizingOption vSizing,
		   const JCoordinate x, const JCoordinate y,
		   const JCoordinate w, const JCoordinate h);

	JXMenu(const JCharacter* title, JXImage* image, const JBoolean menuOwnsImage,
		   JXContainer* enclosure,
		   const HSizingOption hSizing, const VSizingOption vSizing,
		   const JCoordinate x, const JCoordinate y,
		   const JCoordinate w, const JCoordinate h);

	JXMenu(JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	virtual ~JXMenu();

	JBoolean	IsOpen() const;

	const JString&	GetTitleText() const;
	JBoolean		GetTitleText(const JString** text) const;
	JBoolean		GetTitleImage(const JXImage** image) const;
	void			SetTitle(const JCharacter* title,
							 JXImage* image, const JBoolean menuOwnsImage);
	void			SetTitleText(const JCharacter* title);
	void			SetTitleImage(JXImage* image, const JBoolean menuOwnsImage);

	void	SetTitleFontName(const JCharacter* fontName);
	void	SetTitleFontSize(const JSize size);
	void	SetTitleFontStyle(const JFontStyle& style);

	void	SetShortcuts(const JCharacter* list);

	JBoolean	IsEmpty() const;
	JSize		GetItemCount() const;
	void		RemoveItem(const JIndex index);
	void		RemoveAllItems();

	JBoolean	GetItemShortcuts(const JIndex index, JString* shortcuts) const;
	void		SetItemShortcuts(const JIndex index, const JCharacter* shortcuts);

	JBoolean	GetItemID(const JIndex index, const JString** id) const;
	void		SetItemID(const JIndex index, const JCharacter* id);
	JBoolean	ItemIDToIndex(const JCharacter* targetID, JIndex* index) const;

	JBoolean	IsEnabled(const JIndex index) const;
	void		EnableItem(const JIndex index);
	void		EnableAll();
	void		DisableItem(const JIndex index);
	void		DisableAll();
	void		SetItemEnable(const JIndex index, const JBoolean enabled);

	JBoolean	IsChecked(const JIndex index) const;
	void		CheckItem(const JIndex index);

	JBoolean	GetSubmenu(const JIndex index, const JXMenu** menu) const;
	void		AttachSubmenu(const JIndex index, JXMenu* submenu);
	JBoolean	RemoveSubmenu(const JIndex index, JXMenu** theMenu);
	void		DeleteSubmenu(const JIndex index);

	UpdateAction	GetUpdateAction() const;
	void			SetUpdateAction(const UpdateAction action);

	JBoolean		IsPopupChoice() const;
	virtual void	SetToPopupChoice(const JBoolean isPopup,
									 const JIndex initialChoice);
	void			SetPopupChoice(const JIndex index);

	virtual void	Activate();
	virtual void	Deactivate();

	virtual void	Suspend();
	virtual void	Resume();

	JBoolean	IsHiddenPopupMenu() const;
	void		SetToHiddenPopupMenu(const JBoolean isHiddenPopup = kJTrue);
	JBoolean	PopUp(JXContainer* mouseOwner, const JPoint& pt,
					  const JXButtonStates&	buttonStates,
					  const JXKeyModifiers&	modifiers);

	JBoolean	GetMenuBar(JXMenuBar** menuBar) const;
	JXMenu*		GetTopLevelMenu() const;

	ArrowPosition	GetPopupArrowPosition() const;
	void			SetPopupArrowPosition(const ArrowPosition position);

	ArrowDirection	GetPopupArrowDirection() const;
	void			SetPopupArrowDirection(const ArrowDirection direction);

	virtual JBoolean	IsMenu() const;

	virtual void	HandleShortcut(const int key, const JXKeyModifiers& modifiers);

	static Style	GetDefaultStyle();
	static void		SetDefaultStyle(const Style style);

	static Style	GetDisplayStyle();
	static void		SetDisplayStyle(const Style style);

	static JXModifierKey	AdjustNMShortcutModifier(const JXModifierKey key);

	static const JCharacter*	GetDefaultFont(JSize* size);

protected:

	void		SetBaseItemData(JXMenuData* baseItemData);
	void		ClearBaseItemData();
	JBoolean	PrepareToOpenMenu(const JBoolean shortcut);

	virtual JXMenuDirector*	CreateMenuWindow(JXWindowDirector* supervisor) = 0;
	virtual void			AdjustPopupChoiceTitle(const JIndex index) = 0;

	virtual void	Draw(JXWindowPainter& p, const JRect& rect);
	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame);

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);

	virtual void	Receive(JBroadcaster* sender, const Message& message);

	// called by JXMenuTable

	void	BroadcastSelection(const JIndex itemIndex, const JBoolean fromShortcut);

private:

	JString		itsTitle;
	JXImage*	itsTitleImage;			// can be NULL
	JBoolean	itsOwnsTitleImageFlag;	// kJTrue => we delete it
	JString*	itsShortcuts;			// can be NULL
	JIndex		itsULIndex;
	JXMenuData*	itsBaseItemData;		// derived class owns this
	JXMenuBar*	itsMenuBar;				// can be NULL; if there is one, it owns us
	JXMenu*		itsOwner;				// NULL if top level menu

	JString		itsTitleFontName;
	JSize		itsTitleSize;
	JFontStyle	itsTitleStyle;
	JColorIndex	itsTrueTitleColor;		// saves title color while deactivated
	JBoolean	itsShouldBeActiveFlag;	// kJTrue  => last client call was Activate()
	JBoolean	itsUpdateSBAFlag;		// kJFalse => don't change itsShouldBeActiveFlag
	JSize		itsMinWidth;

	UpdateAction	itsUpdateAction;
	JBoolean		itsIsPopupChoiceFlag;
	JBoolean		itsIsHiddenPopupMenuFlag;

	ArrowPosition	itsArrowPosition;	// location of arrow when menu is free-standing
	ArrowDirection	itsArrowDirection;	// direction of arrow when menu is free-standing

	static Style	itsDefaultStyle;
	static Style	itsDisplayStyle;

	static JBoolean	theDefaultMenuFontInitFlag;
	static JString	theDefaultFontName;
	static JSize	theDefaultFontSize;

	// used when menu is pulled down

	JXMenuDirector*	itsMenuDirector;

private:

	void	JXMenuX(const JCharacter* title, JXImage* image,
					const JBoolean menuOwnsImage);
	void	RemoveSubmenu(JXMenu* theMenu);
	void	SetOwner(JXMenu* owner);
	void	UpdateTitleGeometry();
	void	AdjustAppearance();
	void	AdjustAppearance(const JCoordinate minWidth);

	JBoolean	Open(const JPoint& leftPtR = JPoint(),
					 const JPoint& rightPtR = JPoint());
	void		Close();

	void	PrivateActivate();
	void	PrivateDeactivate();

	// called by JXMenuBar

	void	SetMenuBar(JXMenuBar* bar);

	// not allowed

	JXMenu(const JXMenu& source);
	const JXMenu& operator=(const JXMenu& source);

public:

	// JBroadcaster messages

	static const JCharacter* kNeedsUpdate;
	static const JCharacter* kItemSelected;

	class NeedsUpdate : public JBroadcaster::Message
		{
		public:

			NeedsUpdate(const JBoolean fromShortcut)
				:
				JBroadcaster::Message(kNeedsUpdate),
				itsFromShortcutFlag(fromShortcut)
				{ };

			JBoolean
			IsFromShortcut() const
			{
				return itsFromShortcutFlag;
			};

		private:

			const JBoolean itsFromShortcutFlag;
		};

	class ItemSelected : public JBroadcaster::Message
		{
		public:

			ItemSelected(const JIndex index, const JBoolean fromShortcut)
				:
				JBroadcaster::Message(kItemSelected),
				itsIndex(index), itsFromShortcutFlag(fromShortcut)
				{ };

			JIndex
			GetIndex() const
			{
				return itsIndex;
			};

			JBoolean
			IsFromShortcut() const
			{
				return itsFromShortcutFlag;
			};

		private:

			const JIndex	itsIndex;
			const JBoolean	itsFromShortcutFlag;
		};
};


/******************************************************************************
 IsOpen

 ******************************************************************************/

inline JBoolean
JXMenu::IsOpen()
	const
{
	return JConvertToBoolean( itsMenuDirector != NULL );
}

/******************************************************************************
 GetTitleText

 ******************************************************************************/

inline const JString&
JXMenu::GetTitleText()
	const
{
	return itsTitle;
}

inline JBoolean
JXMenu::GetTitleText
	(
	const JString** text
	)
	const
{
	*text = &itsTitle;
	return !itsTitle.IsEmpty();
}

/******************************************************************************
 GetTitleImage

 ******************************************************************************/

inline JBoolean
JXMenu::GetTitleImage
	(
	const JXImage** image
	)
	const
{
	*image = itsTitleImage;
	return JI2B(itsTitleImage != NULL);
}

/******************************************************************************
 GetUpdateAction

 ******************************************************************************/

inline JXMenu::UpdateAction
JXMenu::GetUpdateAction()
	const
{
	return itsUpdateAction;
}

/******************************************************************************
 SetUpdateAction

 ******************************************************************************/

inline void
JXMenu::SetUpdateAction
	(
	const UpdateAction action
	)
{
	itsUpdateAction = action;
}

/******************************************************************************
 PopupChoice

 ******************************************************************************/

inline JBoolean
JXMenu::IsPopupChoice()
	const
{
	return itsIsPopupChoiceFlag;
}

inline void
JXMenu::SetPopupChoice
	(
	const JIndex index
	)
{
	if (itsIsPopupChoiceFlag)
		{
		AdjustPopupChoiceTitle(index);
		}
}

/******************************************************************************
 HiddenPopupMenu

	You must explicitly tell us if the menu is an invisible popup menu,
	because invisible menus are normally disabled.

 ******************************************************************************/

inline JBoolean
JXMenu::IsHiddenPopupMenu()
	const
{
	return itsIsHiddenPopupMenuFlag;
}

inline void
JXMenu::SetToHiddenPopupMenu
	(
	const JBoolean isHiddenPopup
	)
{
	itsIsHiddenPopupMenuFlag = isHiddenPopup;
	if (itsIsHiddenPopupMenuFlag)
		{
		Hide();
		}
}

/******************************************************************************
 Popup arrow

	Control the position and direction of the arrow when the menu is not in
	a menu bar.

 ******************************************************************************/

inline JXMenu::ArrowPosition
JXMenu::GetPopupArrowPosition()
	const
{
	return itsArrowPosition;
}

inline void
JXMenu::SetPopupArrowPosition
	(
	const ArrowPosition position
	)
{
	itsArrowPosition = position;
	Refresh();
}

inline JXMenu::ArrowDirection
JXMenu::GetPopupArrowDirection()
	const
{
	return itsArrowDirection;
}

inline void
JXMenu::SetPopupArrowDirection
	(
	const ArrowDirection direction
	)
{
	itsArrowDirection = direction;
	Refresh();
}

/******************************************************************************
 AdjustAppearance (private)

 ******************************************************************************/

inline void
JXMenu::AdjustAppearance()
{
	AdjustAppearance(itsMinWidth);
}

/******************************************************************************
 Default menu style (static)

	This controls the style of all menus created by JX and also indicates
	what style the programmer hard-coded into the application menus.

 ******************************************************************************/

inline JXMenu::Style
JXMenu::GetDefaultStyle()
{
	return itsDefaultStyle;
}

inline void
JXMenu::SetDefaultStyle
	(
	const Style style
	)
{
	itsDefaultStyle = style;
}

/******************************************************************************
 Display menu style (static)

	This controls the style that is actually displayed in the menus.

 ******************************************************************************/

inline JXMenu::Style
JXMenu::GetDisplayStyle()
{
	return itsDisplayStyle;
}

inline void
JXMenu::SetDisplayStyle
	(
	const Style style
	)
{
	itsDisplayStyle = style;
}

#endif
