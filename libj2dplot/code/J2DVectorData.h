/*********************************************************************************
 J2DVectorData.h

	Interface for the J2DVectorData class.

	Copyright @ 1998 by Glenn W. Bach. All rights reserved.

 ********************************************************************************/

#ifndef _H_J2DVectorData
#define _H_J2DVectorData

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JPlotDataBase.h>
#include <JArray.h>

class J2DVectorData : public JPlotDataBase
{
public:

	static JBoolean	Create( J2DVectorData** plotData,
							const JArray<JFloat>& x,  const JArray<JFloat>& y,
							const JArray<JFloat>& vx, const JArray<JFloat>& vy,
							const JBoolean listen);

	static JBoolean	OKToCreate( const JArray<JFloat>& x,  const JArray<JFloat>& y,
								const JArray<JFloat>& vx, const JArray<JFloat>& vy);

	J2DVectorData();

	virtual ~J2DVectorData();

	virtual void		GetElement(const JIndex index, J2DDataPoint* data) const;
	virtual void		GetElement(const JIndex index, J2DVectorPoint* data) const;

	virtual void		GetXRange(JFloat* min, JFloat* max) const;
	virtual JBoolean	GetYRange(const JFloat xMin, const JFloat xMax,
								  const JBoolean xLinear,
								  JFloat* yMin, JFloat* yMax) const;

	void	AddElement(const JFloat x,  const JFloat y,
					   const JFloat vx, const JFloat vy);
	void	AddElement(const J2DVectorPoint& data);
	void	RemoveElement(const JIndex index);

	JBoolean	IsValid() const;

	const JArray<JFloat>&	GetXData() const;
	const JArray<JFloat>&	GetYData() const;
	const JArray<JFloat>&	GetVXData() const;
	const JArray<JFloat>&	GetVYData() const;

	JBoolean	IsListening() const;
	void		IgnoreDataChanges();

	JBoolean	ArrayInData(const JArray<JFloat>* array) const;

protected:

	J2DVectorData(const JArray<JFloat>& x,  const JArray<JFloat>& y,
				  const JArray<JFloat>& vx, const JArray<JFloat>& vy,
				  const JBoolean listen);

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JArray<JFloat>*	itsXData;
	JArray<JFloat>*	itsYData;
	JArray<JFloat>*	itsVXData;
	JArray<JFloat>*	itsVYData;

	JBoolean	itsIsValidFlag;
	JBoolean	itsIsListeningFlag;

	JFloat	itsCurrentXMin;
	JFloat	itsCurrentXMax;
	JFloat	itsCurrentYMin;
	JFloat	itsCurrentYMax;

private:

	void	J2DVectorDataX();
	void	ValidateData();

	// not allowed

	J2DVectorData(const J2DVectorData& source);
	const J2DVectorData& operator=(const J2DVectorData& source);
};


/*********************************************************************************
 IsValid

 ********************************************************************************/

inline JBoolean
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

inline JBoolean
J2DVectorData::IsListening()
	const
{
	return itsIsListeningFlag;
}

/*********************************************************************************
 ArrayInData

 ********************************************************************************/

inline JBoolean
J2DVectorData::ArrayInData
	(
	const JArray<JFloat>* array
	)
	const
{
	return JI2B(array == itsXData  || array == itsYData ||
				array == itsVXData || array == itsVYData);
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
