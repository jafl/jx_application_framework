/*********************************************************************************
 GLPlotLinearFit.h
 
	Interface for the GLPlotLinearFit class.
 
	Copyright @ 1997 by Glenn W. Bach.

 ********************************************************************************/
 
#ifndef _H_GLPlotLinearFit
#define _H_GLPlotLinearFit

#include <GLPlotFitFunction.h>
#include <JArray.h>
#include <JString.h>

class J2DPlotWidget;

class GLPlotLinearFit : public GLPlotFitFunction
{
public:

	GLPlotLinearFit(J2DPlotWidget* plot, JPlotDataBase* fitData, 
					const JFloat xMin, const JFloat xMax,
					const bool xlog = false, const bool ylog = false);
	GLPlotLinearFit(J2DPlotWidget* plot, JPlotDataBase* fitData, 
					const JFloat xmin, const JFloat xmax,
					const JFloat ymin, const JFloat ymax,
					const bool xlog = false, const bool ylog = false);
	virtual ~GLPlotLinearFit();	

//	virtual void	 GetElement(const JIndex index, J2DDataPoint* data) const;
	virtual bool	GetYRange(const JFloat xMin, const JFloat xMax,
								  const bool xLinear,
								  JFloat* yMin, JFloat* yMax) const override;

//	virtual	void UpdateFunction(const JFloat xmin, const JFloat xmax, 
//								const JSize steps);

	virtual bool	GetParameterName(const JIndex index, JString* name) const override;
	virtual bool	GetParameter(const JIndex index, JFloat* value) const override;

	virtual bool	GetParameterError(const JIndex index, JFloat* value) const override;

	virtual bool	GetGoodnessOfFitName(JString* name) const override;
	virtual bool	GetGoodnessOfFit(JFloat* value) const override;
	
	virtual JString		GetFunctionString() const override;
	virtual JString		GetFitFunctionString() const override;

	virtual bool	GetYValue(const JFloat x, JFloat* y) const override;
	virtual const JPlotDataBase*		GetDataToFit() const;

	void				AdjustDataRange(const JFloat xmin, const JFloat xmax,
										const JFloat ymin, const JFloat ymax);

protected:

	void				Paramin(JFloat ax, JFloat bx, JFloat cx, JFloat* xmin);
	virtual JFloat		ChiSqr(JFloat Bt);
	virtual JFloat		ChiSqrErr(JFloat Bt);
	JFloat				Root(JFloat xtemp);
	void				GenerateFit();
	void				SetFunctionName(const JString& name);
	JFloat				GetCurrentXMax() const;
	JFloat				GetCurrentXMin() const;
	JFloat				GetCurrentStepCount() const;
	virtual bool	DataElementValid(const JIndex index) override;
	virtual bool	GetDataElement(const JIndex index, J2DDataPoint* point) override;

private:

	void				JPlotLinearFitX(J2DPlotWidget* plot, 
										JPlotDataBase* fitData, 
										bool xlog, const bool ylog);

	void				LinearLSQ1();
	void				LinearLSQ2();
	void				Variance(JFloat* vx, JFloat* vy);
	
	void				AdjustDiffData();

private:

	JString		itsFunctionName;
	JFloat		itsCurrentXMin;
	JFloat		itsCurrentXMax;
	JSize		itsCurrentStepCount;
	JFloat		itsAParameter;
	JFloat		itsAErrParameter;
	JFloat		itsBParameter;
	JFloat		itsBErrParameter;
	JFloat		itsChi2;
	JFloat		itsAParameterT;
	JFloat		itsBParameterT;
	JFloat		itsChi2T;
	JFloat 		itsRangeXMax;
	JFloat		itsRangeXMin;
	JFloat 		itsRangeYMax;
	JFloat 		itsRangeYMin;
	bool 	itsUsingRange;
	bool	itsYIsLog;
	bool	itsXIsLog;
	JSize		itsRealCount;

};

/*********************************************************************************
 GetDataToFit
 

 ********************************************************************************/

inline const JPlotDataBase*
GLPlotLinearFit::GetDataToFit()
	const
{
	return GetData();
}

/*********************************************************************************
 GetCurrentXMax
 

 ********************************************************************************/

inline JFloat
GLPlotLinearFit::GetCurrentXMax()
	const
{
	return itsCurrentXMax;
}

/*********************************************************************************
 GetCurrentXMin
 

 ********************************************************************************/

inline JFloat
GLPlotLinearFit::GetCurrentXMin()
	const
{
	return itsCurrentXMin;
}

/*********************************************************************************
 GetCurrentStepCount
 

 ********************************************************************************/

inline JFloat
GLPlotLinearFit::GetCurrentStepCount()
	const
{
	return itsCurrentStepCount;
}


#endif
