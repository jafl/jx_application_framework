/*********************************************************************************
 J2DPlotData.cpp

	J2DPlotData class.

	Copyright @ 1997 by Glenn W. Bach.

 ********************************************************************************/

#include <J2DPlotData.h>
#include <jAssert.h>

/*********************************************************************************
 Create

 ********************************************************************************/

JBoolean
J2DPlotData::Create
	(
	J2DPlotData**			plotData,
	const JArray<JFloat>&	x,
	const JArray<JFloat>&	y,
	const JBoolean			listen
	)
{
	if (OKToCreate(x,y))
		{
		*plotData = jnew J2DPlotData(x,y,listen);
		assert( *plotData != nullptr );
		return kJTrue;
		}
	else
		{
		*plotData = nullptr;
		return kJFalse;
		}
}

/*********************************************************************************
 OKToCreate

 ********************************************************************************/

JBoolean
J2DPlotData::OKToCreate
	(
	const JArray<JFloat>& x,
	const JArray<JFloat>& y
	)
{
	return JI2B(x.GetElementCount() == y.GetElementCount());
}

/*********************************************************************************
 Constructor (protected)

 ********************************************************************************/

J2DPlotData::J2DPlotData
	(
	const JArray<JFloat>&	x,
	const JArray<JFloat>&	y,
	const JBoolean			listen
	)
	:
	JPlotDataBase(kScatterPlot)
{
	assert( OKToCreate(x,y) );

	SetElementCount(x.GetElementCount());
	assert(y.GetElementCount() == x.GetElementCount());

	if (listen)
		{
		itsXData = const_cast< JArray<JFloat>* >(&x);
		assert( itsXData != nullptr );
		ListenTo(itsXData);

		itsYData = const_cast< JArray<JFloat>* >(&y);
		assert( itsYData != nullptr );
		ListenTo(itsYData);
		}
	else
		{
		itsXData = jnew JArray<JFloat>(x);
		assert( itsXData != nullptr );
		itsYData = jnew JArray<JFloat>(y);
		assert( itsYData != nullptr );
		}

	itsXPErrorData = nullptr;
	itsXMErrorData = nullptr;
	itsYPErrorData = nullptr;
	itsYMErrorData = nullptr;

	itsIsValidFlag     = kJTrue;
	itsIsListeningFlag = listen;

	itsCurrentXMin = 0;
	itsCurrentXMax = 0;
	itsCurrentYMin = 0;
	itsCurrentYMax = 0;
}

/*********************************************************************************
 Destructor

 ********************************************************************************/

J2DPlotData::~J2DPlotData()
{
	if (!itsIsListeningFlag)
		{
		jdelete itsXData;
		jdelete itsXPErrorData;
		jdelete itsXMErrorData;
		jdelete itsYData;
		jdelete itsYPErrorData;
		jdelete itsYMErrorData;
		}
}

/*********************************************************************************
 GetElement

 ********************************************************************************/

void
J2DPlotData::GetElement
	(
	const JIndex	index,
	J2DDataPoint*	data
	)
	const
{
	data->x = itsXData->GetElement(index);
	data->y = itsYData->GetElement(index);

	if (itsXPErrorData != nullptr)
		{
		data->xerr = itsXPErrorData->GetElement(index);
		if (itsXMErrorData != nullptr)
			{
			data->xmerr = itsXMErrorData->GetElement(index);
			}
		else
			{
			data->xmerr = data->xerr;
			}
		}
	else
		{
		data->xerr  = 0;
		data->xmerr = 0;
		}

	if (itsYPErrorData != nullptr)
		{
		data->yerr = itsYPErrorData->GetElement(index);
		if (itsYMErrorData != nullptr)
			{
			data->ymerr = itsYMErrorData->GetElement(index);
			}
		else
			{
			data->ymerr = data->yerr;
			}
		}
	else
		{
		data->yerr  = 0;
		data->ymerr = 0;
		}
}

/*********************************************************************************
 GetXRange

 ********************************************************************************/

void
J2DPlotData::GetXRange
	(
	JFloat* min,
	JFloat* max
	)
	const
{
	if (!itsIsValidFlag)
		{
		*min = itsCurrentXMin;
		*max = itsCurrentXMax;
		}
	else
		{
		*min = 0;
		*max = 0;

		J2DDataPoint pt;
		const JSize count = GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			GetElement(i, &pt);
			if (pt.x - pt.xmerr < *min || i == 1)
				{
				*min = pt.x - pt.xmerr;
				}
			if (pt.x + pt.xerr > *max || i == 1)
				{
				*max = pt.x + pt.xerr;
				}
			}

		itsCurrentXMin = *min;
		itsCurrentXMax = *max;
		}
}

/*********************************************************************************
 GetYRange

 ********************************************************************************/

