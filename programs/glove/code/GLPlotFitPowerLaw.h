/*********************************************************************************
 GLPlotFitPowerLaw.h
 
	Interface for the GLPlotFitPowerLaw class.
 
	Copyright @ 2000 by Glenn W. Bach.

 ********************************************************************************/
 
#ifndef _H_GLPlotFitPowerLaw
#define _H_GLPlotFitPowerLaw

#include <GLPlotFitBase.h>
#include <JString.h>

class J2DPlotWidget;

class GLPlotFitPowerLaw : public GLPlotFitBase
{
public:

	GLPlotFitPowerLaw(J2DPlotWidget* plot, JPlotDataBase* fitData,
					const JFloat xMin, const JFloat xMax);
	GLPlotFitPowerLaw(J2DPlotWidget* plot, JPlotDataBase* fitData, 
					const JFloat xmin, const JFloat xmax,
					const JFloat ymin, const JFloat ymax);
	virtual ~GLPlotFitPowerLaw();	

	void				GenerateFit();

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

	JFloat		itsAParm;
	JFloat		itsBParm;
	JFloat		itsAErr;
	JFloat		itsBErr;

	JFloat		itsChi2Start;

private:

	void	CalculateFirstPass();

	void	JPlotFitPowerLawX(J2DPlotWidget* plot, JPlotDataBase* fitData);
};

#endif
