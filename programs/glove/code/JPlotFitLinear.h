/*********************************************************************************
 JPlotFitLinear.h
 
	Interface for the JPlotFitLinear class.
 
	Copyright @ 2000 by Glenn W. Bach. All rights reserved.

 ********************************************************************************/
 
#ifndef _H_JPlotFitLinear
#define _H_JPlotFitLinear

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JPlotFitLinearEq.h>

class J2DPlotWidget;

class JPlotFitLinear : public JPlotFitLinearEq
{
public:

	JPlotFitLinear(J2DPlotWidget* plot, JPlotDataBase* fitData,
					const JFloat xMin, const JFloat xMax);
	JPlotFitLinear(J2DPlotWidget* plot, JPlotDataBase* fitData, 
					const JFloat xmin, const JFloat xmax,
					const JFloat ymin, const JFloat ymax);
	virtual ~JPlotFitLinear();	

private:

	void				JPlotFitLinearX(J2DPlotWidget* plot, 
										JPlotDataBase* fitData);
};

#endif
