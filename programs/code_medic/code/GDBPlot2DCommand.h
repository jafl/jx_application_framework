/******************************************************************************
 GDBPlot2DCommand.h

	Copyright © 2009 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_GDBPlot2DCommand
#define _H_GDBPlot2DCommand

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMPlot2DCommand.h"

class GDBPlot2DCommand : public CMPlot2DCommand
{
public:

	GDBPlot2DCommand(CMPlot2DDir* dir, JArray<JFloat>* x, JArray<JFloat>* y);

	virtual	~GDBPlot2DCommand();

	virtual void	UpdateRange(const JIndex curveIndex,
								const JInteger min,const JInteger max);

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	// not allowed

	GDBPlot2DCommand(const GDBPlot2DCommand& source);
	const GDBPlot2DCommand& operator=(const GDBPlot2DCommand& source);
};

#endif
