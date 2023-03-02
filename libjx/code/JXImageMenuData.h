/******************************************************************************
 JXImageMenuData.h

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXImageMenuData
#define _H_JXImageMenuData

#include "JXMenuData.h"
#include <jx-af/jcore/JArray.h>

class JXImage;
class JXImageMenuTable;

class JXImageMenuData : public JXMenuData
{
public:

	JXImageMenuData(const JSize columnCount);

	~JXImageMenuData() override;

	void	InsertItem(const JIndex index, JXImage* image,
					   const bool menuOwnsImage,
					   const JXMenu::ItemType type = JXMenu::kPlainType,
					   const JString& id = JString::empty);
	void	PrependItem(JXImage* image, const bool menuOwnsImage,
					   const JXMenu::ItemType type = JXMenu::kPlainType,
						const JString& id = JString::empty);
	void	AppendItem(JXImage* image, const bool menuOwnsImage,
					   const JXMenu::ItemType type = JXMenu::kPlainType,
					   const JString& id = JString::empty);
	void	DeleteItem(const JIndex index) override;	// must call inherited
	void	DeleteAll() override;						// must call inherited

	const JXImage*	GetImage(const JIndex index) const;
	void			SetImage(const JIndex index, JXImage* image,
							 const bool menuOwnsImage);

	JSize	GetColumnCount() const;
	void	SetColumnCount(const JSize columnCount);

	// called by JXImageMenuTable

	void	ConfigureTable(JXImageMenuTable* table,
						   bool* hasCheckboxes, bool* hasSubmenus);

private:

	struct IconData
	{
		JXImage*	image;
		bool	ownsImage;	// true if we should delete image

		IconData()
			:
			image( nullptr ), ownsImage( true )
		{ };

		IconData(JXImage* i, const bool own)
			:
			image( i ), ownsImage( own )
		{ };
	};

private:

	JArray<IconData>*	itsIconData;
	JSize				itsColumnCount;

	bool			itsNeedGeomRecalcFlag;
	JCoordinate			itsRowHeight;
	JCoordinate			itsColWidth;

private:

	void	CleanOutIconItem(IconData* itemData);

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
JXImageMenuData::PrependItem
	(
	JXImage*				image,
	const bool			menuOwnsImage,
	const JXMenu::ItemType	type,
	const JString&			id
	)
{
	InsertItem(1, image, menuOwnsImage, type, id);
}

inline void
JXImageMenuData::AppendItem
	(
	JXImage*				image,
	const bool			menuOwnsImage,
	const JXMenu::ItemType	type,
	const JString&			id
	)
{
	InsertItem(GetElementCount()+1, image, menuOwnsImage, type, id);
}

/******************************************************************************
 GetImage

 ******************************************************************************/

inline const JXImage*
JXImageMenuData::GetImage
	(
	const JIndex index
	)
	const
{
	const IconData itemData = itsIconData->GetElement(index);
	return itemData.image;
}

/******************************************************************************
 Item count per row

 ******************************************************************************/

inline JSize
JXImageMenuData::GetColumnCount()
	const
{
	return itsColumnCount;
}

inline void
JXImageMenuData::SetColumnCount
	(
	const JSize columnCount
	)
{
	itsColumnCount = columnCount;
}

#endif
