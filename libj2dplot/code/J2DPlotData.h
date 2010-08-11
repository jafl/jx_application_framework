/*********************************************************************************
 J2DPlotData.h

	Interface for the J2DPlotData class.

	Copyright @ 1997 by Glenn W. Bach. All rights reserved.

 ********************************************************************************/

#ifndef _H_J2DPlotData
#define _H_J2DPlotData

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JPlotDataBase.h>
#include <JArray.h>

class J2DPlotData : public JPlotDataBase
{
public:

	static JBoolean Create(J2DPlotData** plotData,
							const JArray<JFloat>& x, const JArray<JFloat>& y,
							const JBoolean listen);

	static JBoolean OKToCreate(const JArray<JFloat>& x, const JArray<JFloat>& y);

	virtual ~J2DPlotData();

	virtual void GetElement(const JIndex index, J2DDataPoint* data) const;

	JBoolean SetXErrors(const JArray<JFloat>& xErr);
	JBoolean SetXErrors(const JArray<JFloat>& xPErr, const JArray<JFloat>& xMErr);

	JBoolean SetYErrors(const JArray<JFloat>& yErr);
	JBoolean SetYErrors(const JArray<JFloat>& yPErr, const JArray<JFloat>& yMErr);

	virtual void		GetXRange(JFloat* min, JFloat* max) const;
	virtual JBoolean	GetYRange(const JFloat xMin, const JFloat xMax,
								  const JBoolean xLinear,
								  JFloat* yMin, JFloat* yMax) const;

	virtual JBoolean HasXErrors() const;
	virtual JBoolean HasYErrors() const;
	virtual JBoolean HasSymmetricXErrors() const;
	virtual JBoolean HasSymmetricYErrors() const;

	JBoolean	IsValid() const;

	const JArray<JFloat>&	GetXData() const;
	const JArray<JFloat>&	GetYData() const;
	JBoolean				GetXPErrorData(const JArray<JFloat>** array) const;
	JBoolean				GetXMErrorData(const JArray<JFloat>** array) const;
	JBoolean				GetYPErrorData(const JArray<JFloat>** array) const;
	JBoolean				GetYMErrorData(const JArray<JFloat>** array) const;

	JBoolean	IsListening() const;
	void		IgnoreDataChanges();

	JBoolean	ArrayInData(const JArray<JFloat>* array) const;

protected:

	J2DPlotData(const JArray<JFloat>& x, const JArray<JFloat>& y,
				const JBoolean listen = kJFalse);

	virtual void	Receive(JBroadcaster* sender, const Message& message);
	virtual void	ReceiveGoingAway(JBroadcaster* sender);

private:

	JArray<JFloat>* itsXData;			// NULL if deleted by owner
	JArray<JFloat>* itsXPErrorData;		// NULL if no error bars
	JArray<JFloat>* itsXMErrorData;		// NULL if not symmetric errors
	JArray<JFloat>* itsYData;			// NULL if deleted by owner
	JArray<JFloat>* itsYPErrorData;		// NULL if no error bars
	JArray<JFloat>* itsYMErrorData;		// NULL if not symmetric errors

	JBoolean	itsIsValidFlag;
	JBoolean	itsIsListeningFlag;

	mutable JFloat	itsCurrentXMin;
	mutable JFloat	itsCurrentXMax;
	mutable JFloat	itsCurrentYMin;
	mutable JFloat	itsCurrentYMax;

private:

	void ValidateCurve();
};


/*********************************************************************************
 HasXErrors

 ********************************************************************************/

inline JBoolean
J2DPlotData::HasXErrors()
	const
{
	return JI2B(itsXPErrorData != NULL);
}

/*********************************************************************************
 HasYErrors

 ********************************************************************************/

inline JBoolean
J2DPlotData::HasYErrors()
	const
{
	return JI2B(itsYPErrorData != NULL);
}

/*********************************************************************************
 HasSymmetricXErrors

 ********************************************************************************/

inline JBoolean
J2DPlotData::HasSymmetricXErrors()
	const
{
	return JI2B(itsXMErrorData != NULL);
}

/*********************************************************************************
 HasSymmetricYErrors

 ********************************************************************************/

inline JBoolean
J2DPlotData::HasSymmetricYErrors()
	const
{
	return JI2B(itsYMErrorData != NULL);
}

/*********************************************************************************
 IsValid

 ********************************************************************************/

inline JBoolean
J2DPlotData::IsValid()
	const
{
	return itsIsValidFlag;
}

/*********************************************************************************
 GetXData

 ********************************************************************************/

inline const JArray<JFloat>&
J2DPlotData::GetXData()
	const
{
	return *itsXData;
}

/*********************************************************************************
 GetYData

 ********************************************************************************/

inline const JArray<JFloat>&
J2DPlotData::GetYData()
	const
{
	return *itsYData;
}

/*********************************************************************************
 GetXPErrorData

 ********************************************************************************/

inline JBoolean
J2DPlotData::GetXPErrorData
	(
	const JArray<JFloat>** array
	)
	const
{
	*array = itsXPErrorData;
	return JI2B( itsXPErrorData != NULL );
}

/*********************************************************************************
 GetXMErrorData

 ********************************************************************************/

inline JBoolean
J2DPlotData::GetXMErrorData
	(
	const JArray<JFloat>** array
	)
	const
{
	*array = itsXMErrorData;
	return JI2B( itsXMErrorData != NULL );
}

/*********************************************************************************
 GetYPErrorData

 ********************************************************************************/

inline JBoolean
J2DPlotData::GetYPErrorData
	(
	const JArray<JFloat>** array
	)
	const
{
	*array = itsYPErrorData;
	return JI2B( itsYPErrorData != NULL );
}

/*********************************************************************************
 GetYMErrorData

 ********************************************************************************/

inline JBoolean
J2DPlotData::GetYMErrorData
	(
	const JArray<JFloat>** array
	)
	const
{
	*array = itsYMErrorData;
	return JI2B( itsYMErrorData != NULL );
}

/*********************************************************************************
 IsListening

 ********************************************************************************/

inline JBoolean
J2DPlotData::IsListening()
	const
{
	return itsIsListeningFlag;
}

/*********************************************************************************
 ArrayInData

 ********************************************************************************/

inline JBoolean
J2DPlotData::ArrayInData
	(
	const JArray<JFloat>* array
	)
	const
{
	return JI2B(array == itsXData       || array == itsYData ||
				array == itsXPErrorData || array == itsXMErrorData ||
				array == itsYPErrorData || array == itsYMErrorData);
}

#endif
