/******************************************************************************
 GDBGetFrame.h

	Copyright (C) 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_GDBGetFrame
#define _H_GDBGetFrame

#include "CMGetFrame.h"

class CMStackWidget;

class GDBGetFrame : public CMGetFrame
{
public:

	GDBGetFrame(CMStackWidget* widget);

	virtual	~GDBGetFrame();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	CMStackWidget*	itsWidget;

private:

	// not allowed

	GDBGetFrame(const GDBGetFrame& source);
	const GDBGetFrame& operator=(const GDBGetFrame& source);
};

#endif
