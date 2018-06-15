/*********************************************************************************
 JPlotDataBase.h

	Interface for the JPlotDataBase class.

	Copyright @ 1997 by Glenn W. Bach.

 ********************************************************************************/

#ifndef _H_JPlotDataBase
#define _H_JPlotDataBase

#include <JCollection.h>
#include "J2DDataPoint.h"
#include "J2DVectorPoint.h"

class JPlotDataBase : public JCollection
{
public:

	enum Type
	{
		kScatterPlot,
		kSurfacePlot,
		kContourPlot,
		kVectorPlot
	};

public:

	JPlotDataBase(const Type type);

	virtual ~JPlotDataBase();

	Type			GetType() const;
	virtual void	GetElement(const JIndex index, J2DDataPoint* data) const;
	virtual void	GetElement(const JIndex index, J2DVectorPoint* data) const;

	virtual void		GetXRange(JFloat* xMin, JFloat* xMax) const = 0;
	virtual JBoolean	GetYRange(const JFloat xMin, const JFloat xMax,
								  const JBoolean xLinear,
								  JFloat* yMin, JFloat* yMax) const = 0;
	virtual JBoolean	GetZRange(const JFloat xMin, const JFloat xMax,
								  const JBoolean xLinear,
								  const JFloat yMin, const JFloat yMax,
								  const JBoolean yLinear,
								  JFloat* zMin, JFloat* zMax) const;
	virtual JBoolean	Get4thRange(const JFloat xMin, const JFloat xMax,
									const JBoolean xLinear,
									const JFloat yMin, const JFloat yMax,
									const JBoolean yLinear,
									const JFloat zMin, const JFloat zMax,
									const JBoolean zLinear,
									JFloat* min, JFloat* max) const;

	virtual JBoolean	HasXErrors() const;
	virtual JBoolean	HasYErrors() const;
	virtual JBoolean	HasSymmetricXErrors() const;
	virtual JBoolean	HasSymmetricYErrors() const;

	virtual JBoolean	IsFunction() const;

	JBoolean	WillBroadcastChanged() const;
	void		ShouldBroadcastChanged(const JBoolean bcast = kJTrue);

protected:

	void	BroadcastCurveChanged();

private:

	const Type	itsType;
	JBoolean	itsBroadcastFlag;	// kJTrue => broadcast when changed

public:

	static const JUtf8Byte* kCurveChanged;

	class CurveChanged : public JBroadcaster::Message
	{
	public:

		CurveChanged()
			:
			JBroadcaster::Message(kCurveChanged)
			{ };
	};
};


/*********************************************************************************
 GetType

 ********************************************************************************/

inline JPlotDataBase::Type
JPlotDataBase::GetType()
	const
{
	return itsType;
}

/*********************************************************************************
 BroadcastCurveChanged (protected)

 ********************************************************************************/

inline void
JPlotDataBase::BroadcastCurveChanged()
{
	if (itsBroadcastFlag)
		{
		Broadcast(CurveChanged());
		}
}

/*********************************************************************************
 Broadcast CurveChanged message

 ********************************************************************************/

inline JBoolean
JPlotDataBase::WillBroadcastChanged()
	const
{
	return itsBroadcastFlag;
}

inline void
JPlotDataBase::ShouldBroadcastChanged
	(
	const JBoolean bcast
	)
{
	itsBroadcastFlag = bcast;
	BroadcastCurveChanged();	// something has probably changed
}

#endif
