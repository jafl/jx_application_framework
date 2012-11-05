/******************************************************************************
 XDGetFrame.h

	Copyright © 2007 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_XDGetFrame
#define _H_XDGetFrame

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMGetFrame.h"

class CMStackWidget;

class XDGetFrame : public CMGetFrame
{
public:

	XDGetFrame(CMStackWidget* widget);

	virtual	~XDGetFrame();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	CMStackWidget*	itsWidget;

private:

	// not allowed

	XDGetFrame(const XDGetFrame& source);
	const XDGetFrame& operator=(const XDGetFrame& source);
};

#endif
