/******************************************************************************
 JXStyleMenuDirector.cpp

	BASE CLASS = JXTextMenuDirector

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXStyleMenuDirector.h>
#include <JXStyleMenu.h>
#include <JXStyleMenuTable.h>
#include <JXWindow.h>
#include <jAssert.h>

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
	itsStyleMenu = menu;
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
	JXStyleMenuTable* table =
		new JXStyleMenuTable(itsStyleMenu, GetMenuData(), GetWindow(),
							 JXWidget::kHElastic, JXWidget::kVElastic,
							 0,0, 10,10);
	assert( table != NULL );
	return table;
}
