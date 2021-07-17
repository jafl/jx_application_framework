/*********************************************************************************
 GLPlotExpFit.h
 
	Interface for the GLPlotExpFit class.
 
	Copyright @ 1997 by Glenn W. Bach.

 ********************************************************************************/
 
#ifndef _H_GLPlotExpFit
#define _H_GLPlotExpFit

#include <GLPlotLinearFit.h>
#include <jTypes.h>
#include <JArray.h>

class J2DPlotWidget;
class JString;

class GLPlotExpFit : public GLPlotLinearFit
{
public:

	GLPlotExpFit(J2DPlotWidget* plot, JPlotDataBase* fitData,
				const JFloat xMin, const JFloat xMax);
	virtual ~GLPlotExpFit();	

	virtual bool	GetYRange(const JFloat xMin, const JFloat xMax,
								  const bool xLinear,
								  JFloat* yMin, JFloat* yMax) const override;

	virtual bool	GetYValue(const JFloat x, JFloat* y) const override;
	virtual const JPlotDataBase*		GetDataToFit() const override;

	virtual bool	GetParameter(const JIndex index, JFloat* value) const override;
	virtual bool	GetParameterError(const JIndex index, JFloat* value) const override;
//	virtual bool	GetGoodnessOfFit(JFloat* value) const;

private:

	J2DPlotData*		itsAdjustedData;
	JArray<JFloat>*		itsXData;
	JArray<JFloat>*		itsYData;
	JArray<JFloat>*		itsXErrData;
	JArray<JFloat>*		itsYErrData;
	bool			itsHasXErrors;
	bool			itsHasYErrors;
	
private:

	void					AdjustData();
	void					CreateData();
	void					AdjustDiffData();
		
};

#endif
