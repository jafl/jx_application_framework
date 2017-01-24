/******************************************************************************
 JXTextMenuData.h

	Interface for the JXTextMenuData class

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXTextMenuData
#define _H_JXTextMenuData

#include <JXMenuData.h>
#include <JFontStyle.h>
#include <JRunArray.h>

class JFontManager;
class JXImage;
class JXTextMenu;
class JXTextMenuTable;

class JXTextMenuData : public JXMenuData
{
public:

	JXTextMenuData(JXTextMenu* menu);

	virtual ~JXTextMenuData();

	void			InsertItem(const JIndex index, const JString& str,
							   const JXMenu::ItemType type = JXMenu::kPlainType,
							   const JString* shortcuts = NULL,
							   const JString* nmShortcut = NULL,
							   const JString* id = NULL);
	void			PrependItem(const JString& str,
							   const JXMenu::ItemType type = JXMenu::kPlainType,
								const JString* shortcuts = NULL,
								const JString* nmShortcut = NULL,
								const JString* id = NULL);
	void			AppendItem(const JString& str,
							   const JXMenu::ItemType type = JXMenu::kPlainType,
							   const JString* shortcuts = NULL,
							   const JString* nmShortcut = NULL,
							   const JString* id = NULL);
	virtual void	DeleteItem(const JIndex index);		// must call inherited
	virtual void	DeleteAll();						// must call inherited

	const JString&	GetText(const JIndex index) const;
	void			SetText(const JIndex index, const JString& str);
	void			SetMenuItems(const JUtf8Byte* menuStr,
								 const JUtf8Byte* idNamespace = NULL);
	void			InsertMenuItems(const JIndex index, const JUtf8Byte* menuStr,
									const JUtf8Byte* idNamespace = NULL);
	void			PrependMenuItems(const JUtf8Byte* menuStr,
									 const JUtf8Byte* idNamespace = NULL);
	void			AppendMenuItems(const JUtf8Byte* menuStr,
									const JUtf8Byte* idNamespace = NULL);

	JFont	GetFont(const JIndex index) const;

	void	SetFontName(const JIndex index, const JString& name);
	void	SetFontSize(const JIndex index, const JSize size);
	void	SetFontStyle(const JIndex index, const JFontStyle& style);
	void	SetFont(const JIndex index, const JFont& font);

	const JFont&	GetDefaultFont() const;

	void	SetDefaultFontName(const JString& name, const JBoolean updateExisting);
	void	SetDefaultFontSize(const JSize size, const JBoolean updateExisting);
	void	SetDefaultFontStyle(const JFontStyle& style, const JBoolean updateExisting);
	void	SetDefaultFont(const JFont& font, const JBoolean updateExisting);

	JBoolean	GetImage(const JIndex index, const JXImage** image) const;
	void		SetImage(const JIndex index, JXImage* image,
						 const JBoolean menuOwnsImage);
	void		ClearImage(const JIndex index);

	JBoolean	GetNMShortcut(const JIndex index, const JString** str) const;
	void		SetNMShortcut(const JIndex index, const JString& str);

	JBoolean	HasSeparator(const JIndex index) const;
	void		ShowSeparatorAfter(const JIndex index, const JBoolean show = kJTrue);

	JBoolean	HeightCompressed() const;
	void		CompressHeight(const JBoolean compress = kJTrue);

	// called by JXTextMenuTable

	void	ConfigureTable(JXTextMenuTable* table);

	const JString&	GetText(const JIndex index, JIndex* ulIndex, JFont* font) const;
	JBoolean		GetNMShortcut(const JIndex index, const JString** str,
								  JFont* font) const;

protected:

	virtual void	ItemShortcutsChanged(const JIndex index, const JString* shortcuts);

private:

	struct TextItemData
	{
		JString*	text;
		JIndex		ulIndex;
		JString*	nmShortcut;			// can be NULL
		JFont		font;
		JXImage*	image;				// can be NULL
		JBoolean	ownsImage;			// kJTrue if we should delete image
		JBoolean	separator;			// kJTrue if item is followed by separator

		TextItemData()
			:
			text( NULL ), ulIndex( 0 ), nmShortcut( NULL ),
			image( NULL ), ownsImage( kJTrue ), separator( kJFalse )
		{ };

		TextItemData(JString* str, const JFont& f)
			:
			text( str ), ulIndex( 0 ), nmShortcut( NULL ), font( f ),
			image( NULL ), ownsImage( kJTrue ), separator( kJFalse )
		{ };
	};

private:

	JXTextMenu*				itsMenu;		// it owns us
	const JFontManager*		itsFontMgr;
	JArray<TextItemData>*	itsTextItemData;

	JFont	itsDefFont;

	JBoolean				itsNeedGeomRecalcFlag;
	JCoordinate				itsMaxImageWidth;
	JCoordinate				itsMaxTextWidth;
	JCoordinate				itsMaxShortcutWidth;
	JBoolean				itsHasNMShortcutsFlag;
	JBoolean				itsCompressHeightFlag;
	JRunArray<JCoordinate>*	itsItemHeights;

private:

	void	ParseMenuItemStr(JString* text, JBoolean* isActive,
							 JBoolean* hasSeparator, JXMenu::ItemType* type,
							 JString* shortcuts, JString* nmShortcut,
							 JString* id) const;
	void	CleanOutTextItem(TextItemData* itemData);

	JBoolean	ParseNMShortcut(JString* str, int* key,
								JXKeyModifiers* modifiers);
	void		AdjustNMShortcutString(JString* str, const JIndex origKeyIndex,
									   const JXModifierKey newKey);

	void	UpdateItemFonts(const JFont& oldFont, const JFont& newFont);

	// not allowed

	JXTextMenuData(const JXTextMenuData& source);
	const JXTextMenuData& operator=(const JXTextMenuData& source);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kImageChanged;

	class ImageChanged : public JBroadcaster::Message
		{
		public:

			ImageChanged(const JIndex index)
				:
				JBroadcaster::Message(kImageChanged),
				itsIndex(index)
			{ };

			JIndex
			GetIndex() const
			{
				return itsIndex;
			};

		private:

			JIndex itsIndex;
		};
};


/******************************************************************************
 New item

 ******************************************************************************/

