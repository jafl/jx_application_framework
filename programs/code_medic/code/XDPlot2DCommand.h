/******************************************************************************
 XDPlot2DCommand.h

	Copyright � 2007 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_XDPlot2DCommand
#define _H_XDPlot2DCommand

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMPlot2DCommand.h"

class XDPlot2DCommand : public CMPlot2DCommand
{
public:

	XDPlot2DCommand(CMPlot2DDir* dir, JArray<JFloat>* x, JArray<JFloat>* y);

	virtual	~XDPlot2DCommand();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	// not allowed

	XDPlot2DCommand(const XDPlot2DCommand& source);
	const XDPlot2DCommand& operator=(const XDPlot2DCommand& source);
};

#endif
