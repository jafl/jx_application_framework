/******************************************************************************
 CMPlot2DCommand.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_CMPlot2DCommand
#define _H_CMPlot2DCommand

#include "CMCommand.h"
#include <JArray.h>

class CMPlot2DDir;

class CMPlot2DCommand : public CMCommand
{
public:

	CMPlot2DCommand(CMPlot2DDir* dir, JArray<JFloat>* x, JArray<JFloat>* y);

	virtual	~CMPlot2DCommand();

	virtual void	UpdateRange(const JIndex curveIndex,
								const JInteger min,const JInteger max);

	CMPlot2DDir*	GetDirector();
	JArray<JFloat>*	GetX();
	JArray<JFloat>*	GetY();

private:

	CMPlot2DDir*	itsDirector;
	JArray<JFloat>*	itsX;
	JArray<JFloat>*	itsY;

private:

	// not allowed

	CMPlot2DCommand(const CMPlot2DCommand& source);
	const CMPlot2DCommand& operator=(const CMPlot2DCommand& source);
};


/******************************************************************************
 GetDirector

 ******************************************************************************/

inline CMPlot2DDir*
CMPlot2DCommand::GetDirector()
{
	return itsDirector;
}

/******************************************************************************
 GetX

 ******************************************************************************/

inline JArray<JFloat>*
CMPlot2DCommand::GetX()
{
	return itsX;
}

/******************************************************************************
 GetY

 ******************************************************************************/

inline JArray<JFloat>*
CMPlot2DCommand::GetY()
{
	return itsY;
}

#endif
