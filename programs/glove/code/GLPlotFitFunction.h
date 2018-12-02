/*********************************************************************************
 GLPlotFitFunction.h
 
	Interface for the GLPlotFitFunction class.
 
	Copyright @ 1997 by Glenn W. Bach.

 ********************************************************************************/
 
#ifndef _H_GLPlotFitFunction
#define _H_GLPlotFitFunction

#include <JPlotFunctionBase.h>
#include <GLFitBase.h>
#include <JArray.h>

class J2DPlotWidget;
class J2DPlotData;

class GLPlotFitFunction : public JPlotFunctionBase, public GLFitBase
{
public:

	GLPlotFitFunction(J2DPlotWidget* plot, JPlotDataBase* fitData, 
						const JFloat xMin, const JFloat xMax);
	virtual ~GLPlotFitFunction();	

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
GLPlotFitFunction::GetData()
	const
{
	return itsData;
}

/*********************************************************************************
 GetData
 

 ********************************************************************************/
 
inline JFloat
GLPlotFitFunction::GetStdDev()
	const
{
	return itsStdDev;
}


#endif
