/******************************************************************************
 JVMPlot2DCommand.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMPlot2DCommand
#define _H_JVMPlot2DCommand

#include "CMPlot2DCommand.h"

class JVMPlot2DCommand : public CMPlot2DCommand
{
public:

	JVMPlot2DCommand(CMPlot2DDir* dir, JArray<JFloat>* x, JArray<JFloat>* y);

	virtual	~JVMPlot2DCommand();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	// not allowed

	JVMPlot2DCommand(const JVMPlot2DCommand& source);
	const JVMPlot2DCommand& operator=(const JVMPlot2DCommand& source);
};

#endif
