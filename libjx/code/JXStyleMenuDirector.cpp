/******************************************************************************
 JXStyleMenuDirector.cpp

	BASE CLASS = JXTextMenuDirector

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "jx-af/jx/JXStyleMenuDirector.h"
#include "jx-af/jx/JXStyleMenu.h"
#include "jx-af/jx/JXStyleMenuTable.h"
#include "jx-af/jx/JXWindow.h"
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
