/*********************************************************************************
 JPlotFitPowerLaw.h
 
	Interface for the JPlotFitPowerLaw class.
 
	Copyright @ 2000 by Glenn W. Bach.

 ********************************************************************************/
 
#ifndef _H_JPlotFitPowerLaw
#define _H_JPlotFitPowerLaw

#include <JPlotFitBase.h>
#include <JString.h>

class J2DPlotWidget;

class JPlotFitPowerLaw : public JPlotFitBase
{
public:

	JPlotFitPowerLaw(J2DPlotWidget* plot, JPlotDataBase* fitData,
					const JFloat xMin, const JFloat xMax);
	JPlotFitPowerLaw(J2DPlotWidget* plot, JPlotDataBase* fitData, 
					const JFloat xmin, const JFloat xmax,
					const JFloat ymin, const JFloat ymax);
	virtual ~JPlotFitPowerLaw();	

	void				GenerateFit();

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
