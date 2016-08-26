/*********************************************************************************
 JPlotFitLinearEq.h
 
	Interface for the JPlotFitLinearEq class.
 
	Copyright @ 2000 by Glenn W. Bach. All rights reserved.

 ********************************************************************************/
 
#ifndef _H_JPlotFitLinearEq
#define _H_JPlotFitLinearEq

#include <JPlotFitBase.h>
#include <JArray.h>
#include <JString.h>

class J2DPlotWidget;
class JVector;
class JMatrix;

class JPlotFitLinearEq : public JPlotFitBase
{
public:

	JPlotFitLinearEq(J2DPlotWidget* plot, JPlotDataBase* fitData,
					const JFloat xMin, const JFloat xMax);
	JPlotFitLinearEq(J2DPlotWidget* plot, JPlotDataBase* fitData, 
					const JFloat xmin, const JFloat xmax,
					const JFloat ymin, const JFloat ymax);
	virtual ~JPlotFitLinearEq();

	void	InitializePolynomial(const JArray<JIndex>& powers);
	void	GenerateFit();

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
