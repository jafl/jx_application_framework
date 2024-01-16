/******************************************************************************
 WidgetPanelBase.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_WidgetPanelBase
#define _H_WidgetPanelBase

#include <jx-af/jcore/jTypes.h>

class WidgetParametersDialog;

class WidgetPanelBase
{
public:

	virtual ~WidgetPanelBase();

	virtual bool	Validate() const;
};

#endif
