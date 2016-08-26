/******************************************************************************
 JVMGetFrame.h

	Copyright � 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JVMGetFrame
#define _H_JVMGetFrame

#include "CMGetFrame.h"

class CMStackWidget;

class JVMGetFrame : public CMGetFrame
{
public:

	JVMGetFrame(CMStackWidget* widget);

	virtual	~JVMGetFrame();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	CMStackWidget*	itsWidget;

private:

	// not allowed

	JVMGetFrame(const JVMGetFrame& source);
	const JVMGetFrame& operator=(const JVMGetFrame& source);
};

#endif
