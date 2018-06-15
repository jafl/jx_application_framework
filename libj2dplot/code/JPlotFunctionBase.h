/*********************************************************************************
 JPlotFunctionBase.h

	Interface for the JPlotFunctionBase class.

	Copyright @ 1997 by Glenn W. Bach.

 ********************************************************************************/

#ifndef _H_JPlotFunctionBase
#define _H_JPlotFunctionBase

#include "JPlotDataBase.h"
#include <JArray.h>

class JString;
class J2DPlotWidget;

class JPlotFunctionBase : public JPlotDataBase
{
public:

	JPlotFunctionBase(const Type type, J2DPlotWidget* plot,
					  const JFloat xMin, const JFloat xMax);

	virtual ~JPlotFunctionBase();

	virtual JBoolean	IsFunction() const override;
	virtual void		GetElement(const JIndex index, J2DDataPoint* data) const override;

	virtual void		GetXRange(JFloat* min, JFloat* max) const override;
	virtual JBoolean	GetYRange(const JFloat xMin, const JFloat xMax,
								  const JBoolean xLinear,
								  JFloat* yMin, JFloat* yMax) const override;

	virtual JString		GetFunctionString() const = 0;
	virtual JBoolean	GetYValue(const JFloat x, JFloat* y) const = 0;

	void	SetXRange(const JFloat xMin, const JFloat xMax);

protected:

	void	UpdateFunction();
	void	UpdateFunction(const JFloat xmin, const JFloat xmax,
						   const JSize stepCount);

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	struct Point
	{
		JFloat x,y;
	};

private:

	J2DPlotWidget*	itsPlot;

	// values returned by GetXRange()

	JFloat	itsXMin;
	JFloat	itsXMax;

	// values returned by GetElement()

	JArray<Point>*	itsValues;

private:

	void	EvaluateFunction(const JFloat min, const JFloat max, const JBoolean linear,
							 const JSize stepCount, JArray<Point>* list) const;
};


/*********************************************************************************
 SetXRange

 ********************************************************************************/

inline void
JPlotFunctionBase::SetXRange
	(
	const JFloat min,
	const JFloat max
	)
{
	itsXMin = min;
	itsXMax = max;
}

#endif
