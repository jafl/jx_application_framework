/******************************************************************************
 LLDBGetFrame.h

	Copyright © 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_LLDBGetFrame
#define _H_LLDBGetFrame

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMGetFrame.h"

class CMStackWidget;

class LLDBGetFrame : public CMGetFrame
{
public:

	LLDBGetFrame(CMStackWidget* widget);

	virtual	~LLDBGetFrame();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	CMStackWidget*	itsWidget;

private:

	// not allowed

	LLDBGetFrame(const LLDBGetFrame& source);
	const LLDBGetFrame& operator=(const LLDBGetFrame& source);
};

#endif
