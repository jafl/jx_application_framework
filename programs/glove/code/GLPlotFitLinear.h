/*********************************************************************************
 GLPlotFitLinear.h
 
	Interface for the GLPlotFitLinear class.
 
	Copyright @ 2000 by Glenn W. Bach.

 ********************************************************************************/
 
#ifndef _H_GLPlotFitLinear
#define _H_GLPlotFitLinear

#include <GLPlotFitLinearEq.h>

class J2DPlotWidget;

class GLPlotFitLinear : public GLPlotFitLinearEq
{
public:

	GLPlotFitLinear(J2DPlotWidget* plot, JPlotDataBase* fitData,
					const JFloat xMin, const JFloat xMax);
	GLPlotFitLinear(J2DPlotWidget* plot, JPlotDataBase* fitData, 
					const JFloat xmin, const JFloat xmax,
					const JFloat ymin, const JFloat ymax);
	virtual ~GLPlotFitLinear();	

private:

	void				JPlotFitLinearX(J2DPlotWidget* plot, 
										JPlotDataBase* fitData);
};

#endif
