/*********************************************************************************
 J2DPlotData.h

	Interface for the J2DPlotData class.

	Copyright @ 1997 by Glenn W. Bach.

 ********************************************************************************/

#ifndef _H_J2DPlotData
#define _H_J2DPlotData

#include "J2DPlotDataBase.h"
#include <jx-af/jcore/JArray.h>

class J2DPlotData : public J2DPlotDataBase
{
public:

	static bool Create(J2DPlotData** plotData,
						const JArray<JFloat>& x, const JArray<JFloat>& y,
						const bool listen);

	static bool OKToCreate(const JArray<JFloat>& x, const JArray<JFloat>& y);

	~J2DPlotData() override;

	using J2DPlotDataBase::GetItem;

	void GetItem(const JIndex index, J2DDataPoint* data) const override;

	bool SetXErrors(const JArray<JFloat>& xErr);
	bool SetXErrors(const JArray<JFloat>& xPErr, const JArray<JFloat>& xMErr);

	bool SetYErrors(const JArray<JFloat>& yErr);
	bool SetYErrors(const JArray<JFloat>& yPErr, const JArray<JFloat>& yMErr);

	void	GetXRange(JFloat* min, JFloat* max) const override;
	bool	GetYRange(const JFloat xMin, const JFloat xMax,
					  const bool xLinear,
					  JFloat* yMin, JFloat* yMax) const override;

	bool HasXErrors() const override;
	bool HasYErrors() const override;
	bool HasSymmetricXErrors() const override;
	bool HasSymmetricYErrors() const override;

	bool	IsValid() const;

	const JArray<JFloat>&	GetXData() const;
	const JArray<JFloat>&	GetYData() const;
	bool					GetXPErrorData(const JArray<JFloat>** array) const;
	bool					GetXMErrorData(const JArray<JFloat>** array) const;
	bool					GetYPErrorData(const JArray<JFloat>** array) const;
	bool					GetYMErrorData(const JArray<JFloat>** array) const;

	bool	IsListening() const;
	void	IgnoreDataChanges();

	bool	ArrayInData(const JArray<JFloat>* array) const;

protected:

	J2DPlotData(const JArray<JFloat>& x, const JArray<JFloat>& y,
				const bool listen = false);

	void	Receive(JBroadcaster* sender, const Message& message) override;
	void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	JArray<JFloat>* itsXData;			// nullptr if deleted by owner
	JArray<JFloat>* itsXPErrorData;		// nullptr if no error bars
	JArray<JFloat>* itsXMErrorData;		// nullptr if not symmetric errors
	JArray<JFloat>* itsYData;			// nullptr if deleted by owner
	JArray<JFloat>* itsYPErrorData;		// nullptr if no error bars
	JArray<JFloat>* itsYMErrorData;		// nullptr if not symmetric errors

	bool	itsIsValidFlag;
	bool	itsIsListeningFlag;

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

inline bool
J2DPlotData::HasXErrors()
	const
{
	return itsXPErrorData != nullptr;
}

/*********************************************************************************
 HasYErrors

 ********************************************************************************/

inline bool
J2DPlotData::HasYErrors()
	const
{
	return itsYPErrorData != nullptr;
}

/*********************************************************************************
 HasSymmetricXErrors

 ********************************************************************************/

inline bool
J2DPlotData::HasSymmetricXErrors()
	const
{
	return itsXMErrorData != nullptr;
}

/*********************************************************************************
 HasSymmetricYErrors

 ********************************************************************************/

inline bool
J2DPlotData::HasSymmetricYErrors()
	const
{
	return itsYMErrorData != nullptr;
}

/*********************************************************************************
 IsValid

 ********************************************************************************/

inline bool
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

inline bool
J2DPlotData::GetXPErrorData
	(
	const JArray<JFloat>** array
	)
	const
{
	*array = itsXPErrorData;
	return itsXPErrorData != nullptr;
}

/*********************************************************************************
 GetXMErrorData

 ********************************************************************************/

inline bool
J2DPlotData::GetXMErrorData
	(
	const JArray<JFloat>** array
	)
	const
{
	*array = itsXMErrorData;
	return itsXMErrorData != nullptr;
}

/*********************************************************************************
 GetYPErrorData

 ********************************************************************************/

inline bool
J2DPlotData::GetYPErrorData
	(
	const JArray<JFloat>** array
	)
	const
{
	*array = itsYPErrorData;
	return itsYPErrorData != nullptr;
}

/*********************************************************************************
 GetYMErrorData

 ********************************************************************************/

inline bool
J2DPlotData::GetYMErrorData
	(
	const JArray<JFloat>** array
	)
	const
{
	*array = itsYMErrorData;
	return itsYMErrorData != nullptr;
}

/*********************************************************************************
 IsListening

 ********************************************************************************/

inline bool
J2DPlotData::IsListening()
	const
{
	return itsIsListeningFlag;
}

/*********************************************************************************
 ArrayInData

 ********************************************************************************/

inline bool
J2DPlotData::ArrayInData
	(
	const JArray<JFloat>* array
	)
	const
{
	return array == itsXData       || array == itsYData ||
				array == itsXPErrorData || array == itsXMErrorData ||
				array == itsYPErrorData || array == itsYMErrorData;
}

#endif
