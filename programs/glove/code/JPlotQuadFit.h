/*********************************************************************************
 JPlotQuadFit.h
 
	Interface for the JPlotQuadFit class.
 
	Copyright @ 1997 by Glenn W. Bach.

 ********************************************************************************/
 
#ifndef _H_JPlotQuadFit
#define _H_JPlotQuadFit

#include <JPlotFitFunction.h>
#include <JArray.h>
#include <JString.h>

class J2DPlotWidget;
class JVector;
class JMatrix;

class JPlotQuadFit : public JPlotFitFunction
{
public:

	JPlotQuadFit(J2DPlotWidget* plot, JPlotDataBase* fitData,
					const JFloat xMin, const JFloat xMax);
	JPlotQuadFit(J2DPlotWidget* plot, JPlotDataBase* fitData, 
					const JFloat xmin, const JFloat xmax,
					const JFloat ymin, const JFloat ymax);
	virtual ~JPlotQuadFit();	

	virtual JBoolean	GetYRange(const JFloat xMin, const JFloat xMax,
								  const JBoolean xLinear,
								  JFloat* yMin, JFloat* yMax) const override;

	virtual JBoolean	GetParameterName(const JIndex index, JString* name) const override;
	virtual JBoolean	GetParameter(const JIndex index, JFloat* value) const override;

	virtual JBoolean	GetParameterError(const JIndex index, JFloat* value) const override;

	virtual JBoolean	GetGoodnessOfFitName(JString* name) const override;
	virtual JBoolean	GetGoodnessOfFit(JFloat* value) const override;
	
	virtual JString		GetFunctionString() const override;
	virtual JString		GetFitFunctionString() const override;

	virtual JBoolean	GetYValue(const JFloat x, JFloat* y) const override;
	virtual const JPlotDataBase*		GetDataToFit() const;

	void				AdjustDataRange(const JFloat xmin, const JFloat xmax,
										const JFloat ymin, const JFloat ymax);

protected:

	JFloat				BracketAndMinimize(	JFloat* parameter, 
											const JFloat chitemp, 
											const JIndex type);
	JFloat				Minimize(	JFloat ax, 
									JFloat bx, 
									JFloat cx, 
									JFloat* xmin, 
									const JIndex type);
	JFloat				MinimizeN(	JVector& p, 
									JMatrix& xi, 
									JSize *iter, 
									const JIndex type);
	JFloat				LinearMinimization(	JVector& p, 
											JVector& xi,
											const JIndex type);
	void				Bracket (	JFloat *ax, 
									JFloat *bx, 
									JFloat *cx, 
									JFloat *fa, 
									JFloat *fb, 
									JFloat *fc,
									const JIndex type);
	void				Shift(	JFloat& a,
								JFloat& b,
								JFloat& c,
								JFloat& d);
	virtual JFloat		FunctionN(	JVector& parameters, 
									const JIndex type);
	virtual JFloat		Function(	JFloat Bt, 
									const JIndex type);
	virtual JFloat		ChiSqr(	JFloat Bt, 
								const JIndex type);

	JFloat				MinimizeChiSqr(	const JFloat chi2, 
										const JIndex type);
	
	void				GenerateFit();
	void				SetFunctionName(const JString& name);
	JFloat				GetCurrentXMax() const;
	JFloat				GetCurrentXMin() const;
	JFloat				GetCurrentStepCount() const;
	virtual JBoolean	DataElementValid(const JIndex index) override;
	virtual JBoolean	GetDataElement(const JIndex index, J2DDataPoint* point) override;

private:

	void				JPlotQuadFitX(J2DPlotWidget* plot, 
										JPlotDataBase* fitData);

	void				QuadFirstPass();
	void				QuadMinFit();
	
	void				AdjustDiffData();
	JFloat				CalcError(const JIndex type);

private:

	JString		itsFunctionName;
	JFloat		itsCurrentXMin;
	JFloat		itsCurrentXMax;
	JSize		itsCurrentStepCount;
	JFloat		itsAParameter;
	JFloat		itsAErrParameter;
	JFloat		itsBParameter;
	JFloat		itsBErrParameter;
	JFloat		itsCParameter;
	JFloat		itsCErrParameter;
	JFloat		itsChi2;
	JFloat		itsAParameterT;
	JFloat		itsBParameterT;
	JFloat		itsCParameterT;
	JFloat		itsChi2T;
	JFloat 		itsRangeXMax;
	JFloat		itsRangeXMin;
	JFloat 		itsRangeYMax;
	JFloat 		itsRangeYMin;
	JBoolean 	itsUsingRange;
	JSize		itsRealCount;
	
	JArray<J2DDataPoint>* itsRealData;

	JVector*	itsP;
	JVector*	itsXi;
	JIndex		itsCurrentType;

};

/*********************************************************************************
 GetDataToFit
 

 ********************************************************************************/

inline const JPlotDataBase*
JPlotQuadFit::GetDataToFit()
	const
{
	return GetData();
}

/*********************************************************************************
 GetCurrentXMax
 

 ********************************************************************************/

inline JFloat
JPlotQuadFit::GetCurrentXMax()
	const
{
	return itsCurrentXMax;
}

/*********************************************************************************
 GetCurrentXMin
 

 ********************************************************************************/

inline JFloat
JPlotQuadFit::GetCurrentXMin()
	const
{
	return itsCurrentXMin;
}

/*********************************************************************************
 GetCurrentStepCount
 

 ********************************************************************************/

inline JFloat
JPlotQuadFit::GetCurrentStepCount()
	const
{
	return itsCurrentStepCount;
}


#endif
