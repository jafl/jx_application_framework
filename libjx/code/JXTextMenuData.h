/******************************************************************************
 JXTextMenuData.h

	Interface for the JXTextMenuData class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXTextMenuData
#define _H_JXTextMenuData

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

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

	void			InsertItem(const JIndex index, const JCharacter* str,
							   const JBoolean isCheckbox = kJFalse,
							   const JBoolean isRadio = kJFalse,
							   const JCharacter* shortcuts = NULL,
							   const JCharacter* nmShortcut = NULL,
							   const JCharacter* id = NULL);
	void			PrependItem(const JCharacter* str,
								const JBoolean isCheckbox = kJFalse,
								const JBoolean isRadio = kJFalse,
								const JCharacter* shortcuts = NULL,
								const JCharacter* nmShortcut = NULL,
								const JCharacter* id = NULL);
	void			AppendItem(const JCharacter* str,
							   const JBoolean isCheckbox = kJFalse,
							   const JBoolean isRadio = kJFalse,
							   const JCharacter* shortcuts = NULL,
							   const JCharacter* nmShortcut = NULL,
							   const JCharacter* id = NULL);
	virtual void	DeleteItem(const JIndex index);		// must call inherited
	virtual void	DeleteAll();						// must call inherited

	const JString&	GetText(const JIndex index) const;
	void			SetText(const JIndex index, const JCharacter* str);
	void			SetMenuItems(const JCharacter* menuStr,
								 const JCharacter* idNamespace = NULL);
	void			InsertMenuItems(const JIndex index, const JCharacter* menuStr,
									const JCharacter* idNamespace = NULL);
	void			PrependMenuItems(const JCharacter* menuStr,
									 const JCharacter* idNamespace = NULL);
	void			AppendMenuItems(const JCharacter* menuStr,
									const JCharacter* idNamespace = NULL);

	const JCharacter*	GetFontName(const JIndex index) const;
	JSize				GetFontSize(const JIndex index) const;
	const JFontStyle&	GetFontStyle(const JIndex index) const;
	void				GetFont(const JIndex index, JString* name,
								JSize* size, JFontStyle* style) const;
	void				GetFont(const JIndex index, JFontID* id,
								JSize* size, JFontStyle* style) const;

	void	SetFontName(const JIndex index, const JCharacter* name);
	void	SetFontSize(const JIndex index, const JSize size);
	void	SetFontStyle(const JIndex index, const JFontStyle& style);
	void	SetFont(const JIndex index, const JCharacter* name,
					const JSize size, const JFontStyle& style = JFontStyle());
	void	SetFont(const JIndex index, const JFontID id,
					const JSize size, const JFontStyle& style = JFontStyle());

	const JCharacter*	GetDefaultFontName() const;
	JSize				GetDefaultFontSize() const;
	const JFontStyle&	GetDefaultFontStyle() const;
	void				GetDefaultFont(JString* name, JSize* size,
									   JFontStyle* style) const;
	void				GetDefaultFont(JFontID* id, JSize* size,
									   JFontStyle* style) const;

	void	SetDefaultFontName(const JCharacter* name, const JBoolean updateExisting);
	void	SetDefaultFontSize(const JSize size, const JBoolean updateExisting);
	void	SetDefaultFontStyle(const JFontStyle& style, const JBoolean updateExisting);
	void	SetDefaultFont(const JCharacter* name, const JSize size,
						   const JFontStyle& style, const JBoolean updateExisting);
	void	SetDefaultFont(const JFontID id, const JSize size,
						   const JFontStyle& style, const JBoolean updateExisting);

	JBoolean	GetImage(const JIndex index, const JXImage** image) const;
	void		SetImage(const JIndex index, JXImage* image,
						 const JBoolean menuOwnsImage);
	void		ClearImage(const JIndex index);

	JBoolean	GetNMShortcut(const JIndex index, const JString** str) const;
	void		SetNMShortcut(const JIndex index, const JCharacter* str);

	JBoolean	HasSeparator(const JIndex index) const;
	void		ShowSeparatorAfter(const JIndex index, const JBoolean show = kJTrue);

	JBoolean	HeightCompressed() const;
	void		CompressHeight(const JBoolean compress = kJTrue);

	// called by JXTextMenuTable

	void	ConfigureTable(JXTextMenuTable* table);

	const JString&	GetText(const JIndex index, JIndex* ulIndex,
							JFontID* id, JSize* size, JFontStyle* style) const;
	JBoolean		GetNMShortcut(const JIndex index, const JString** str,
								  JFontID* id, JSize* size, JFontStyle* style) const;

protected:

	virtual void	ItemShortcutsChanged(const JIndex index, const JString* shortcuts);

private:

	struct TextItemData
	{
		JString*	text;
		JIndex		ulIndex;
		JString*	nmShortcut;			// can be NULL
		JFontID		fontID;
		JSize		fontSize;
		JFontStyle	fontStyle;
		JXImage*	image;				// can be NULL
		JBoolean	ownsImage;			// kJTrue if we should delete image
		JBoolean	separator;			// kJTrue if item is followed by separator

		TextItemData()
			:
			text( NULL ), ulIndex( 0 ), nmShortcut( NULL ),
			fontID( 0 ), fontSize( 0 ), fontStyle(),
			image( NULL ), ownsImage( kJTrue ), separator( kJFalse )
		{ };

		TextItemData(JString* str, const JFontID id, const JSize size,
					 const JFontStyle& style)
			:
			text( str ), ulIndex( 0 ), nmShortcut( NULL ),
			fontID( id ), fontSize( size ), fontStyle( style ),
			image( NULL ), ownsImage( kJTrue ), separator( kJFalse )
		{ };
	};

private:

	JXTextMenu*				itsMenu;		// it owns us
	const JFontManager*		itsFontMgr;
	JArray<TextItemData>*	itsTextItemData;

	JFontID		itsDefFontID;
	JSize		itsDefFontSize;
	JFontStyle	itsDefFontStyle;

	JBoolean				itsNeedGeomRecalcFlag;
	JCoordinate				itsMaxImageWidth;
	JCoordinate				itsMaxTextWidth;
	JCoordinate				itsMaxShortcutWidth;
	JBoolean				itsHasNMShortcutsFlag;
	JBoolean				itsCompressHeightFlag;
	JRunArray<JCoordinate>*	itsItemHeights;

private:

	void	ParseMenuItemStr(JString* text, JBoolean* isActive,
							 JBoolean* hasSeparator, JBoolean* isCheckbox,
							 JBoolean* isRadio, JString* shortcuts,
							 JString* nmShortcut, JString* id) const;
	void	CleanOutTextItem(TextItemData* itemData);

	JBoolean	ParseNMShortcut(JString* str, int* key,
								JXKeyModifiers* modifiers);
	void		AdjustNMShortcutString(JString* str, const JIndex origKeyIndex,
									   const JXModifierKey newKey);

	void	UpdateItemFonts(const JFontID oldID, const JSize oldSize, const JFontStyle& oldStyle,
							const JFontID newID, const JSize newSize, const JFontStyle& newStyle);

	// not allowed

	JXTextMenuData(const JXTextMenuData& source);
	const JXTextMenuData& operator=(const JXTextMenuData& source);

public:

	// JBroadcaster messages

	static const JCharacter* kImageChanged;

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
	const JCharacter*	str,
	const JBoolean		isCheckbox,
	const JBoolean		isRadio,
	const JCharacter*	shortcuts,
	const JCharacter*	nmShortcut,
	const JCharacter*	id
	)
{
	InsertItem(1, str, isCheckbox, isRadio, shortcuts, nmShortcut, id);
}

inline void
JXTextMenuData::AppendItem
	(
	const JCharacter*	str,
	const JBoolean		isCheckbox,
	const JBoolean		isRadio,
	const JCharacter*	shortcuts,
	const JCharacter*	nmShortcut,
	const JCharacter*	id
	)
{
	InsertItem(GetElementCount()+1, str, isCheckbox, isRadio, shortcuts, nmShortcut, id);
}

/******************************************************************************
 New items

 ******************************************************************************/