JBoolean
J2DPlotData::GetYRange
	(
	const JFloat	xMin,
	const JFloat	xMax,
	const JBoolean	xLinear,
	JFloat*			yMin,
	JFloat*			yMax
	)
	const
{
	if (!itsIsValidFlag)
		{
		*yMin = itsCurrentYMin;
		*yMax = itsCurrentYMax;
		}
	else
		{
		*yMin = 0;
		*yMax = 0;

		J2DDataPoint pt;
		const JSize count = GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			GetElement(i, &pt);
			if (pt.y - pt.ymerr < *yMin || i == 1)
				{
				*yMin = pt.y - pt.ymerr;
				}
			if (pt.y + pt.yerr > *yMax || i == 1)
				{
				*yMax = pt.y + pt.yerr;
				}
			}

		itsCurrentYMin = *yMin;
		itsCurrentYMax = *yMax;
		}

	return kJTrue;
}

/*********************************************************************************
 IgnoreDataChanges

 ********************************************************************************/

void
J2DPlotData::IgnoreDataChanges()
{
	assert( itsIsValidFlag );
	if (itsIsListeningFlag)
		{
		StopListening(itsXData);
		itsXData = jnew JArray<JFloat>(*itsXData);
		assert( itsXData != nullptr );

		if (itsXPErrorData != nullptr)
			{
			StopListening(itsXPErrorData);
			itsXPErrorData = jnew JArray<JFloat>(*itsXPErrorData);
			assert( itsXPErrorData != nullptr );
			}
		if (itsXMErrorData != nullptr)
			{
			StopListening(itsXMErrorData);
			itsXMErrorData = jnew JArray<JFloat>(*itsXMErrorData);
			assert( itsXMErrorData != nullptr );
			}

		StopListening(itsYData);
		itsYData = jnew JArray<JFloat>(*itsYData);
		assert( itsYData != nullptr );

		if (itsYPErrorData != nullptr)
			{
			StopListening(itsYPErrorData);
			itsYPErrorData = jnew JArray<JFloat>(*itsYPErrorData);
			assert( itsYPErrorData != nullptr );
			}
		if (itsYMErrorData != nullptr)
			{
			StopListening(itsYMErrorData);
			itsYMErrorData = jnew JArray<JFloat>(*itsYMErrorData);
			assert( itsYMErrorData != nullptr );
			}

		itsIsListeningFlag = kJFalse;
		}
}

/*********************************************************************************
 SetXErrors

 ********************************************************************************/

JBoolean
J2DPlotData::SetXErrors
	(
	const JArray<JFloat>& xErr
	)
{
	if (xErr.GetElementCount() != GetElementCount())
		{
		return kJFalse;
		}

	if (itsIsListeningFlag)
		{
		if (itsXPErrorData != nullptr)
			{
			StopListening(itsXPErrorData);
			}
		if (itsXMErrorData != nullptr)
			{
			StopListening(itsXMErrorData);
			}
		itsXPErrorData = const_cast< JArray<JFloat>* >(&xErr);
		itsXMErrorData = nullptr;
		ClearWhenGoingAway(itsXPErrorData, &itsXPErrorData);
		}
	else
		{
		jdelete itsXMErrorData;
		itsXMErrorData = nullptr;

		jdelete itsXPErrorData;
		itsXPErrorData = jnew JArray<JFloat>(xErr);
		assert( itsXPErrorData != nullptr );
		}

	BroadcastCurveChanged();
	return kJTrue;
}

/*********************************************************************************
 SetXErrors

 ********************************************************************************/

JBoolean
J2DPlotData::SetXErrors
	(
	const JArray<JFloat>& xPErr,
	const JArray<JFloat>& xMErr
	)
{
	if ((xPErr.GetElementCount() != GetElementCount()) ||
		(xMErr.GetElementCount() != GetElementCount()))
		{
		return kJFalse;
		}

	if (itsIsListeningFlag)
		{
		if (itsXPErrorData != nullptr)
			{
			StopListening(itsXPErrorData);
			}
		if (itsXMErrorData != nullptr)
			{
			StopListening(itsXMErrorData);
			}
		itsXPErrorData = const_cast< JArray<JFloat>* >(&xPErr);
		itsXMErrorData = const_cast< JArray<JFloat>* >(&xMErr);
		ClearWhenGoingAway(itsXPErrorData, &itsXPErrorData);
		ClearWhenGoingAway(itsXMErrorData, &itsXMErrorData);
		}
	else
		{
		jdelete itsXPErrorData;
		itsXPErrorData = jnew JArray<JFloat>(xPErr);
		assert( itsXPErrorData != nullptr );

		jdelete itsXMErrorData;
		itsXMErrorData = jnew JArray<JFloat>(xMErr);
		assert( itsXMErrorData != nullptr );
		}

	BroadcastCurveChanged();
	return kJTrue;
}

