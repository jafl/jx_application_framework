/******************************************************************************
 WidgetPanelBase.cpp

	BASE CLASS = WidgetPanelBase

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "WidgetPanelBase.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Destructor

 ******************************************************************************/

WidgetPanelBase::~WidgetPanelBase()
{
}

/******************************************************************************
 Validate (virtual)

 ******************************************************************************/

bool
WidgetPanelBase::Validate()
	const
{
	return true;
}