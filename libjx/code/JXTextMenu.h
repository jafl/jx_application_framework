/******************************************************************************
 JXTextMenu.h

	Interface for the JXTextMenu class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXTextMenu
#define _H_JXTextMenu

#include "JXMenu.h"
#include <JFont.h>
#include "JXPM.h"

class JXTextMenuData;
class JXImage;

class JXTextMenu : public JXMenu
{
public:

	JXTextMenu(const JString& title, JXContainer* enclosure,
			   const HSizingOption hSizing, const VSizingOption vSizing,
			   const JCoordinate x, const JCoordinate y,
			   const JCoordinate w, const JCoordinate h);

	JXTextMenu(JXImage* image, const JBoolean menuOwnsImage,
			   JXContainer* enclosure,
			   const HSizingOption hSizing, const VSizingOption vSizing,
			   const JCoordinate x, const JCoordinate y,
			   const JCoordinate w, const JCoordinate h);

	JXTextMenu(JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	virtual ~JXTextMenu();

	void	SetMenuItems(const JUtf8Byte* menuStr,
						 const JUtf8Byte* idNamespace = nullptr);
	void	InsertMenuItems(const JIndex index, const JUtf8Byte* menuStr,
							const JUtf8Byte* idNamespace = nullptr);
	void	PrependMenuItems(const JUtf8Byte* menuStr,
							 const JUtf8Byte* idNamespace = nullptr);
	void	AppendMenuItems(const JUtf8Byte* menuStr,
							const JUtf8Byte* idNamespace = nullptr);

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

	void	SetDefaultFontName(const JString& name, const JBoolean updateExisting);
	void	SetDefaultFontSize(const JSize size, const JBoolean updateExisting);
	void	SetDefaultFontStyle(const JFontStyle& style, const JBoolean updateExisting);
	void	SetDefaultFont(const JFont& font, const JBoolean updateExisting);

	JBoolean	GetItemImage(const JIndex index, const JXImage** image) const;
	void		SetItemImage(const JIndex index, JXImage* image,
							 const JBoolean menuOwnsImage);
	void		SetItemImage(const JIndex index, const JXPM& data);
	void		ClearItemImage(const JIndex index);

	JBoolean	GetItemNMShortcut(const JIndex index, JString* str) const;
	void		SetItemNMShortcut(const JIndex index, const JString& str);

	JBoolean	HasSeparatorAfter(const JIndex index) const;
	void		ShowSeparatorAfter(const JIndex index, const JBoolean show = kJTrue);

	JBoolean	HeightCompressed() const;
	void		CompressHeight(const JBoolean compress = kJTrue);

	virtual void	SetToPopupChoice(const JBoolean isPopup,
									 const JIndex initialChoice);

	// called by JXWindow

	void	HandleNMShortcut(const JIndex index, const JXKeyModifiers& modifiers);

protected:

	virtual JXMenuDirector*	CreateMenuWindow(JXWindowDirector* supervisor);
	virtual void			AdjustPopupChoiceTitle(const JIndex index);

	JXTextMenuData*	GetTextMenuData();

private:

	JXTextMenuData*	itsTextMenuData;

private:

	void	JXTextMenuX();

	// not allowed

	JXTextMenu(const JXTextMenu& source);
	const JXTextMenu& operator=(const JXTextMenu& source);
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
