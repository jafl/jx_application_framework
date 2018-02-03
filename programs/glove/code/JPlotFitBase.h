/*********************************************************************************
 JPlotFitBase.h
 
	Interface for the JPlotFitBase class.
 
	Copyright @ 2000 by Glenn W. Bach.

 ********************************************************************************/
 
#ifndef _H_JPlotFitBase
#define _H_JPlotFitBase

#include <JPlotFitFunction.h>
#include <JArray.h>
#include <JString.h>

class J2DPlotWidget;
class JVector;
class JMatrix;

class JPlotFitBase : public JPlotFitFunction
{
public:

	JPlotFitBase(J2DPlotWidget* plot, JPlotDataBase* fitData,
					const JFloat xMin, const JFloat xMax);
	JPlotFitBase(J2DPlotWidget* plot, JPlotDataBase* fitData, 
					const JFloat xmin, const JFloat xmax,
					const JFloat ymin, const JFloat ymax);
	virtual ~JPlotFitBase();	

	virtual JBoolean GetYRange(	JFloat* min, JFloat* max,
								JFloat  xMin, JFloat  xMax);

	virtual JBoolean	GetGoodnessOfFitName(JString* name) const;
	virtual JBoolean	GetGoodnessOfFit(JFloat* value) const;
	
	void				AdjustDataRange(const JFloat xmin, const JFloat xmax,
										const JFloat ymin, const JFloat ymax);

	virtual JString		GetFunctionString() const;
	virtual JString		GetFitFunctionString() const;

	void				GenerateFit(const JVector& parameters, const JFloat chi2);

protected:

	JFloat				Minimize(	JFloat ax, 
									JFloat bx, 
									JFloat cx,
									const JVector& p, 
									const JVector& xi,
									JFloat* xmin);
	JFloat				MinimizeN(	JVector* p, 
									JMatrix* xi, 
									JSize *iter);
	JFloat				LinearMinimization(	JVector* p, 
											JVector* xi);
	void				Bracket (	JFloat *ax, 
									JFloat *bx, 
									JFloat *cx, 
									JFloat *fa, 
									JFloat *fb, 
									JFloat *fc,
									const JVector& p, 
									const JVector& xi);
	void				Shift(	JFloat& a,
								JFloat& b,
								JFloat& c,
								JFloat& d);
	virtual JFloat		ChiSqrSqrt(const JVector& parameters);
	virtual JFloat		Function(JFloat Bt,
								 const JVector& p, 
								 const JVector& xi);
	virtual JFloat		ChiSqr(const JVector& p);

	virtual void 		GenerateDiffData();

	virtual JBoolean	DataElementValid(const JIndex index);
	virtual JBoolean	GetDataElement(const JIndex index, J2DDataPoint* point);

	J2DDataPoint		GetRealElement(const JIndex index);
	JSize				GetRealElementCount();

	virtual void		SetCurrentParameters(const JVector& p) = 0;
	virtual void		SetErrors(const JVector& p) = 0;
	virtual JFloat		FunctionN(const JFloat x) = 0;
	virtual JFloat		FunctionNPrimed(const JFloat x);
	
	void				SetFunctionString(const JCharacter* str);

private:

	void				JPlotFitBaseX(J2DPlotWidget* plot, 
										JPlotDataBase* fitData);

	void				AdjustDiffData();
	JFloat				CalcError(const JVector& parameters, const JIndex constIndex);

private:

	JFloat		itsChi2;
	JFloat 		itsRangeXMax;
	JFloat		itsRangeXMin;
	JFloat 		itsRangeYMax;
	JFloat 		itsRangeYMin;
	JBoolean 	itsUsingRange;
	JSize		itsRealCount;
	
	JArray<J2DDataPoint>* itsRealData;

	JIndex		itsCurrentConstantParmIndex;
	JFloat		itsCurrentConstantParm;
	JBoolean	itsUseAltFunction;
	JFloat		itsChiPlus;

	JString		itsFunctionStr;

};

#endif
