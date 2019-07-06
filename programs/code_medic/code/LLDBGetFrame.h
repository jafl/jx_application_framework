/******************************************************************************
 LLDBGetFrame.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBGetFrame
#define _H_LLDBGetFrame

#include "CMGetFrame.h"

class CMStackWidget;

class LLDBGetFrame : public CMGetFrame
{
public:

	LLDBGetFrame(CMStackWidget* widget);

	virtual	~LLDBGetFrame();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	CMStackWidget*	itsWidget;

private:

	// not allowed

	LLDBGetFrame(const LLDBGetFrame& source);
	const LLDBGetFrame& operator=(const LLDBGetFrame& source);
};

#endif
