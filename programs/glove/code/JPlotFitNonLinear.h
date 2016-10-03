/******************************************************************************
 JPlotFitNonLinear.h

	Interface for the JPlotFitNonLinear class

	Copyright (C) 2000 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#ifndef _H_JPlotFitNonLinear
#define _H_JPlotFitNonLinear

// Superclass Header
#include <JPlotFitBase.h>
#include <JString.h>

class GVarList;
class JFunction;
class JVector;

class JPlotFitNonLinear : public JPlotFitBase
{
public:

	JPlotFitNonLinear(J2DPlotWidget* plot, JPlotDataBase* fitData,
					  const JFloat xMin, const JFloat xMax);
	JPlotFitNonLinear(J2DPlotWidget* plot, JPlotDataBase* fitData, 
					  const JFloat xmin, const JFloat xmax,
					  const JFloat ymin, const JFloat ymax);
	virtual ~JPlotFitNonLinear();

	void	SetVarList(GVarList* varList);
	void	SetFunction(const JCharacter* function);
	void	SetFPrimed(const JCharacter* fPrimed);
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

	GVarList*		itsVarList;			// we don't own this
	JFunction*		itsFunction;
	JFunction*		itsFPrimed;
	JVector*		itsErrors;

private:

	// not allowed

	JPlotFitNonLinear(const JPlotFitNonLinear& source);
	const JPlotFitNonLinear& operator=(const JPlotFitNonLinear& source);

};
#endif
