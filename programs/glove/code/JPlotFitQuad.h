/*********************************************************************************
 JPlotFitQuad.h
 
	Interface for the JPlotFitQuad class.
 
	Copyright @ 2000 by Glenn W. Bach.

 ********************************************************************************/
 
#ifndef _H_JPlotFitQuad
#define _H_JPlotFitQuad

#include <JPlotFitLinearEq.h>

class J2DPlotWidget;

class JPlotFitQuad : public JPlotFitLinearEq
{
public:

	JPlotFitQuad(J2DPlotWidget* plot, JPlotDataBase* fitData,
					const JFloat xMin, const JFloat xMax);
	JPlotFitQuad(J2DPlotWidget* plot, JPlotDataBase* fitData, 
					const JFloat xmin, const JFloat xmax,
					const JFloat ymin, const JFloat ymax);
	virtual ~JPlotFitQuad();	

private:

	void	JPlotFitQuadX(J2DPlotWidget* plot, JPlotDataBase* fitData);	
};

#endif
