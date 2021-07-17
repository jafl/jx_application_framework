/*********************************************************************************
 GLPlotFitQuad2.h
 
	Interface for the GLPlotFitQuad2 class.
 
	Copyright @ 2000 by Glenn W. Bach.

 ********************************************************************************/
 
#ifndef _H_GLPlotFitQuad2
#define _H_GLPlotFitQuad2

#include <GLPlotFitBase.h>
#include <JArray.h>
#include <JString.h>

class J2DPlotWidget;
class JVector;
class JMatrix;

class GLPlotFitQuad2 : public GLPlotFitBase
{
public:

	GLPlotFitQuad2(J2DPlotWidget* plot, JPlotDataBase* fitData,
					const JFloat xMin, const JFloat xMax);
	GLPlotFitQuad2(J2DPlotWidget* plot, JPlotDataBase* fitData, 
					const JFloat xmin, const JFloat xmax,
					const JFloat ymin, const JFloat ymax);
	virtual ~GLPlotFitQuad2();	

	virtual bool	GetParameterName(const JIndex index, JString* name) const;
	virtual bool	GetParameter(const JIndex index, JFloat* value) const;

	virtual bool	GetParameterError(const JIndex index, JFloat* value) const;

	virtual JString		GetFunctionString() const;
	virtual JString		GetFitFunctionString() const;

	virtual bool	GetYValue(const JFloat x, JFloat* y) const;

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
