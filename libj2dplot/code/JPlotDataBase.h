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

	virtual void	GetXRange(JFloat* xMin, JFloat* xMax) const = 0;
	virtual bool	GetYRange(const JFloat xMin, const JFloat xMax,
								  const bool xLinear,
								  JFloat* yMin, JFloat* yMax) const = 0;
	virtual bool	GetZRange(const JFloat xMin, const JFloat xMax,
								  const bool xLinear,
								  const JFloat yMin, const JFloat yMax,
								  const bool yLinear,
								  JFloat* zMin, JFloat* zMax) const;
	virtual bool	Get4thRange(const JFloat xMin, const JFloat xMax,
									const bool xLinear,
									const JFloat yMin, const JFloat yMax,
									const bool yLinear,
									const JFloat zMin, const JFloat zMax,
									const bool zLinear,
									JFloat* min, JFloat* max) const;

	virtual bool	HasXErrors() const;
	virtual bool	HasYErrors() const;
	virtual bool	HasSymmetricXErrors() const;
	virtual bool	HasSymmetricYErrors() const;

	virtual bool	IsFunction() const;

	bool	WillBroadcastChanged() const;
	void	ShouldBroadcastChanged(const bool bcast = true);

protected:

	void	BroadcastCurveChanged();

private:

	const Type	itsType;
	bool		itsBroadcastFlag;	// true => broadcast when changed

	// not allowed

	JPlotDataBase(const JPlotDataBase&) = delete;
	JPlotDataBase& operator=(const JPlotDataBase&) = delete;

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

inline bool
JPlotDataBase::WillBroadcastChanged()
	const
{
	return itsBroadcastFlag;
}

inline void
JPlotDataBase::ShouldBroadcastChanged
	(
	const bool bcast
	)
{
	itsBroadcastFlag = bcast;
	BroadcastCurveChanged();	// something has probably changed
}

#endif
