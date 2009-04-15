/******************************************************************************
 JXTextMenuDirector.h

	Interface for the JXTextMenuDirector class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXTextMenuDirector
#define _H_JXTextMenuDirector

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXMenuDirector.h>

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

private:

	// not allowed

	JXTextMenuDirector(const JXTextMenuDirector& source);
	const JXTextMenuDirector& operator=(const JXTextMenuDirector& source);
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