inline void
JXTextMenuData::PrependMenuItems
	(
	const JCharacter* menuStr,
	const JCharacter* idNamespace
	)
{
	InsertMenuItems(1, menuStr, idNamespace);
}

inline void
JXTextMenuData::AppendMenuItems
	(
	const JCharacter* menuStr,
	const JCharacter* idNamespace
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

inline JSize
JXTextMenuData::GetFontSize
	(
	const JIndex index
	)
	const
{
	const TextItemData itemData = itsTextItemData->GetElement(index);
	return itemData.fontSize;
}

inline const JFontStyle&
JXTextMenuData::GetFontStyle
	(
	const JIndex index
	)
	const
{
	const TextItemData itemData = itsTextItemData->GetElement(index);
	return itemData.fontStyle;
}

inline void
JXTextMenuData::GetFont
	(
	const JIndex	index,
	JFontID*		id,
	JSize*			size,
	JFontStyle*		style
	)
	const
{
	const TextItemData itemData = itsTextItemData->GetElement(index);
	*id    = itemData.fontID;
	*size  = itemData.fontSize;
	*style = itemData.fontStyle;
}

/******************************************************************************
 Get default font

 ******************************************************************************/

inline JSize
JXTextMenuData::GetDefaultFontSize()
	const
{
	return itsDefFontSize;
}

inline const JFontStyle&
JXTextMenuData::GetDefaultFontStyle()
	const
{
	return itsDefFontStyle;
}

inline void
JXTextMenuData::GetDefaultFont
	(
	JFontID*	id,
	JSize*		size,
	JFontStyle*	style
	)
	const
{
	*id    = itsDefFontID;
	*size  = itsDefFontSize;
	*style = itsDefFontStyle;
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
