/******************************************************************************
 JXImageMenuData.h

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXImageMenuData
#define _H_JXImageMenuData

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXMenuData.h>
#include <JArray.h>

class JXImage;
class JXImageMenuTable;

class JXImageMenuData : public JXMenuData
{
public:

	JXImageMenuData(const JSize columnCount);

	virtual ~JXImageMenuData();

	void			InsertItem(const JIndex index, JXImage* image,
							   const JBoolean menuOwnsImage,
							   const JBoolean isCheckbox = kJFalse,
							   const JBoolean isRadio = kJFalse,
							   const JCharacter* id = NULL);
	void			PrependItem(JXImage* image, const JBoolean menuOwnsImage,
								const JBoolean isCheckbox = kJFalse,
								const JBoolean isRadio = kJFalse,
								const JCharacter* id = NULL);
	void			AppendItem(JXImage* image, const JBoolean menuOwnsImage,
							   const JBoolean isCheckbox = kJFalse,
							   const JBoolean isRadio = kJFalse,
							   const JCharacter* id = NULL);
	virtual void	DeleteItem(const JIndex index);		// must call inherited
	virtual void	DeleteAll();						// must call inherited

	const JXImage*	GetImage(const JIndex index) const;
	void			SetImage(const JIndex index, JXImage* image,
							 const JBoolean menuOwnsImage);

	JSize	GetColumnCount() const;
	void	SetColumnCount(const JSize columnCount);

	// called by JXImageMenuTable

	void	ConfigureTable(JXImageMenuTable* table,
						   JBoolean* hasCheckboxes, JBoolean* hasSubmenus);

private:

	struct IconData
	{
		JXImage*	image;
		JBoolean	ownsImage;	// kJTrue if we should delete image

		IconData()
			:
			image( NULL ), ownsImage( kJTrue )
		{ };

		IconData(JXImage* i, const JBoolean own)
			:
			image( i ), ownsImage( own )
		{ };
	};

private:

	JArray<IconData>*	itsIconData;
	JSize				itsColumnCount;

	JBoolean			itsNeedGeomRecalcFlag;
	JCoordinate			itsRowHeight;
	JCoordinate			itsColWidth;

private:

	void	CleanOutIconItem(IconData* itemData);

	// not allowed

	JXImageMenuData(const JXImageMenuData& source);
	const JXImageMenuData& operator=(const JXImageMenuData& source);

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
JXImageMenuData::PrependItem
	(
	JXImage*			image,
	const JBoolean		menuOwnsImage,
	const JBoolean		isCheckbox,
	const JBoolean		isRadio,
	const JCharacter*	id
	)
{
	InsertItem(1, image, menuOwnsImage, isCheckbox, isRadio, id);
}

inline void
JXImageMenuData::AppendItem
	(
	JXImage*			image,
	const JBoolean		menuOwnsImage,
	const JBoolean		isCheckbox,
	const JBoolean		isRadio,
	const JCharacter*	id
	)
{
	InsertItem(GetElementCount()+1, image, menuOwnsImage, isCheckbox, isRadio, id);
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
