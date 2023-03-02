/******************************************************************************
 JXStyleMenuDirector.cpp

	BASE CLASS = JXTextMenuDirector

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXStyleMenuDirector.h"
#include "JXStyleMenu.h"
#include "JXStyleMenuTable.h"
#include "JXWindow.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXStyleMenuDirector::JXStyleMenuDirector
	(
	JXDirector*		supervisor,
	JXStyleMenu*	menu,
	JXTextMenuData*	menuData
	)
	:
	JXTextMenuDirector(supervisor, menu, menuData)
{
	itsMenu = menu;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXStyleMenuDirector::~JXStyleMenuDirector()
{
}

/******************************************************************************
 CreateMenuTable

 ******************************************************************************/

JXMenuTable*
JXStyleMenuDirector::CreateMenuTable()
{
	auto* table =
		jnew JXStyleMenuTable(itsMenu, GetMenuData(), GetWindow(),
							 JXWidget::kHElastic, JXWidget::kVElastic,
							 0,0, 10,10);
	assert( table != nullptr );
	return table;
}
