/*********************************************************************************
 GLPlotFitBase.h
 
	Interface for the GLPlotFitBase class.
 
	Copyright @ 2000 by Glenn W. Bach.

 ********************************************************************************/
 
#ifndef _H_GLPlotFitBase
#define _H_GLPlotFitBase

#include <GLPlotFitFunction.h>
#include <JArray.h>
#include <JString.h>

class J2DPlotWidget;
class JVector;
class JMatrix;

class GLPlotFitBase : public GLPlotFitFunction
{
public:

	GLPlotFitBase(J2DPlotWidget* plot, JPlotDataBase* fitData,
					const JFloat xMin, const JFloat xMax);
	GLPlotFitBase(J2DPlotWidget* plot, JPlotDataBase* fitData, 
					const JFloat xmin, const JFloat xmax,
					const JFloat ymin, const JFloat ymax);
	virtual ~GLPlotFitBase();	

	virtual JBoolean	GetGoodnessOfFitName(JString* name) const override;
	virtual JBoolean	GetGoodnessOfFit(JFloat* value) const override;
	
	void				AdjustDataRange(const JFloat xmin, const JFloat xmax,
										const JFloat ymin, const JFloat ymax);

	virtual JString		GetFunctionString() const override;
	virtual JString		GetFitFunctionString() const override;

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

	virtual void 		GenerateDiffData() override;

	virtual JBoolean	DataElementValid(const JIndex index) override;
	virtual JBoolean	GetDataElement(const JIndex index, J2DDataPoint* point) override;

	J2DDataPoint		GetRealElement(const JIndex index);
	JSize				GetRealElementCount();

	virtual void		SetCurrentParameters(const JVector& p) = 0;
	virtual void		SetErrors(const JVector& p) = 0;
	virtual JFloat		FunctionN(const JFloat x) = 0;
	virtual JFloat		FunctionNPrimed(const JFloat x);
	
	void				SetFunctionString(const JString& str);

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
