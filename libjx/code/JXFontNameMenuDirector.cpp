/******************************************************************************
 JXFontNameMenuDirector.cpp

	BASE CLASS = JXTextMenuDirector

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXFontNameMenuDirector.h"
#include "JXFontNameMenu.h"
#include "JXFontNameMenuTable.h"
#include "JXWindow.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXFontNameMenuDirector::JXFontNameMenuDirector
	(
	JXDirector*		supervisor,
	JXFontNameMenu*	menu,
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

JXFontNameMenuDirector::~JXFontNameMenuDirector()
{
}

/******************************************************************************
 CreateMenuTable

 ******************************************************************************/

JXMenuTable*
JXFontNameMenuDirector::CreateMenuTable()
{
	return jnew JXFontNameMenuTable(itsMenu, GetMenuData(), GetWindow(),
									JXWidget::kHElastic, JXWidget::kVElastic,
									0,0, 10,10);
}
