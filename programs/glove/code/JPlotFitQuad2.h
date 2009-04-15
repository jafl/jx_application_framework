/*********************************************************************************
 JPlotFitQuad2.h
 
	Interface for the JPlotFitQuad2 class.
 
	Copyright @ 2000 by Glenn W. Bach. All rights reserved.

 ********************************************************************************/
 
#ifndef _H_JPlotFitQuad2
#define _H_JPlotFitQuad2

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JPlotFitBase.h>
#include <JArray.h>
#include <JString.h>

class J2DPlotWidget;
class JVector;
class JMatrix;

class JPlotFitQuad2 : public JPlotFitBase
{
public:

	JPlotFitQuad2(J2DPlotWidget* plot, JPlotDataBase* fitData,
					const JFloat xMin, const JFloat xMax);
	JPlotFitQuad2(J2DPlotWidget* plot, JPlotDataBase* fitData, 
					const JFloat xmin, const JFloat xmax,
					const JFloat ymin, const JFloat ymax);
	virtual ~JPlotFitQuad2();	

	virtual JBoolean	GetParameterName(const JIndex index, JString* name) const;
	virtual JBoolean	GetParameter(const JIndex index, JFloat* value) const;

	virtual JBoolean	GetParameterError(const JIndex index, JFloat* value) const;

	virtual JString		GetFunctionString() const;
	virtual JString		GetFitFunctionString() const;

	virtual JBoolean	GetYValue(const JFloat x, JFloat* y) const;

protected:

	virtual void		SetCurrentParameters(const JVector& p);
	virtual void		SetErrors(const JVector& p);
	virtual JFloat		FunctionN(const JFloat x);
	virtual JFloat		FunctionNPrimed(const JFloat x);


private:

	void				JPlotFitQuad2X(J2DPlotWidget* plot, 
										JPlotDataBase* fitData);

	void				CalculateFirstPass();

private:

	JString		itsFunctionName;
	JFloat		itsAParameter;
	JFloat		itsAErrParameter;
	JFloat		itsBParameter;
	JFloat		itsBErrParameter;
	JFloat		itsCParameter;
	JFloat		itsCErrParameter;
	JFloat		itsChi2Start;
	
};

#endif
