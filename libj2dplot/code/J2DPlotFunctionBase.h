/*********************************************************************************
 J2DPlotFunctionBase.h

	Interface for the J2DPlotFunctionBase class.

	Copyright @ 1997 by Glenn W. Bach.

 ********************************************************************************/

#ifndef _H_JPlotFunctionBase
#define _H_JPlotFunctionBase

#include "jx-af/j2dplot/J2DPlotDataBase.h"
#include <jx-af/jcore/JArray.h>

class JString;
class J2DPlotWidget;

class J2DPlotFunctionBase : public J2DPlotDataBase
{
public:

	J2DPlotFunctionBase(const Type type, J2DPlotWidget* plot,
					  const JFloat xMin, const JFloat xMax);

	virtual ~J2DPlotFunctionBase();

	bool	IsFunction() const override;
	void	GetElement(const JIndex index, J2DDataPoint* data) const override;

	void	GetXRange(JFloat* min, JFloat* max) const override;
	bool	GetYRange(const JFloat xMin, const JFloat xMax,
							  const bool xLinear,
							  JFloat* yMin, JFloat* yMax) const override;

	virtual JString	GetFunctionString() const = 0;
	virtual bool	GetYValue(const JFloat x, JFloat* y) const = 0;

	void	SetXRange(const JFloat xMin, const JFloat xMax);

protected:

	void	UpdateFunction();
	void	UpdateFunction(const JFloat xmin, const JFloat xmax,
						   const JSize stepCount);

	void	Receive(JBroadcaster* sender, const Message& message) override;

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

	void	EvaluateFunction(const JFloat min, const JFloat max, const bool linear,
							 const JSize stepCount, JArray<Point>* list) const;
};


/*********************************************************************************
 SetXRange

 ********************************************************************************/

inline void
J2DPlotFunctionBase::SetXRange
	(
	const JFloat min,
	const JFloat max
	)
{
	itsXMin = min;
	itsXMax = max;
}

#endif
