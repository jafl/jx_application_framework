/******************************************************************************
 JPlotFitModule.h

	Interface for the JPlotFitModule class

	Copyright (C) 2000 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#ifndef _H_JPlotFitModule
#define _H_JPlotFitModule

// Superclass Header
#include <JPlotFitBase.h>
#include <JString.h>

class GLDLFitModule;
class JVector;

class JPlotFitModule : public JPlotFitBase
{
public:

	JPlotFitModule(J2DPlotWidget* plot, JPlotDataBase* fitData,
					  const JFloat xMin, const JFloat xMax);
	JPlotFitModule(J2DPlotWidget* plot, JPlotDataBase* fitData, 
					  const JFloat xmin, const JFloat xmax,
					  const JFloat ymin, const JFloat ymax);
	virtual ~JPlotFitModule();

	void	SetFitModule(GLDLFitModule* fit);
	void	SetInitialParameters(const JVector& p);

	virtual JBoolean	GetParameterName(const JIndex index, JString* name) const;
	virtual JBoolean	GetParameter(const JIndex index, JFloat* value) const;

	virtual JBoolean	GetParameterError(const JIndex index, JFloat* value) const;

	virtual JBoolean	GetYValue(const JFloat x, JFloat* y) const;

protected:

	virtual void		SetCurrentParameters(const JVector& p);
	virtual void		SetErrors(const JVector& p);
	virtual JFloat		FunctionN(const JFloat x);
	virtual JFloat		FunctionNPrimed(const JFloat x);

private:

	GLDLFitModule*	itsModule;			// we don't own this
	JVector*		itsErrors;
	JVector*		itsParameters;

private:

	// not allowed

	JPlotFitModule(const JPlotFitModule& source);
	const JPlotFitModule& operator=(const JPlotFitModule& source);

};
#endif
