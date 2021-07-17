/*********************************************************************************
 GLPlotModuleFit.h
 
	Interface for the GLPlotModuleFit class.
 
	Copyright @ 1997 by Glenn W. Bach.

 ********************************************************************************/
 
#ifndef _H_GLPlotModuleFit
#define _H_GLPlotModuleFit

#include <GLPlotFitFunction.h>
#include <JPtrArray-JString.h>

class J2DPlotWidget;
class JFunction;
class GLVarList;

class GLPlotModuleFit : public GLPlotFitFunction
{
public:

	GLPlotModuleFit(	J2DPlotWidget* plot, JPlotDataBase* fitData,
					const JFloat xMin, const JFloat xMax,
					JPtrArray<JString>* names, JArray<JFloat>* values,
					JFunction*	function,
					GLVarList* list,
					const JSize parmscount,	
					const bool errors = false, 
					const bool gof = false);
	GLPlotModuleFit(J2DPlotWidget* plot, JPlotDataBase* fitData, 
					JPtrArray<JString>* names, JArray<JFloat>* values,
					JFunction*	function,
					GLVarList* list,
					const JFloat xmin, const JFloat xmax,
					const JFloat ymin, const JFloat ymax,
					const JSize parmscount,	
					const bool errors = false, 
					const bool gof = false);
	GLPlotModuleFit(J2DPlotWidget* plot, JPlotDataBase* fitData, std::istream& is);
	virtual ~GLPlotModuleFit();	

	virtual void GetElement(const JIndex index, J2DDataPoint* data) const override;

	virtual	void UpdateFunction(const JFloat xmin, const JFloat xmax, 
								const JSize steps);

	virtual bool	GetParameterName(const JIndex index, JString* name) const override;
	virtual bool	GetParameter(const JIndex index, JFloat* value) const override;

	virtual bool	GetParameterError(const JIndex index, JFloat* value) const override;

	virtual bool	GetGoodnessOfFitName(JString* name) const override;
	virtual bool	GetGoodnessOfFit(JFloat* value) const override;
	
	virtual JString		GetFunctionString() const override;
	virtual JString		GetFitFunctionString() const override;

	virtual bool	GetYValue(const JFloat x, JFloat* y) const override;
	virtual const JPlotDataBase*		GetDataToFit() const;
	
	void				WriteData(std::ostream& os);


protected:

	JFloat				GetCurrentXMax() const;
	JFloat				GetCurrentXMin() const;
	JFloat				GetCurrentStepCount() const;
	virtual bool	DataElementValid(const JIndex index) override;
	virtual bool	GetDataElement(const JIndex index, J2DDataPoint* point) override;

private:

	void		JPlotModuleFitX(J2DPlotWidget* plot, 
								JPlotDataBase* fitData, 
								JPtrArray<JString>* names, 
								JArray<JFloat>* values,
								JFunction*	function,
								GLVarList* list,
								const JSize parmscount,	
								const bool errors = false, 
								const bool gof = false);

private:

	JString		itsFunctionName;
	JFloat		itsCurrentXMin;
	JFloat		itsCurrentXMax;
	JSize		itsCurrentStepCount;
	JFloat 		itsRangeXMax;
	JFloat		itsRangeXMin;
	JFloat 		itsRangeYMax;
	JFloat 		itsRangeYMin;
	bool 	itsUsingRange;
	JPtrArray<JString>* itsNames;
	JArray<JFloat>* 	itsValues;
	JFunction*			itsFunction;
	GLVarList*			itsList;

};

/*********************************************************************************
 GetDataToFit
 

 ********************************************************************************/

inline const JPlotDataBase*
GLPlotModuleFit::GetDataToFit()
	const
{
	return GetData();
}

/*********************************************************************************
 GetCurrentXMax
 

 ********************************************************************************/

inline JFloat
GLPlotModuleFit::GetCurrentXMax()
	const
{
	return itsCurrentXMax;
}

/*********************************************************************************
 GetCurrentXMin
 

 ********************************************************************************/

inline JFloat
GLPlotModuleFit::GetCurrentXMin()
	const
{
	return itsCurrentXMin;
}

/*********************************************************************************
 GetCurrentStepCount
 

 ********************************************************************************/

inline JFloat
GLPlotModuleFit::GetCurrentStepCount()
	const
{
	return itsCurrentStepCount;
}


#endif
