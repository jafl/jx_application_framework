/*********************************************************************************
 JPlotLinearFit.h
 
	Interface for the JPlotLinearFit class.
 
	Copyright @ 1997 by Glenn W. Bach.

 ********************************************************************************/
 
#ifndef _H_JPlotLinearFit
#define _H_JPlotLinearFit

#include <JPlotFitFunction.h>
#include <JArray.h>

class J2DPlotWidget;
class JString;

class JPlotLinearFit : public JPlotFitFunction
{
public:

	JPlotLinearFit(J2DPlotWidget* plot, JPlotDataBase* fitData, 
					const JFloat xMin, const JFloat xMax,
					const JBoolean xlog = kJFalse, const JBoolean ylog = kJFalse);
	JPlotLinearFit(J2DPlotWidget* plot, JPlotDataBase* fitData, 
					const JFloat xmin, const JFloat xmax,
					const JFloat ymin, const JFloat ymax,
					const JBoolean xlog = kJFalse, const JBoolean ylog = kJFalse);
	virtual ~JPlotLinearFit();	

//	virtual void	 GetElement(const JIndex index, J2DDataPoint* data) const;
	virtual JBoolean GetYRange(	JFloat* min, JFloat* max,
								JFloat  xMin, JFloat  xMax);

//	virtual	void UpdateFunction(const JFloat xmin, const JFloat xmax, 
//								const JSize steps);

	virtual JBoolean	GetParameterName(const JIndex index, JString* name) const;
	virtual JBoolean	GetParameter(const JIndex index, JFloat* value) const;

	virtual JBoolean	GetParameterError(const JIndex index, JFloat* value) const;

	virtual JBoolean	GetGoodnessOfFitName(JString* name) const;
	virtual JBoolean	GetGoodnessOfFit(JFloat* value) const;
	
	virtual JString		GetFunctionString() const;
	virtual JString		GetFitFunctionString() const;

	virtual JBoolean	GetYValue(const JFloat x, JFloat* y) const;
	virtual const JPlotDataBase*		GetDataToFit() const;

	void				AdjustDataRange(const JFloat xmin, const JFloat xmax,
										const JFloat ymin, const JFloat ymax);

protected:

	void				Paramin(JFloat ax, JFloat bx, JFloat cx, JFloat* xmin);
	virtual JFloat		ChiSqr(JFloat Bt);
	virtual JFloat		ChiSqrErr(JFloat Bt);
	JFloat				Root(JFloat xtemp);
	void				GenerateFit();
	void				SetFunctionName(const JCharacter* name);
	JFloat				GetCurrentXMax() const;
	JFloat				GetCurrentXMin() const;
	JFloat				GetCurrentStepCount() const;
	virtual JBoolean	DataElementValid(const JIndex index);
	virtual JBoolean	GetDataElement(const JIndex index, J2DDataPoint* point);

private:

	void				JPlotLinearFitX(J2DPlotWidget* plot, 
										JPlotDataBase* fitData, 
										JBoolean xlog, const JBoolean ylog);

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
	JBoolean 	itsUsingRange;
	JBoolean	itsYIsLog;
	JBoolean	itsXIsLog;
	JSize		itsRealCount;

};

/*********************************************************************************
 GetDataToFit
 

 ********************************************************************************/

inline const JPlotDataBase*
JPlotLinearFit::GetDataToFit()
	const
{
	return GetData();
}

/*********************************************************************************
 GetCurrentXMax
 

 ********************************************************************************/

inline JFloat
JPlotLinearFit::GetCurrentXMax()
	const
{
	return itsCurrentXMax;
}

/*********************************************************************************
 GetCurrentXMin
 

 ********************************************************************************/

inline JFloat
JPlotLinearFit::GetCurrentXMin()
	const
{
	return itsCurrentXMin;
}

/*********************************************************************************
 GetCurrentStepCount
 

 ********************************************************************************/

inline JFloat
JPlotLinearFit::GetCurrentStepCount()
	const
{
	return itsCurrentStepCount;
}


#endif
