/*********************************************************************************
 JPlotFitExp.h
 
	Interface for the JPlotFitExp class.
 
	Copyright @ 2000 by Glenn W. Bach. All rights reserved.

 ********************************************************************************/
 
#ifndef _H_JPlotFitExp
#define _H_JPlotFitExp

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JPlotFitBase.h>
#include <JString.h>

class J2DPlotWidget;

class JPlotFitExp : public JPlotFitBase
{
public:

	JPlotFitExp(J2DPlotWidget* plot, JPlotDataBase* fitData,
					const JFloat xMin, const JFloat xMax);
	JPlotFitExp(J2DPlotWidget* plot, JPlotDataBase* fitData, 
					const JFloat xmin, const JFloat xmax,
					const JFloat ymin, const JFloat ymax);
	virtual ~JPlotFitExp();	

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

	void	JPlotFitExpX(J2DPlotWidget* plot, JPlotDataBase* fitData);
};

#endif
