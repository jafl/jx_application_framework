/*********************************************************************************
 JPlotExpFit.h
 
	Interface for the JPlotExpFit class.
 
	Copyright @ 1997 by Glenn W. Bach.

 ********************************************************************************/
 
#ifndef _H_JPlotExpFit
#define _H_JPlotExpFit

#include <JPlotLinearFit.h>
#include <jTypes.h>
#include <JArray.h>

class J2DPlotWidget;
class JString;

class JPlotExpFit : public JPlotLinearFit
{
public:

	JPlotExpFit(J2DPlotWidget* plot, JPlotDataBase* fitData,
				const JFloat xMin, const JFloat xMax);
	virtual ~JPlotExpFit();	

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
