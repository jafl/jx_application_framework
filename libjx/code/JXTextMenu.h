/******************************************************************************
 JXTextMenu.h

	Interface for the JXTextMenu class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXTextMenu
#define _H_JXTextMenu

#include "JXMenu.h"
#include <jx-af/jcore/JFont.h>
#include "jx-af/jcore/JXPM.h"

class JXTextMenuData;
class JXImage;

class JXTextMenu : public JXMenu
{
public:

	JXTextMenu(const JString& title, JXContainer* enclosure,
			   const HSizingOption hSizing, const VSizingOption vSizing,
			   const JCoordinate x, const JCoordinate y,
			   const JCoordinate w, const JCoordinate h);

	JXTextMenu(JXImage* image, const bool menuOwnsImage,
			   JXContainer* enclosure,
			   const HSizingOption hSizing, const VSizingOption vSizing,
			   const JCoordinate x, const JCoordinate y,
			   const JCoordinate w, const JCoordinate h);

	JXTextMenu(JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	~JXTextMenu() override;

	void	SetMenuItems(const JUtf8Byte* menuStr);
	void	InsertMenuItems(const JIndex index, const JUtf8Byte* menuStr);
	void	PrependMenuItems(const JUtf8Byte* menuStr);
	void	AppendMenuItems(const JUtf8Byte* menuStr);

	void	InsertItem(const JIndex index, const JString& str,
					   const ItemType type = kPlainType,
					   const JString& shortcuts = JString::empty,
					   const JString& nmShortcut = JString::empty,
					   const JString& id = JString::empty);
	void	PrependItem(const JString& str,
					   const ItemType type = kPlainType,
						const JString& shortcuts = JString::empty,
						const JString& nmShortcut = JString::empty,
						const JString& id = JString::empty);
	void	AppendItem(const JString& str,
					   const ItemType type = kPlainType,
					   const JString& shortcuts = JString::empty,
					   const JString& nmShortcut = JString::empty,
					   const JString& id = JString::empty);

	const JString&	GetItemText(const JIndex index) const;
	void			SetItemText(const JIndex index, const JString& str);

	JFont	GetItemFont(const JIndex index) const;

	void	SetItemFontName(const JIndex index, const JString& name);
	void	SetItemFontSize(const JIndex index, const JSize size);
	void	SetItemFontStyle(const JIndex index, const JFontStyle& style);
	void	SetItemFont(const JIndex index, const JFont& font);

	const JFont&	GetDefaultFont() const;

	void	SetDefaultFontName(const JString& name, const bool updateExisting);
	void	SetDefaultFontSize(const JSize size, const bool updateExisting);
	void	SetDefaultFontStyle(const JFontStyle& style, const bool updateExisting);
	void	SetDefaultFont(const JFont& font, const bool updateExisting);

	bool	GetItemImage(const JIndex index, const JXImage** image) const;
	void	SetItemImage(const JIndex index, JXImage* image,
						 const bool menuOwnsImage);
	void	SetItemImage(const JIndex index, const JXPM& data);
	void	ClearItemImage(const JIndex index);

	bool	GetItemNMShortcut(const JIndex index, JString* str) const;
	void	SetItemNMShortcut(const JIndex index, const JString& str);

	bool	HasSeparatorAfter(const JIndex index) const;
	void	ShowSeparatorAfter(const JIndex index, const bool show = true);

	bool	HeightCompressed() const;
	void	CompressHeight(const bool compress = true);

	void	SetToPopupChoice(const bool isPopup,
							 const JIndex initialChoice) override;

	// called by JXWindow

	void	HandleNMShortcut(const JIndex index, const JXKeyModifiers& modifiers);

protected:

	JXMenuDirector*	CreateMenuWindow(JXWindowDirector* supervisor) override;
	void			AdjustPopupChoiceTitle(const JIndex index) override;

	JXTextMenuData*	GetTextMenuData();

private:

	JXTextMenuData*	itsTextMenuData;

private:

	void	JXTextMenuX();
};


/******************************************************************************
 GetTextMenuData (protected)

 ******************************************************************************/

inline JXTextMenuData*
JXTextMenu::GetTextMenuData()
{
	return itsTextMenuData;
}

#endif
