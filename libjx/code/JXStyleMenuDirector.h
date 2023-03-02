/******************************************************************************
 JXStyleMenuDirector.h

	Interface for the JXStyleMenuDirector class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXStyleMenuDirector
#define _H_JXStyleMenuDirector

#include "JXTextMenuDirector.h"

class JXStyleMenu;

class JXStyleMenuDirector : public JXTextMenuDirector
{
public:

	JXStyleMenuDirector(JXDirector* supervisor,
						JXStyleMenu* menu, JXTextMenuData* menuData);

	~JXStyleMenuDirector() override;

protected:

	JXMenuTable*	CreateMenuTable() override;

private:

	JXStyleMenu*	itsMenu;	// it owns us
};

#endif
