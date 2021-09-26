/******************************************************************************
 JXTextMenuDirector.h

	Interface for the JXTextMenuDirector class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXTextMenuDirector
#define _H_JXTextMenuDirector

#include "jx-af/jx/JXMenuDirector.h"

class JXMenu;
class JXTextMenuData;

class JXTextMenuDirector : public JXMenuDirector
{
public:

	JXTextMenuDirector(JXDirector* supervisor,
					   JXMenu* menu, JXTextMenuData* menuData);

	virtual ~JXTextMenuDirector();

protected:

	virtual JXMenuTable*	CreateMenuTable();

	JXMenu*			GetMenu();
	JXTextMenuData*	GetMenuData();

private:

	// We don't own these objects.  We only save their values
	// for use in CreateMenuTable().

	JXMenu*			itsMenu;
	JXTextMenuData*	itsMenuData;
};


/******************************************************************************
 GetMenu (protected)

 ******************************************************************************/

inline JXMenu*
JXTextMenuDirector::GetMenu()
{
	return itsMenu;
}

/******************************************************************************
 GetMenuData (protected)

 ******************************************************************************/

inline JXTextMenuData*
JXTextMenuDirector::GetMenuData()
{
	return itsMenuData;
}

#endif
