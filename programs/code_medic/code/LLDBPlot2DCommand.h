/******************************************************************************
 LLDBPlot2DCommand.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBPlot2DCommand
#define _H_LLDBPlot2DCommand

#include "CMPlot2DCommand.h"

class LLDBPlot2DCommand : public CMPlot2DCommand
{
public:

	LLDBPlot2DCommand(CMPlot2DDir* dir, JArray<JFloat>* x, JArray<JFloat>* y);

	virtual	~LLDBPlot2DCommand();

	virtual void	UpdateRange(const JIndex curveIndex,
								const JInteger min,const JInteger max) override;

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	JIndex	itsCurveIndex;

private:

	// not allowed

	LLDBPlot2DCommand(const LLDBPlot2DCommand& source);
	const LLDBPlot2DCommand& operator=(const LLDBPlot2DCommand& source);
};

#endif
