/*********************************************************************************
 GLPlotFitQuad.h
 
	Interface for the GLPlotFitQuad class.
 
	Copyright @ 2000 by Glenn W. Bach.

 ********************************************************************************/
 
#ifndef _H_GLPlotFitQuad
#define _H_GLPlotFitQuad

#include <GLPlotFitLinearEq.h>

class J2DPlotWidget;

class GLPlotFitQuad : public GLPlotFitLinearEq
{
public:

	GLPlotFitQuad(J2DPlotWidget* plot, JPlotDataBase* fitData,
					const JFloat xMin, const JFloat xMax);
	GLPlotFitQuad(J2DPlotWidget* plot, JPlotDataBase* fitData, 
					const JFloat xmin, const JFloat xmax,
					const JFloat ymin, const JFloat ymax);
	virtual ~GLPlotFitQuad();	

private:

	void	JPlotFitQuadX(J2DPlotWidget* plot, JPlotDataBase* fitData);	
};

#endif
