/******************************************************************************
 JXMenu.h

	Interface for the JXMenu class

	Copyright (C) 1996-98 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXMenu
#define _H_JXMenu

#include "JXWidget.h"
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/JFont.h>

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

	JXMenu(const JString& title, JXContainer* enclosure,
		   const HSizingOption hSizing, const VSizingOption vSizing,
		   const JCoordinate x, const JCoordinate y,
		   const JCoordinate w, const JCoordinate h);

	JXMenu(JXImage* image, const bool menuOwnsImage, JXContainer* enclosure,
		   const HSizingOption hSizing, const VSizingOption vSizing,
		   const JCoordinate x, const JCoordinate y,
		   const JCoordinate w, const JCoordinate h);

	JXMenu(const JString& title, JXImage* image, const bool menuOwnsImage,
		   JXContainer* enclosure,
		   const HSizingOption hSizing, const VSizingOption vSizing,
		   const JCoordinate x, const JCoordinate y,
		   const JCoordinate w, const JCoordinate h);

	JXMenu(JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	~JXMenu() override;

	bool	IsOpen() const;

	const JString&	GetTitleText() const;
	bool			GetTitleText(const JString** text) const;
	bool			GetTitleImage(const JXImage** image) const;
	void			SetTitle(const JString& title,
							 JXImage* image, const bool menuOwnsImage);
	void			SetTitleText(const JString& title);
	void			SetTitleImage(JXImage* image, const bool menuOwnsImage);

	void	SetTitleFontName(const JString& fontName);
	void	SetTitleFontSize(const JSize size);
	void	SetTitleFontStyle(const JFontStyle& style);
	void	SetTitleFont(const JFont& font);

	const JPoint&	GetTitlePadding() const;
	void			SetTitlePadding(const JPoint& p);

	void	SetShortcuts(const JString& list);

	void	AttachHandlers(JBroadcaster* target,
						   const std::function<void(void)>& updater,
						   const std::function<void(const JIndex)>& handler);

	bool	IsEmpty() const;
	JSize	GetItemCount() const;
	void	RemoveItem(const JIndex index);
	void	RemoveAllItems();

	bool	GetItemShortcuts(const JIndex index, JString* shortcuts) const;
	void	SetItemShortcuts(const JIndex index, const JString& shortcuts);

	bool	GetItemID(const JIndex index, const JString** id) const;
	void	SetItemID(const JIndex index, const JString& id);
	bool	ItemIDToIndex(const JString& targetID, JIndex* index) const;

	bool	IsEnabled(const JIndex index) const;
	void	EnableItem(const JIndex index);
	void	EnableAll();
	void	DisableItem(const JIndex index);
	void	DisableAll();
	void	SetItemEnabled(const JIndex index, const bool enabled);

	bool	IsChecked(const JIndex index) const;
	void	CheckItem(const JIndex index);

	bool	GetSubmenu(const JIndex index, const JXMenu** menu) const;
	void	AttachSubmenu(const JIndex index, JXMenu* submenu);
	bool	RemoveSubmenu(const JIndex index, JXMenu** theMenu);
	void	DeleteSubmenu(const JIndex index);

	UpdateAction	GetUpdateAction() const;
	void			SetUpdateAction(const UpdateAction action);

	bool			IsPopupChoice() const;
	virtual void	SetToPopupChoice(const bool isPopup,
									 const JIndex initialChoice);
	void			SetPopupChoice(const JIndex index);

	void	Activate() override;
	void	Deactivate() override;

	void	Suspend() override;
	void	Resume() override;

	bool	IsHiddenPopupMenu() const;
	void	SetToHiddenPopupMenu(const bool isHiddenPopup = true);
	bool	PopUp(JXContainer* mouseOwner, const JPoint& pt,
				  const JXButtonStates&	buttonStates,
				  const JXKeyModifiers&	modifiers);

	bool	GetMenuBar(JXMenuBar** menuBar) const;
	JXMenu*		GetTopLevelMenu() const;

	ArrowPosition	GetPopupArrowPosition() const;
	void			SetPopupArrowPosition(const ArrowPosition position);

	ArrowDirection	GetPopupArrowDirection() const;
	void			SetPopupArrowDirection(const ArrowDirection direction);

	bool	IsMenu() const override;

	void	HandleShortcut(const int key, const JXKeyModifiers& modifiers) override;

	static Style	GetDefaultStyle();
	static void		SetDefaultStyle(const Style style);

	static Style	GetDisplayStyle();
	static void		SetDisplayStyle(const Style style);

	static JXModifierKey	AdjustNMShortcutModifier(const JXModifierKey key);

protected:

	void	SetBaseItemData(JXMenuData* baseItemData);
	void	ClearBaseItemData();
	bool	PrepareToOpenMenu(const bool shortcut);

	virtual JXMenuDirector*	CreateMenuWindow(JXWindowDirector* supervisor) = 0;
	virtual void			AdjustPopupChoiceTitle(const JIndex index) = 0;

	void	Draw(JXWindowPainter& p, const JRect& rect) override;
	void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;

	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

	bool		IncludeInFTC() const override;
	JCoordinate	GetFTCMinContentSize(const bool horizontal) const override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

	// called by JXMenuTable

	void	BroadcastSelection(const JIndex itemIndex, const bool fromShortcut);

private:

	JString		itsTitle;
	JXImage*	itsTitleImage;			// can be nullptr
	bool		itsOwnsTitleImageFlag;	// true => we delete it
	JPoint		itsTitlePadding;
	JString*	itsShortcuts;			// can be nullptr
	JIndex		itsULIndex;
	JXMenuData*	itsBaseItemData;		// derived class owns this
	JXMenuBar*	itsMenuBar;				// can be nullptr; if there is one, it owns us
	JXMenu*		itsOwner;				// nullptr if top level menu

	JFont	itsTitleFont;
	bool	itsShouldBeActiveFlag;	// true  => last client call was Activate()
	bool	itsUpdateSBAFlag;		// false => don't change itsShouldBeActiveFlag
	JSize	itsMinWidth;
	bool	itsWaitingForFTCFlag;

	UpdateAction	itsUpdateAction;
	bool			itsIsPopupChoiceFlag;
	bool			itsIsHiddenPopupMenuFlag;

	ArrowPosition	itsArrowPosition;	// location of arrow when menu is free-standing
	ArrowDirection	itsArrowDirection;	// direction of arrow when menu is free-standing

	static Style	theDefaultStyle;
	static Style	theDisplayStyle;

	// used when menu is pulled down

	JXMenuDirector*	itsMenuDirector;
	bool*			itsDeletedFlag;		// can be nullptr

private:

	void	JXMenuX(const JString& title, JXImage* image,
					const bool menuOwnsImage);
	void	RemoveSubmenu(JXMenu* theMenu);
	void	SetOwner(JXMenu* owner);
	void	UpdateTitleGeometry();
	void	AdjustAppearance();
	void	AdjustAppearance(const JCoordinate minWidth);
	JSize	GetMaxPopupChoiceTitleWidth() const;

	bool	Open(const JPoint& leftPtR = JPoint(),
				 const JPoint& rightPtR = JPoint());
	void	Close();

	void	PrivateActivate();
	void	PrivateDeactivate();

	// called by JXMenuBar

	void	SetMenuBar(JXMenuBar* bar);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kNeedsUpdate;
	static const JUtf8Byte* kItemSelected;

	class NeedsUpdate : public JBroadcaster::Message
	{
	public:

		NeedsUpdate(const bool fromShortcut)
			:
			JBroadcaster::Message(kNeedsUpdate),
			itsFromShortcutFlag(fromShortcut)
			{ };

		bool
		IsFromShortcut() const
		{
			return itsFromShortcutFlag;
		};

	private:

		const bool itsFromShortcutFlag;
	};

	class ItemSelected : public JBroadcaster::Message
	{
	public:

		ItemSelected(const JIndex index, const bool fromShortcut)
			:
			JBroadcaster::Message(kItemSelected),
			itsIndex(index), itsFromShortcutFlag(fromShortcut)
			{ };

		JIndex
		GetIndex() const
		{
			return itsIndex;
		};

		bool
		IsFromShortcut() const
		{
			return itsFromShortcutFlag;
		};

	private:

		const JIndex	itsIndex;
		const bool		itsFromShortcutFlag;
	};
};


/******************************************************************************
 IsOpen

 ******************************************************************************/

