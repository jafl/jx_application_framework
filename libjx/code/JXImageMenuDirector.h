/******************************************************************************
 JXImageMenuDirector.h

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXImageMenuDirector
#define _H_JXImageMenuDirector

#include "jx-af/jx/JXMenuDirector.h"

class JXMenu;
class JXImageMenuData;

class JXImageMenuDirector : public JXMenuDirector
{
public:

	JXImageMenuDirector(JXDirector* supervisor,
						JXMenu* menu, JXImageMenuData* menuData);

	~JXImageMenuDirector();

protected:

	JXMenuTable*	CreateMenuTable() override;

	JXMenu*				GetMenu();
	JXImageMenuData*	GetMenuData();

private:

	// We don't own these objects.  We only save their values
	// for use in CreateMenuTable().

	JXMenu*				itsMenu;
	JXImageMenuData*	itsMenuData;
};


/******************************************************************************
 GetMenu (protected)

 ******************************************************************************/

inline JXMenu*
JXImageMenuDirector::GetMenu()
{
	return itsMenu;
}

/******************************************************************************
 GetMenuData (protected)

 ******************************************************************************/

inline JXImageMenuData*
JXImageMenuDirector::GetMenuData()
{
	return itsMenuData;
}

#endif
