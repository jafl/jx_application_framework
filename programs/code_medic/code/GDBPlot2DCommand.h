/******************************************************************************
 GDBPlot2DCommand.h

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBPlot2DCommand
#define _H_GDBPlot2DCommand

#include "CMPlot2DCommand.h"

class GDBPlot2DCommand : public CMPlot2DCommand
{
public:

	GDBPlot2DCommand(CMPlot2DDir* dir, JArray<JFloat>* x, JArray<JFloat>* y);

	virtual	~GDBPlot2DCommand();

	virtual void	UpdateRange(const JIndex curveIndex,
								const JInteger min,const JInteger max) override;

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	// not allowed

	GDBPlot2DCommand(const GDBPlot2DCommand& source);
	const GDBPlot2DCommand& operator=(const GDBPlot2DCommand& source);
};

#endif
