/******************************************************************************
 JXImageMenuDirector.cpp

	BASE CLASS = JXMenuDirector

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXImageMenuDirector.h>
#include <JXWindow.h>
#include <JXImageMenuTable.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXImageMenuDirector::JXImageMenuDirector
	(
	JXDirector*			supervisor,
	JXMenu*				menu,
	JXImageMenuData*	menuData
	)
	:
	JXMenuDirector(supervisor, menu)
{
	itsMenu     = menu;
	itsMenuData = menuData;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXImageMenuDirector::~JXImageMenuDirector()
{
}

/******************************************************************************
 CreateMenuTable

 ******************************************************************************/

JXMenuTable*
JXImageMenuDirector::CreateMenuTable()
{
	JXImageMenuTable* table =
		new JXImageMenuTable(itsMenu, itsMenuData, GetWindow(),
							 JXWidget::kHElastic, JXWidget::kVElastic,
							 0,0, 10,10);
	assert( table != NULL );
	return table;
}
