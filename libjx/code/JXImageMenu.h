/******************************************************************************
 JXImageMenu.h

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXImageMenu
#define _H_JXImageMenu

#include <JXMenu.h>
#include <JXImageMenuData.h>	// need defn of ItemType
#include <JXPM.h>

class JXImage;

class JXImageMenu : public JXMenu
{
public:

	JXImageMenu(const JCharacter* title, const JSize columnCount,
				JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	JXImageMenu(JXImage* image, const JBoolean menuOwnsImage,
				const JSize columnCount, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	JXImageMenu(const JSize columnCount, JXMenu* owner, const JIndex itemIndex,
				JXContainer* enclosure);

	virtual ~JXImageMenu();

	void			InsertItem(const JIndex index, JXImage* image,
							   const JBoolean menuOwnsImage,
							   const ItemType type = kPlainType,
							   const JCharacter* id = NULL);
	void			PrependItem(JXImage* image, const JBoolean menuOwnsImage,
							   const ItemType type = kPlainType,
								const JCharacter* id = NULL);
	void			AppendItem(JXImage* image, const JBoolean menuOwnsImage,
							   const ItemType type = kPlainType,
							   const JCharacter* id = NULL);

	const JXImage*	GetItemImage(const JIndex index) const;
	void			SetItemImage(const JIndex index, JXImage* image,
								 const JBoolean menuOwnsImage);
	void			SetItemImage(const JIndex index, const JXPM& data);

protected:

	virtual JXMenuDirector*	CreateMenuWindow(JXWindowDirector* supervisor);
	virtual void			AdjustPopupChoiceTitle(const JIndex index);

	JXImageMenuData*	GetIconMenuData();

private:

	JXImageMenuData*	itsIconMenuData;

private:

	void	JXImageMenuX(const JSize columnCount);

	// not allowed

	JXImageMenu(const JXImageMenu& source);
	const JXImageMenu& operator=(const JXImageMenu& source);
};


/******************************************************************************
 GetIconMenuData (protected)

 ******************************************************************************/

inline JXImageMenuData*
JXImageMenu::GetIconMenuData()
{
	return itsIconMenuData;
}

#endif
