/******************************************************************************
 JXImageMenu.h

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXImageMenu
#define _H_JXImageMenu

#include "JXMenu.h"
#include "JXImageMenuData.h"	// need defn of ItemType
#include "jx-af/jcore/JXPM.h"

class JXImage;

class JXImageMenu : public JXMenu
{
public:

	JXImageMenu(const JString& title, const JSize columnCount,
				JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	JXImageMenu(JXImage* image, const bool menuOwnsImage,
				const JSize columnCount, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	JXImageMenu(const JSize columnCount, JXMenu* owner, const JIndex itemIndex,
				JXContainer* enclosure);

	~JXImageMenu() override;

	void			InsertItem(const JIndex index, JXImage* image,
							   const bool menuOwnsImage,
							   const ItemType type = kPlainType,
							   const JString& id = JString::empty);
	void			PrependItem(JXImage* image, const bool menuOwnsImage,
							   const ItemType type = kPlainType,
								const JString& id = JString::empty);
	void			AppendItem(JXImage* image, const bool menuOwnsImage,
							   const ItemType type = kPlainType,
							   const JString& id = JString::empty);

	const JXImage*	GetItemImage(const JIndex index) const;
	void			SetItemImage(const JIndex index, JXImage* image,
								 const bool menuOwnsImage);
	void			SetItemImage(const JIndex index, const JXPM& data);

protected:

	JXMenuDirector*	CreateMenuWindow(JXWindowDirector* supervisor) override;
	void			AdjustPopupChoiceTitle(const JIndex index) override;

	JXImageMenuData*	GetIconMenuData();

private:

	JXImageMenuData*	itsIconMenuData;

private:

	void	JXImageMenuX(const JSize columnCount);
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
