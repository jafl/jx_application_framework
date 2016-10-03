/******************************************************************************
 JXStyleMenuDirector.h

	Interface for the JXStyleMenuDirector class

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXStyleMenuDirector
#define _H_JXStyleMenuDirector

#include <JXTextMenuDirector.h>

class JXStyleMenu;

class JXStyleMenuDirector : public JXTextMenuDirector
{
public:

	JXStyleMenuDirector(JXDirector* supervisor,
						JXStyleMenu* menu, JXTextMenuData* menuData);

	virtual ~JXStyleMenuDirector();

protected:

	virtual JXMenuTable*	CreateMenuTable();

private:

	JXStyleMenu*	itsStyleMenu;	// it owns us

private:

	// not allowed

	JXStyleMenuDirector(const JXStyleMenuDirector& source);
	const JXStyleMenuDirector& operator=(const JXStyleMenuDirector& source);
};

#endif
