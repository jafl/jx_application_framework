/******************************************************************************
 JXTextMenuDirector.cpp

	BASE CLASS = JXMenuDirector

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXTextMenuDirector.h"
#include "JXWindow.h"
#include "JXTextMenuTable.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXTextMenuDirector::JXTextMenuDirector
	(
	JXDirector*		supervisor,
	JXMenu*			menu,
	JXTextMenuData*	menuData
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

JXTextMenuDirector::~JXTextMenuDirector()
{
}

/******************************************************************************
 CreateMenuTable

 ******************************************************************************/

JXMenuTable*
JXTextMenuDirector::CreateMenuTable()
{
	JXTextMenuTable* table =
		jnew JXTextMenuTable(itsMenu, itsMenuData, GetWindow(),
							 JXWidget::kHElastic, JXWidget::kVElastic,
							 0,0, 10,10);
	assert( table != nullptr );
	return table;
}
