/******************************************************************************
 JXImageMenuDirector.h

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXImageMenuDirector
#define _H_JXImageMenuDirector

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXMenuDirector.h>

class JXMenu;
class JXImageMenuData;

class JXImageMenuDirector : public JXMenuDirector
{
public:

	JXImageMenuDirector(JXDirector* supervisor,
						JXMenu* menu, JXImageMenuData* menuData);

	virtual ~JXImageMenuDirector();

protected:

	virtual JXMenuTable*	CreateMenuTable();

	JXMenu*				GetMenu();
	JXImageMenuData*	GetMenuData();

private:

	// We don't own these objects.  We only save their values
	// for use in CreateMenuTable().

	JXMenu*				itsMenu;
	JXImageMenuData*	itsMenuData;

private:

	// not allowed

	JXImageMenuDirector(const JXImageMenuDirector& source);
	const JXImageMenuDirector& operator=(const JXImageMenuDirector& source);
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
