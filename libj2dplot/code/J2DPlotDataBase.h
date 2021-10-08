/*********************************************************************************
 J2DPlotDataBase.h

	Interface for the J2DPlotDataBase class.

	Copyright @ 1997 by Glenn W. Bach.

 ********************************************************************************/

#ifndef _H_JPlotDataBase
#define _H_JPlotDataBase

#include <jx-af/jcore/JCollection.h>
#include "jx-af/j2dplot/J2DDataPoint.h"
#include "jx-af/j2dplot/J2DVectorPoint.h"

class J2DPlotDataBase : public JCollection
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

	J2DPlotDataBase(const Type type);

	~J2DPlotDataBase() override;

	Type			GetType() const;
	virtual void	GetElement(const JIndex index, J2DDataPoint* data) const;
	virtual void	GetElement(const JIndex index, J2DVectorPoint* data) const;

	virtual void	GetXRange(JFloat* xMin, JFloat* xMax) const = 0;
	virtual bool	GetYRange(const JFloat xMin, const JFloat xMax,
							  const bool xLinear,
							  JFloat* yMin, JFloat* yMax) const = 0;

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

	J2DPlotDataBase(const J2DPlotDataBase&) = delete;
	J2DPlotDataBase& operator=(const J2DPlotDataBase&) = delete;

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

inline J2DPlotDataBase::Type
J2DPlotDataBase::GetType()
	const
{
	return itsType;
}

/*********************************************************************************
 BroadcastCurveChanged (protected)

 ********************************************************************************/

inline void
J2DPlotDataBase::BroadcastCurveChanged()
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
J2DPlotDataBase::WillBroadcastChanged()
	const
{
	return itsBroadcastFlag;
}

inline void
J2DPlotDataBase::ShouldBroadcastChanged
	(
	const bool bcast
	)
{
	itsBroadcastFlag = bcast;
	BroadcastCurveChanged();	// something has probably changed
}

#endif
