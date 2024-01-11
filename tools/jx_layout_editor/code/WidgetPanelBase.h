/******************************************************************************
 WidgetPanelBase.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_WidgetPanelBase
#define _H_WidgetPanelBase

class WidgetPanelBase
{
public:

	virtual ~WidgetPanelBase();

	virtual bool	Validate() const;
};

#endif
