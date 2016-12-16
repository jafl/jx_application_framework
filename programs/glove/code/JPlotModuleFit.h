/*********************************************************************************
 JPlotModuleFit.h
 
	Interface for the JPlotModuleFit class.
 
	Copyright @ 1997 by Glenn W. Bach. All rights reserved.

 ********************************************************************************/
 
#ifndef _H_JPlotModuleFit
#define _H_JPlotModuleFit

#include <JPlotFitFunction.h>
#include <JArray.h>
#include <JPtrArray.h>

class J2DPlotWidget;
class JString;
class JFunction;
class GVarList;

class JPlotModuleFit : public JPlotFitFunction
{
public:

	JPlotModuleFit(	J2DPlotWidget* plot, JPlotDataBase* fitData,
					const JFloat xMin, const JFloat xMax,
					JPtrArray<JString>* names, JArray<JFloat>* values,
					JFunction*	function,
					GVarList* list,
					const JSize parmscount,	
					const JBoolean errors = kJFalse, 
					const JBoolean gof = kJFalse);
	JPlotModuleFit(J2DPlotWidget* plot, JPlotDataBase* fitData, 
					JPtrArray<JString>* names, JArray<JFloat>* values,
					JFunction*	function,
					GVarList* list,
					const JFloat xmin, const JFloat xmax,
					const JFloat ymin, const JFloat ymax,
					const JSize parmscount,	
					const JBoolean errors = kJFalse, 
					const JBoolean gof = kJFalse);
	JPlotModuleFit(J2DPlotWidget* plot, JPlotDataBase* fitData, std::istream& is);
	virtual ~JPlotModuleFit();	

	virtual void GetElement(const JIndex index, J2DDataPoint* data) const;
	virtual JBoolean GetYRange(	JFloat* min, JFloat* max,
								JFloat  xMin, JFloat  xMax);

	virtual	void UpdateFunction(const JFloat xmin, const JFloat xmax, 
								const JSize steps);

	virtual JBoolean	GetParameterName(const JIndex index, JString* name) const;
	virtual JBoolean	GetParameter(const JIndex index, JFloat* value) const;

	virtual JBoolean	GetParameterError(const JIndex index, JFloat* value) const;

	virtual JBoolean	GetGoodnessOfFitName(JString* name) const;
	virtual JBoolean	GetGoodnessOfFit(JFloat* value) const;
	
	virtual JString		GetFunctionString() const;
	virtual JString		GetFitFunctionString() const;

	virtual JBoolean	GetYValue(const JFloat x, JFloat* y) const;
	virtual const JPlotDataBase*		GetDataToFit() const;
	
	void				WriteData(std::ostream& os);


protected:

	JFloat				GetCurrentXMax() const;
	JFloat				GetCurrentXMin() const;
	JFloat				GetCurrentStepCount() const;
	virtual JBoolean	DataElementValid(const JIndex index);
	virtual JBoolean	GetDataElement(const JIndex index, J2DDataPoint* point);

private:

	void		JPlotModuleFitX(J2DPlotWidget* plot, 
								JPlotDataBase* fitData, 
								JPtrArray<JString>* names, 
								JArray<JFloat>* values,
								JFunction*	function,
								GVarList* list,
								const JSize parmscount,	
								const JBoolean errors = kJFalse, 
								const JBoolean gof = kJFalse);

private:

	JString*	itsFunctionName;
	JFloat		itsCurrentXMin;
	JFloat		itsCurrentXMax;
	JSize		itsCurrentStepCount;
	JFloat 		itsRangeXMax;
	JFloat		itsRangeXMin;
	JFloat 		itsRangeYMax;
	JFloat 		itsRangeYMin;
	JBoolean 	itsUsingRange;
	JPtrArray<JString>* itsNames;
	JArray<JFloat>* 	itsValues;
	JFunction*			itsFunction;
	GVarList*			itsList;

};

/*********************************************************************************
 GetDataToFit
 

 ********************************************************************************/

inline const JPlotDataBase*
JPlotModuleFit::GetDataToFit()
	const
{
	return GetData();
}

/*********************************************************************************
 GetCurrentXMax
 

 ********************************************************************************/

inline JFloat
JPlotModuleFit::GetCurrentXMax()
	const
{
	return itsCurrentXMax;
}

/*********************************************************************************
 GetCurrentXMin
 

 ********************************************************************************/

inline JFloat
JPlotModuleFit::GetCurrentXMin()
	const
{
	return itsCurrentXMin;
}

/*********************************************************************************
 GetCurrentStepCount
 

 ********************************************************************************/

inline JFloat
JPlotModuleFit::GetCurrentStepCount()
	const
{
	return itsCurrentStepCount;
}


#endif
