/******************************************************************************
 JXImageMenuDirector.cpp

	BASE CLASS = JXMenuDirector

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXImageMenuDirector.h"
#include "JXWindow.h"
#include "JXImageMenuTable.h"
#include <jx-af/jcore/jAssert.h>

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
	return jnew JXImageMenuTable(itsMenu, itsMenuData, GetWindow(),
								 JXWidget::kHElastic, JXWidget::kVElastic,
								 0,0, 10,10);
}
