/*********************************************************************************
 GLPlotFitLinearEq.h
 
	Interface for the GLPlotFitLinearEq class.
 
	Copyright @ 2000 by Glenn W. Bach.

 ********************************************************************************/
 
#ifndef _H_GLPlotFitLinearEq
#define _H_GLPlotFitLinearEq

#include <GLPlotFitBase.h>
#include <JArray.h>
#include <JString.h>

class J2DPlotWidget;
class JVector;
class JMatrix;

class GLPlotFitLinearEq : public GLPlotFitBase
{
public:

	GLPlotFitLinearEq(J2DPlotWidget* plot, JPlotDataBase* fitData,
					const JFloat xMin, const JFloat xMax);
	GLPlotFitLinearEq(J2DPlotWidget* plot, JPlotDataBase* fitData, 
					const JFloat xmin, const JFloat xmax,
					const JFloat ymin, const JFloat ymax);
	virtual ~GLPlotFitLinearEq();

	void	InitializePolynomial(const JArray<JIndex>& powers);
	void	GenerateFit();

	virtual bool	GetParameterName(const JIndex index, JString* name) const;
	virtual bool	GetParameter(const JIndex index, JFloat* value) const;

	virtual bool	GetParameterError(const JIndex index, JFloat* value) const;

	virtual bool	GetYValue(const JFloat x, JFloat* y) const;

protected:

	virtual void		SetCurrentParameters(const JVector& p);
	virtual void		SetErrors(const JVector& p);
	virtual JFloat		FunctionN(const JFloat x);
	virtual JFloat		FunctionNPrimed(const JFloat x);


private:

	void				JPlotFitLinearEqX(J2DPlotWidget* plot, 
										  JPlotDataBase* fitData);

	void				CalculateFirstPass();

private:

	JArray<JIndex>*	itsPowers;
	JVector*		itsParameters;
	JVector*		itsErrors;
	
	JFloat		itsChi2Start;
	
};

#endif
