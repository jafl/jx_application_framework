/******************************************************************************
 JXTextMenuData.h

	Interface for the JXTextMenuData class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXTextMenuData
#define _H_JXTextMenuData

#include "JXMenuData.h"
#include <jx-af/jcore/JFontStyle.h>
#include <jx-af/jcore/JRunArray.h>

class JXImage;
class JXTextMenu;
class JXTextMenuTable;

class JXTextMenuData : public JXMenuData
{
public:

	JXTextMenuData(JXTextMenu* menu);

	~JXTextMenuData() override;

	void	InsertItem(const JIndex index, const JString& str,
					   const JXMenu::ItemType type = JXMenu::kPlainType,
					   const JString& shortcuts = JString::empty,
					   const JString& nmShortcut = JString::empty,
					   const JString& id = JString::empty);
	void	PrependItem(const JString& str,
						const JXMenu::ItemType type = JXMenu::kPlainType,
						const JString& shortcuts = JString::empty,
						const JString& nmShortcut = JString::empty,
						const JString& id = JString::empty);
	void	AppendItem(const JString& str,
					   const JXMenu::ItemType type = JXMenu::kPlainType,
					   const JString& shortcuts = JString::empty,
					   const JString& nmShortcut = JString::empty,
					   const JString& id = JString::empty);
	void	DeleteItem(const JIndex index) override;		// must call inherited
	void	DeleteAll() override;						// must call inherited

	const JString&	GetText(const JIndex index) const;
	void			SetText(const JIndex index, const JString& str);
	void			SetMenuItems(const JUtf8Byte* menuStr);
	void			InsertMenuItems(const JIndex index, const JUtf8Byte* menuStr);
	void			PrependMenuItems(const JUtf8Byte* menuStr);
	void			AppendMenuItems(const JUtf8Byte* menuStr);

	JFont	GetFont(const JIndex index) const;

	void	SetFontName(const JIndex index, const JString& name);
	void	SetFontSize(const JIndex index, const JSize size);
	void	SetFontStyle(const JIndex index, const JFontStyle& style);
	void	SetFont(const JIndex index, const JFont& font);

	const JFont&	GetDefaultFont() const;

	void	SetDefaultFontName(const JString& name, const bool updateExisting);
	void	SetDefaultFontSize(const JSize size, const bool updateExisting);
	void	SetDefaultFontStyle(const JFontStyle& style, const bool updateExisting);
	void	SetDefaultFont(const JFont& font, const bool updateExisting);

	bool	GetImage(const JIndex index, const JXImage** image) const;
	void	SetImage(const JIndex index, JXImage* image,
					 const bool menuOwnsImage);
	void	ClearImage(const JIndex index);

	bool	GetNMShortcut(const JIndex index, const JString** str) const;
	void	SetNMShortcut(const JIndex index, const JString& str);

	bool	HasSeparator(const JIndex index) const;
	void	ShowSeparatorAfter(const JIndex index, const bool show = true);

	bool	HeightCompressed() const;
	void	CompressHeight(const bool compress = true);

	// called by JXTextMenuTable

	void	ConfigureTable(JXTextMenuTable* table);

	const JString&	GetText(const JIndex index, JIndex* ulIndex, JFont* font) const;
	bool			GetNMShortcut(const JIndex index, const JString** str,
								  JFont* font) const;

protected:

	void	ItemShortcutsChanged(const JIndex index, const JString* shortcuts) override;

private:

	struct TextItemData
	{
		JString*	text;
		JIndex		ulIndex;
		JString*	nmShortcut;			// can be nullptr
		JFont		font;
		JXImage*	image;				// can be nullptr
		bool		ownsImage;			// true if we should delete image
		bool		separator;			// true if item is followed by separator

		TextItemData()
			:
			text( nullptr ), ulIndex( 0 ), nmShortcut( nullptr ),
			image( nullptr ), ownsImage( true ), separator( false )
		{ };

		TextItemData(JString* str, const JFont& f)
			:
			text( str ), ulIndex( 0 ), nmShortcut( nullptr ), font( f ),
			image( nullptr ), ownsImage( true ), separator( false )
		{ };
	};

private:

	JXTextMenu*				itsMenu;		// it owns us
	JArray<TextItemData>*	itsTextItemData;

	JFont	itsDefaultFont;

	bool					itsNeedGeomRecalcFlag;
	JCoordinate				itsMaxImageWidth;
	JCoordinate				itsMaxTextWidth;
	JCoordinate				itsMaxShortcutWidth;
	bool					itsHasNMShortcutsFlag;
	bool					itsCompressHeightFlag;
	JRunArray<JCoordinate>*	itsItemHeights;

private:

	void	CleanOutTextItem(TextItemData* itemData);

	void	UpdateItemFonts(const JFont& oldFont, const JFont& newFont);

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
	const JString&			shortcuts,
	const JString&			nmShortcut,
	const JString&			id
	)
{
	InsertItem(1, str, type, shortcuts, nmShortcut, id);
}

inline void
JXTextMenuData::AppendItem
	(
	const JString&			str,
	const JXMenu::ItemType	type,
	const JString&			shortcuts,
	const JString&			nmShortcut,
	const JString&			id
	)
{
	InsertItem(GetItemCount()+1, str, type, shortcuts, nmShortcut, id);
}

/******************************************************************************
 New items

 ******************************************************************************/

inline void
JXTextMenuData::PrependMenuItems
	(
	const JUtf8Byte* menuStr
	)
{
	InsertMenuItems(1, menuStr);
}

inline void
JXTextMenuData::AppendMenuItems
	(
	const JUtf8Byte* menuStr
	)
{
	InsertMenuItems(GetItemCount()+1, menuStr);
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
	const TextItemData itemData = itsTextItemData->GetItem(index);
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
	const TextItemData itemData = itsTextItemData->GetItem(index);
	return itemData.font;
}

/******************************************************************************
 Get default font

 ******************************************************************************/

inline const JFont&
JXTextMenuData::GetDefaultFont()
	const
{
	return itsDefaultFont;
}

/******************************************************************************
 HasSeparator

 ******************************************************************************/

inline bool
JXTextMenuData::HasSeparator
	(
	const JIndex index
	)
	const
{
	const TextItemData itemData = itsTextItemData->GetItem(index);
	return itemData.separator;
}

/******************************************************************************
 Compress height

	This takes effect next time the menu is displayed.

 ******************************************************************************/

inline bool
JXTextMenuData::HeightCompressed()
	const
{
	return itsCompressHeightFlag;
}

inline void
JXTextMenuData::CompressHeight
	(
	const bool compress
	)
{
	if (itsCompressHeightFlag != compress)
		{
		itsCompressHeightFlag = compress;
		itsNeedGeomRecalcFlag = true;
		}
}

#endif
