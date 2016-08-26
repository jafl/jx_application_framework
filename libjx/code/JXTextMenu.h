/******************************************************************************
 JXTextMenu.h

	Interface for the JXTextMenu class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXTextMenu
#define _H_JXTextMenu

#include <JXMenu.h>
#include <JFont.h>
#include <JXPM.h>

class JXTextMenuData;
class JXImage;

class JXTextMenu : public JXMenu
{
public:

	JXTextMenu(const JCharacter* title, JXContainer* enclosure,
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

	void	SetMenuItems(const JCharacter* menuStr,
						 const JCharacter* idNamespace = NULL);
	void	InsertMenuItems(const JIndex index, const JCharacter* menuStr,
							const JCharacter* idNamespace = NULL);
	void	PrependMenuItems(const JCharacter* menuStr,
							 const JCharacter* idNamespace = NULL);
	void	AppendMenuItems(const JCharacter* menuStr,
							const JCharacter* idNamespace = NULL);

	void	InsertItem(const JIndex index, const JCharacter* str,
					   const ItemType type = kPlainType,
					   const JCharacter* shortcuts = NULL,
					   const JCharacter* nmShortcut = NULL,
					   const JCharacter* id = NULL);
	void	PrependItem(const JCharacter* str,
					   const ItemType type = kPlainType,
						const JCharacter* shortcuts = NULL,
						const JCharacter* nmShortcut = NULL,
						const JCharacter* id = NULL);
	void	AppendItem(const JCharacter* str,
					   const ItemType type = kPlainType,
					   const JCharacter* shortcuts = NULL,
					   const JCharacter* nmShortcut = NULL,
					   const JCharacter* id = NULL);

	const JString&	GetItemText(const JIndex index) const;
	void			SetItemText(const JIndex index, const JCharacter* str);

	JFont	GetItemFont(const JIndex index) const;

	void	SetItemFontName(const JIndex index, const JCharacter* name);
	void	SetItemFontSize(const JIndex index, const JSize size);
	void	SetItemFontStyle(const JIndex index, const JFontStyle& style);
	void	SetItemFont(const JIndex index, const JFont& font);

	const JFont&	GetDefaultFont() const;

	void	SetDefaultFontName(const JCharacter* name, const JBoolean updateExisting);
	void	SetDefaultFontSize(const JSize size, const JBoolean updateExisting);
	void	SetDefaultFontStyle(const JFontStyle& style, const JBoolean updateExisting);
	void	SetDefaultFont(const JFont& font, const JBoolean updateExisting);

	JBoolean	GetItemImage(const JIndex index, const JXImage** image) const;
	void		SetItemImage(const JIndex index, JXImage* image,
							 const JBoolean menuOwnsImage);
	void		SetItemImage(const JIndex index, const JXPM& data);
	void		ClearItemImage(const JIndex index);

	JBoolean	GetItemNMShortcut(const JIndex index, JString* str) const;
	void		SetItemNMShortcut(const JIndex index, const JCharacter* str);

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
