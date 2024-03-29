/*********************************************************************************
 J2DVectorData.h

	Interface for the J2DVectorData class.

	Copyright @ 1998 by Glenn W. Bach.

 ********************************************************************************/

#ifndef _H_J2DVectorData
#define _H_J2DVectorData

#include "J2DPlotDataBase.h"
#include <jx-af/jcore/JArray.h>

class J2DVectorData : public J2DPlotDataBase
{
public:

	static bool	Create( J2DVectorData** plotData,
							const JArray<JFloat>& x,  const JArray<JFloat>& y,
							const JArray<JFloat>& vx, const JArray<JFloat>& vy,
							const bool listen);

	static bool	OKToCreate( const JArray<JFloat>& x,  const JArray<JFloat>& y,
							const JArray<JFloat>& vx, const JArray<JFloat>& vy);

	J2DVectorData();

	~J2DVectorData() override;

	void	GetItem(const JIndex index, J2DDataPoint* data) const override;
	void	GetItem(const JIndex index, J2DVectorPoint* data) const override;

	void	GetXRange(JFloat* min, JFloat* max) const override;
	bool	GetYRange(const JFloat xMin, const JFloat xMax,
					  const bool xLinear,
					  JFloat* yMin, JFloat* yMax) const override;

	void	AddElement(const JFloat x,  const JFloat y,
					   const JFloat vx, const JFloat vy);
	void	AddElement(const J2DVectorPoint& data);
	void	RemoveItem(const JIndex index);

	bool	IsValid() const;

	const JArray<JFloat>&	GetXData() const;
	const JArray<JFloat>&	GetYData() const;
	const JArray<JFloat>&	GetVXData() const;
	const JArray<JFloat>&	GetVYData() const;

	bool	IsListening() const;
	void	IgnoreDataChanges();

	bool	ArrayInData(const JArray<JFloat>* array) const;

protected:

	J2DVectorData(const JArray<JFloat>& x,  const JArray<JFloat>& y,
				  const JArray<JFloat>& vx, const JArray<JFloat>& vy,
				  const bool listen);

	void	Receive(JBroadcaster* sender, const Message& message) override;
	void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	JArray<JFloat>*	itsXData;
	JArray<JFloat>*	itsYData;
	JArray<JFloat>*	itsVXData;
	JArray<JFloat>*	itsVYData;

	bool	itsIsValidFlag;
	bool	itsIsListeningFlag;

	mutable JFloat	itsCurrentXMin;
	mutable JFloat	itsCurrentXMax;
	mutable JFloat	itsCurrentYMin;
	mutable JFloat	itsCurrentYMax;

private:

	void	J2DVectorDataX();
	void	ValidateData();
};


/*********************************************************************************
 IsValid

 ********************************************************************************/

inline bool
J2DVectorData::IsValid()
	const
{
	return itsIsValidFlag;
}

/*********************************************************************************
 GetXData

 ********************************************************************************/

inline const JArray<JFloat>&
J2DVectorData::GetXData()
	const
{
	return *itsXData;
}

/*********************************************************************************
 GetYData

 ********************************************************************************/

inline const JArray<JFloat>&
J2DVectorData::GetYData()
	const
{
	return *itsYData;
}

/*********************************************************************************
 GetVXData

 ********************************************************************************/

inline const JArray<JFloat>&
J2DVectorData::GetVXData()
	const
{
	return *itsVXData;
}

/*********************************************************************************
 GetVYData

 ********************************************************************************/

inline const JArray<JFloat>&
J2DVectorData::GetVYData()
	const
{
	return *itsVYData;
}

/*********************************************************************************
 IsListening

 ********************************************************************************/

inline bool
J2DVectorData::IsListening()
	const
{
	return itsIsListeningFlag;
}

/*********************************************************************************
 ArrayInData

 ********************************************************************************/

inline bool
J2DVectorData::ArrayInData
	(
	const JArray<JFloat>* array
	)
	const
{
	return array == itsXData  || array == itsYData ||
				array == itsVXData || array == itsVYData;
}

/*********************************************************************************
 AddElement

 ********************************************************************************/

inline void
J2DVectorData::AddElement
	(
	const J2DVectorPoint& data
	)
{
	AddElement(data.x, data.y, data.vx, data.vy);
}

#endif
