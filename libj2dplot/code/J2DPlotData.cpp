/*********************************************************************************
 J2DPlotData.cpp

	J2DPlotData class.

	Copyright @ 1997 by Glenn W. Bach. All rights reserved.

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
		*plotData = new J2DPlotData(x,y,listen);
		assert( *plotData != NULL );
		return kJTrue;
		}
	else
		{
		*plotData = NULL;
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
		assert( itsXData != NULL );
		ListenTo(itsXData);

		itsYData = const_cast< JArray<JFloat>* >(&y);
		assert( itsYData != NULL );
		ListenTo(itsYData);
		}
	else
		{
		itsXData = new JArray<JFloat>(x);
		assert( itsXData != NULL );
		itsYData = new JArray<JFloat>(y);
		assert( itsYData != NULL );
		}

	itsXPErrorData = NULL;
	itsXMErrorData = NULL;
	itsYPErrorData = NULL;
	itsYMErrorData = NULL;

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
		delete itsXData;
		delete itsXPErrorData;
		delete itsXMErrorData;
		delete itsYData;
		delete itsYPErrorData;
		delete itsYMErrorData;
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

	if (itsXPErrorData != NULL)
		{
		data->xerr = itsXPErrorData->GetElement(index);
		if (itsXMErrorData != NULL)
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

	if (itsYPErrorData != NULL)
		{
		data->yerr = itsYPErrorData->GetElement(index);
		if (itsYMErrorData != NULL)
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
		itsXData = new JArray<JFloat>(*itsXData);
		assert( itsXData != NULL );

		if (itsXPErrorData != NULL)
			{
			StopListening(itsXPErrorData);
			itsXPErrorData = new JArray<JFloat>(*itsXPErrorData);
			assert( itsXPErrorData != NULL );
			}
		if (itsXMErrorData != NULL)
			{
			StopListening(itsXMErrorData);
			itsXMErrorData = new JArray<JFloat>(*itsXMErrorData);
			assert( itsXMErrorData != NULL );
			}

		StopListening(itsYData);
		itsYData = new JArray<JFloat>(*itsYData);
		assert( itsYData != NULL );

		if (itsYPErrorData != NULL)
			{
			StopListening(itsYPErrorData);
			itsYPErrorData = new JArray<JFloat>(*itsYPErrorData);
			assert( itsYPErrorData != NULL );
			}
		if (itsYMErrorData != NULL)
			{
			StopListening(itsYMErrorData);
			itsYMErrorData = new JArray<JFloat>(*itsYMErrorData);
			assert( itsYMErrorData != NULL );
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
		if (itsXPErrorData != NULL)
			{
			StopListening(itsXPErrorData);
			}
		if (itsXMErrorData != NULL)
			{
			StopListening(itsXMErrorData);
			}
		itsXPErrorData = const_cast< JArray<JFloat>* >(&xErr);
		itsXMErrorData = NULL;
		ClearWhenGoingAway(itsXPErrorData, &itsXPErrorData);
		}
	else
		{
		delete itsXMErrorData;
		itsXMErrorData = NULL;

		delete itsXPErrorData;
		itsXPErrorData = new JArray<JFloat>(xErr);
		assert( itsXPErrorData != NULL );
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
		if (itsXPErrorData != NULL)
			{
			StopListening(itsXPErrorData);
			}
		if (itsXMErrorData != NULL)
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
		delete itsXPErrorData;
		itsXPErrorData = new JArray<JFloat>(xPErr);
		assert( itsXPErrorData != NULL );

		delete itsXMErrorData;
		itsXMErrorData = new JArray<JFloat>(xMErr);
		assert( itsXMErrorData != NULL );
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
		if (itsYPErrorData != NULL)
			{
			StopListening(itsYPErrorData);
			}
		if (itsYMErrorData != NULL)
			{
			StopListening(itsYMErrorData);
			}
		itsYPErrorData = const_cast< JArray<JFloat>* >(&yErr);
		itsYMErrorData = NULL;
		ClearWhenGoingAway(itsYPErrorData, &itsYPErrorData);
		}
	else
		{
		delete itsYMErrorData;
		itsYMErrorData = NULL;

		delete itsYPErrorData;
		itsYPErrorData = new JArray<JFloat>(yErr);
		assert( itsYPErrorData != NULL );
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
		if (itsYPErrorData != NULL)
			{
			StopListening(itsYPErrorData);
			}
		if (itsYMErrorData != NULL)
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
		delete itsYPErrorData;
		itsYPErrorData = new JArray<JFloat>(yPErr);
		assert( itsYPErrorData != NULL );

		delete itsYMErrorData;
		itsYMErrorData = new JArray<JFloat>(yMErr);
		assert( itsYMErrorData != NULL );
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
		if (message.Is(JOrderedSetT::kElementChanged) ||
			message.Is(JOrderedSetT::kElementMoved) )
			{
			BroadcastCurveChanged();
			}
		else if (message.Is(JOrderedSetT::kElementsInserted) ||
				 message.Is(JOrderedSetT::kElementsRemoved) )
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
		itsXData = NULL;
		itsYData = NULL;
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
	if (itsXData == NULL || itsYData == NULL)
		{
		itsIsValidFlag = kJFalse;
		SetElementCount(0);
		return;
		}

	const JSize xCount = itsXData->GetElementCount();
	const JSize yCount = itsYData->GetElementCount();
	itsIsValidFlag = JI2B(xCount == yCount);

	if (itsXPErrorData != NULL &&
		itsXPErrorData->GetElementCount() != xCount)
		{
		itsIsValidFlag = kJFalse;
		}
	if (itsXMErrorData != NULL &&
		itsXMErrorData->GetElementCount() != xCount)
		{
		itsIsValidFlag = kJFalse;
		}

	if (itsYPErrorData != NULL &&
		itsYPErrorData->GetElementCount() != xCount)
		{
		itsIsValidFlag = kJFalse;
		}
	if (itsYMErrorData != NULL &&
		itsYMErrorData->GetElementCount() != xCount)
		{
		itsIsValidFlag = kJFalse;
		}

	SetElementCount(itsIsValidFlag ? xCount : 0);
}