inline void
JXTextMenuData::PrependItem
	(
	const JString&			str,
	const JXMenu::ItemType	type,
	const JString*			shortcuts,
	const JString*			nmShortcut,
	const JString*			id
	)
{
	InsertItem(1, str, type, shortcuts, nmShortcut, id);
}

inline void
JXTextMenuData::AppendItem
	(
	const JString&			str,
	const JXMenu::ItemType	type,
	const JString*			shortcuts,
	const JString*			nmShortcut,
	const JString*			id
	)
{
	InsertItem(GetElementCount()+1, str, type, shortcuts, nmShortcut, id);
}

/******************************************************************************
 New items

 ******************************************************************************/

inline void
JXTextMenuData::PrependMenuItems
	(
	const JUtf8Byte* menuStr,
	const JUtf8Byte* idNamespace
	)
{
	InsertMenuItems(1, menuStr, idNamespace);
}

inline void
JXTextMenuData::AppendMenuItems
	(
	const JUtf8Byte* menuStr,
	const JUtf8Byte* idNamespace
	)
{
	InsertMenuItems(GetElementCount()+1, menuStr, idNamespace);
}

/******************************************************************************
 GetText

 ******************************************************************************/

inline const JString&
JXTextMenuData::GetText
	(
	const JIndex index
	)
	const
{
	const TextItemData itemData = itsTextItemData->GetElement(index);
	return *(itemData.text);
}

/******************************************************************************
 Get font

 ******************************************************************************/

inline JFont
JXTextMenuData::GetFont
	(
	const JIndex index
	)
	const
{
	const TextItemData itemData = itsTextItemData->GetElement(index);
	return itemData.font;
}

/******************************************************************************
 Get default font

 ******************************************************************************/

inline const JFont&
JXTextMenuData::GetDefaultFont()
	const
{
	return itsDefFont;
}

/******************************************************************************
 HasSeparator

 ******************************************************************************/

inline JBoolean
JXTextMenuData::HasSeparator
	(
	const JIndex index
	)
	const
{
	const TextItemData itemData = itsTextItemData->GetElement(index);
	return itemData.separator;
}

/******************************************************************************
 Compress height

	This takes effect next time the menu is displayed.

 ******************************************************************************/

inline JBoolean
JXTextMenuData::HeightCompressed()
	const
{
	return itsCompressHeightFlag;
}

inline void
JXTextMenuData::CompressHeight
	(
	const JBoolean compress
	)
{
	if (itsCompressHeightFlag != compress)
		{
		itsCompressHeightFlag = compress;
		itsNeedGeomRecalcFlag = kJTrue;
		}
}

#endif
