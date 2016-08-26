/*********************************************************************************
 JPlotFitFunction.h
 
	Interface for the JPlotFitFunction class.
 
	Copyright @ 1997 by Glenn W. Bach. All rights reserved.

 ********************************************************************************/
 
#ifndef _H_JPlotFitFunction
#define _H_JPlotFitFunction

#include <JPlotFunctionBase.h>
#include <JFitBase.h>
#include <JArray.h>

class J2DPlotWidget;
class J2DPlotData;

class JPlotFitFunction : public JPlotFunctionBase, public JFitBase
{
public:

	JPlotFitFunction(J2DPlotWidget* plot, JPlotDataBase* fitData, 
						const JFloat xMin, const JFloat xMax);
	virtual ~JPlotFitFunction();	

	const JPlotDataBase*	GetData() const;
	J2DPlotData*			GetDiffData() const;
	JFloat					GetStdDev() const;

protected:

	virtual void 		GenerateDiffData();
	void				CalculateStdDev();
	void				AdjustDiffDataValue(const JIndex index, const JFloat value);
	virtual JBoolean	DataElementValid(const JIndex index);
	virtual JBoolean	GetDataElement(const JIndex index, J2DDataPoint* point);
	void				SetDiffData(J2DPlotData* data);
	
private:

	JPlotDataBase*		itsData;
	J2DPlotData*		itsDiffData;
	JBoolean			itsHasXErrors;
	JBoolean			itsHasYErrors;
	JFloat				itsStdDev;
	
};

/*********************************************************************************
 GetData
 

 ********************************************************************************/
 
inline const JPlotDataBase*
JPlotFitFunction::GetData()
	const
{
	return itsData;
}

/*********************************************************************************
 GetData
 

 ********************************************************************************/
 
inline JFloat
JPlotFitFunction::GetStdDev()
	const
{
	return itsStdDev;
}


#endif