/*********************************************************************************
 SetYErrors

 ********************************************************************************/

JBoolean
J2DPlotData::SetYErrors
	(
	const JArray<JFloat>& yErr
	)
{
	if (yErr.GetElementCount() != GetElementCount())
		{
		return kJFalse;
		}

	if (itsIsListeningFlag)
		{
		if (itsYPErrorData != nullptr)
			{
			StopListening(itsYPErrorData);
			}
		if (itsYMErrorData != nullptr)
			{
			StopListening(itsYMErrorData);
			}
		itsYPErrorData = const_cast< JArray<JFloat>* >(&yErr);
		itsYMErrorData = nullptr;
		ClearWhenGoingAway(itsYPErrorData, &itsYPErrorData);
		}
	else
		{
		jdelete itsYMErrorData;
		itsYMErrorData = nullptr;

		jdelete itsYPErrorData;
		itsYPErrorData = jnew JArray<JFloat>(yErr);
		assert( itsYPErrorData != nullptr );
		}

	BroadcastCurveChanged();
	return kJTrue;
}

/*********************************************************************************
 SetYErrors

 ********************************************************************************/

JBoolean
J2DPlotData::SetYErrors
	(
	const JArray<JFloat>& yPErr,
	const JArray<JFloat>& yMErr
	)
{
	if ((yPErr.GetElementCount() != GetElementCount()) ||
		(yMErr.GetElementCount() != GetElementCount()))
		{
		return kJFalse;
		}

	if (itsIsListeningFlag)
		{
		if (itsYPErrorData != nullptr)
			{
			StopListening(itsYPErrorData);
			}
		if (itsYMErrorData != nullptr)
			{
			StopListening(itsYMErrorData);
			}
		itsYPErrorData = const_cast< JArray<JFloat>* >(&yPErr);
		itsYMErrorData = const_cast< JArray<JFloat>* >(&yMErr);
		ClearWhenGoingAway(itsYPErrorData, &itsYPErrorData);
		ClearWhenGoingAway(itsYMErrorData, &itsYMErrorData);
		}
	else
		{
		jdelete itsYPErrorData;
		itsYPErrorData = jnew JArray<JFloat>(yPErr);
		assert( itsYPErrorData != nullptr );

		jdelete itsYMErrorData;
		itsYMErrorData = jnew JArray<JFloat>(yMErr);
		assert( itsYMErrorData != nullptr );
		}

	BroadcastCurveChanged();
	return kJTrue;
}

/*********************************************************************************
 Receive

 ********************************************************************************/

void
J2DPlotData::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsXData ||
		sender == itsYData ||
		sender == itsXPErrorData ||
		sender == itsXMErrorData ||
		sender == itsYPErrorData ||
		sender == itsYMErrorData)
		{
		if (message.Is(JListT::kElementChanged) ||
			message.Is(JListT::kElementMoved) )
			{
			BroadcastCurveChanged();
			}
		else if (message.Is(JListT::kElementsInserted) ||
				 message.Is(JListT::kElementsRemoved) )
			{
			ValidateCurve();
			BroadcastCurveChanged();
			}
		}
	else
		{
		JPlotDataBase::Receive(sender, message);
		}
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

 ******************************************************************************/

void
J2DPlotData::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (sender == itsXData || sender == itsYData)
		{
		itsXData = nullptr;
		itsYData = nullptr;
		ValidateCurve();
		}
	else
		{
		JPlotDataBase::ReceiveGoingAway(sender);
		}
}

/*********************************************************************************
 ValidateCurve

 ********************************************************************************/

void
J2DPlotData::ValidateCurve()
{
	if (itsXData == nullptr || itsYData == nullptr)
		{
		itsIsValidFlag = kJFalse;
		SetElementCount(0);
		return;
		}

	const JSize xCount = itsXData->GetElementCount();
	const JSize yCount = itsYData->GetElementCount();
	itsIsValidFlag = JI2B(xCount == yCount);

	if (itsXPErrorData != nullptr &&
		itsXPErrorData->GetElementCount() != xCount)
		{
		itsIsValidFlag = kJFalse;
		}
	if (itsXMErrorData != nullptr &&
		itsXMErrorData->GetElementCount() != xCount)
		{
		itsIsValidFlag = kJFalse;
		}

	if (itsYPErrorData != nullptr &&
		itsYPErrorData->GetElementCount() != xCount)
		{
		itsIsValidFlag = kJFalse;
		}
	if (itsYMErrorData != nullptr &&
		itsYMErrorData->GetElementCount() != xCount)
		{
		itsIsValidFlag = kJFalse;
		}

	SetElementCount(itsIsValidFlag ? xCount : 0);
}
