/******************************************************************************
 JXTextMenuDirector.cpp

	BASE CLASS = JXMenuDirector

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "jx-af/jx/JXTextMenuDirector.h"
#include "jx-af/jx/JXWindow.h"
#include "jx-af/jx/JXTextMenuTable.h"
#include <jx-af/jcore/jAssert.h>

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
	auto* table =
		jnew JXTextMenuTable(itsMenu, itsMenuData, GetWindow(),
							 JXWidget::kHElastic, JXWidget::kVElastic,
							 0,0, 10,10);
	assert( table != nullptr );
	return table;
}
