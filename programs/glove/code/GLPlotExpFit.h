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

	virtual JBoolean	GetYRange(const JFloat xMin, const JFloat xMax,
								  const JBoolean xLinear,
								  JFloat* yMin, JFloat* yMax) const override;

	virtual JBoolean	GetYValue(const JFloat x, JFloat* y) const override;
	virtual const JPlotDataBase*		GetDataToFit() const override;

	virtual JBoolean	GetParameter(const JIndex index, JFloat* value) const override;
	virtual JBoolean	GetParameterError(const JIndex index, JFloat* value) const override;
//	virtual JBoolean	GetGoodnessOfFit(JFloat* value) const;

private:

	J2DPlotData*		itsAdjustedData;
	JArray<JFloat>*		itsXData;
	JArray<JFloat>*		itsYData;
	JArray<JFloat>*		itsXErrData;
	JArray<JFloat>*		itsYErrData;
	JBoolean			itsHasXErrors;
	JBoolean			itsHasYErrors;
	
private:

	void					AdjustData();
	void					CreateData();
	void					AdjustDiffData();
		
};

#endif