inline bool
JXMenu::IsOpen()
	const
{
	return itsMenuDirector != nullptr;
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

inline bool
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

inline bool
JXMenu::GetTitleImage
	(
	const JXImage** image
	)
	const
{
	*image = itsTitleImage;
	return itsTitleImage != nullptr;
}

/******************************************************************************
 Title padding

 ******************************************************************************/

inline const JPoint&
JXMenu::GetTitlePadding()
	const
{
	return itsTitlePadding;
}

inline void
JXMenu::SetTitlePadding
	(
	const JPoint& p
	)
{
	itsTitlePadding = p;
	AdjustAppearance();
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

inline bool
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

inline bool
JXMenu::IsHiddenPopupMenu()
	const
{
	return itsIsHiddenPopupMenuFlag;
}

inline void
JXMenu::SetToHiddenPopupMenu
	(
	const bool isHiddenPopup
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
 Default menu style (static)

	This controls the style of all menus created by JX and also indicates
	what style the programmer hard-coded into the application menus.

 ******************************************************************************/

inline JXMenu::Style
JXMenu::GetDefaultStyle()
{
	return theDefaultStyle;
}

inline void
JXMenu::SetDefaultStyle
	(
	const Style style
	)
{
	theDefaultStyle = style;
}

/******************************************************************************
 Display menu style (static)

	This controls the style that is actually displayed in the menus.

 ******************************************************************************/

inline JXMenu::Style
JXMenu::GetDisplayStyle()
{
	return theDisplayStyle;
}

inline void
JXMenu::SetDisplayStyle
	(
	const Style style
	)
{
	theDisplayStyle = style;
}

#endif
