/*********************************************************************************
 J2DVectorData.cpp

	BASE CLASS = JPlotDataBase

	Copyright @ 1998 by Glenn W. Bach. All rights reserved.

 ********************************************************************************/

#include <J2DVectorData.h>
#include <JMinMax.h>
#include <jAssert.h>

/*********************************************************************************
 Constructor function (static)

 ********************************************************************************/

JBoolean
J2DVectorData::Create
	(
	J2DVectorData**			plotData,
	const JArray<JFloat>&	x,
	const JArray<JFloat>&	y,
	const JArray<JFloat>&	vx,
	const JArray<JFloat>&	vy,
	const JBoolean			listen
	)
{
	if (OKToCreate(x,y, vx,vy))
		{
		*plotData = jnew J2DVectorData(x,y, vx,vy, listen);
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
 OKToCreate (static)

 ********************************************************************************/

JBoolean
J2DVectorData::OKToCreate
	(
	const JArray<JFloat>& x,
	const JArray<JFloat>& y,
	const JArray<JFloat>& vx,
	const JArray<JFloat>& vy
	)
{
	return JI2B(x.GetElementCount()  == y.GetElementCount() &&
				y.GetElementCount()  == vx.GetElementCount() &&
				vx.GetElementCount() == vy.GetElementCount());
}

/*********************************************************************************
 Constructor

 ********************************************************************************/

J2DVectorData::J2DVectorData()
	:
	JPlotDataBase(kVectorPlot)
{
	J2DVectorDataX();

	itsXData = jnew JArray<JFloat>(100);
	assert( itsXData != NULL );
	itsYData = jnew JArray<JFloat>(100);
	assert( itsYData != NULL );
	itsVXData = jnew JArray<JFloat>(100);
	assert( itsVXData != NULL );
	itsVYData = jnew JArray<JFloat>(100);
	assert( itsVYData != NULL );

	itsIsListeningFlag = kJFalse;
}

// protected

J2DVectorData::J2DVectorData
	(
	const JArray<JFloat>&	x,
	const JArray<JFloat>&	y,
	const JArray<JFloat>&	vx,
	const JArray<JFloat>&	vy,
	const JBoolean			listen
	)
	:
	JPlotDataBase(kVectorPlot)
{
	assert( OKToCreate(x,y, vx,vy) );

	J2DVectorDataX();

	const JSize count = x.GetElementCount();
	SetElementCount(count);

	itsIsListeningFlag = listen;
	if (listen)
		{
		itsXData = const_cast< JArray<JFloat>* >(&x);
		assert( itsXData != NULL );
		ListenTo(itsXData);

		itsYData = const_cast< JArray<JFloat>* >(&y);
		assert( itsYData != NULL );
		ListenTo(itsYData);

		itsVXData = const_cast< JArray<JFloat>* >(&vx);
		assert( itsVXData != NULL );
		ListenTo(itsVXData);

		itsVYData = const_cast< JArray<JFloat>* >(&vy);
		assert( itsVYData != NULL );
		ListenTo(itsVYData);
		}
	else
		{
		itsXData = jnew JArray<JFloat>(x);
		assert( itsXData != NULL );
		itsYData = jnew JArray<JFloat>(y);
		assert( itsYData != NULL );
		itsVXData = jnew JArray<JFloat>(vx);
		assert( itsVXData != NULL );
		itsVYData = jnew JArray<JFloat>(vy);
		assert( itsVYData != NULL );
		}
}

// private

void
J2DVectorData::J2DVectorDataX()
{
	itsIsValidFlag = kJTrue;

	itsCurrentXMin = 0;
	itsCurrentXMax = 0;
	itsCurrentYMin = 0;
	itsCurrentYMax = 0;
}

/*********************************************************************************
 Destructor

 ********************************************************************************/

J2DVectorData::~J2DVectorData()
{
	if (!itsIsListeningFlag)
		{
		jdelete itsXData;
		jdelete itsYData;
		jdelete itsVXData;
		jdelete itsVYData;
		}
}

/*********************************************************************************
 GetElement (virtual)

 ********************************************************************************/

void
J2DVectorData::GetElement
	(
	const JIndex	index,
	J2DDataPoint*	data
	)
	const
{
	data->x = itsXData->GetElement(index);
	data->y = itsYData->GetElement(index);
}

/*********************************************************************************
 GetElement (virtual)

 ********************************************************************************/

void
J2DVectorData::GetElement
	(
	const JIndex	index,
	J2DVectorPoint*	data
	)
	const
{
	data->x  = itsXData->GetElement(index);
	data->y  = itsYData->GetElement(index);
	data->vx = itsVXData->GetElement(index);
	data->vy = itsVYData->GetElement(index);
}

/*********************************************************************************
 GetXRange (virtual)

 ********************************************************************************/

void
J2DVectorData::GetXRange
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

		const JSize count = itsXData->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			const JFloat x1 = itsXData->GetElement(i);
			const JFloat x2 = x1 + itsVXData->GetElement(i);

			if (i == 1)
				{
				*min = JMin(x1, x2);
				*max = JMax(x1, x2);
				}

			if (x1 < *min)
				{
				*min = x1;
				}
			if (x2 < *min)
				{
				*min = x2;
				}

			if (x1 > *max)
				{
				*max = x1;
				}
			if (x2 > *max)
				{
				*max = x2;
				}
			}

		itsCurrentXMin = *min;
		itsCurrentXMax = *max;
		}
}

/*********************************************************************************
 GetYRange (virtual)

 ********************************************************************************/

JBoolean
J2DVectorData::GetYRange
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

		const JSize count = itsYData->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			const JFloat y1 = itsYData->GetElement(i);
			const JFloat y2 = y1 + itsVYData->GetElement(i);

			if (i == 1)
				{
				*yMin = JMin(y1, y2);
				*yMax = JMax(y1, y2);
				}

			if (y1 < *yMin)
				{
				*yMin = y1;
				}
			if (y2 < *yMin)
				{
				*yMin = y2;
				}

			if (y1 > *yMax)
				{
				*yMax = y1;
				}
			if (y2 > *yMax)
				{
				*yMax = y2;
				}
			}

		itsCurrentYMin = *yMin;
		itsCurrentYMax = *yMax;
		}

	return kJTrue;
}

/*********************************************************************************
 AddElement

 ********************************************************************************/

void
J2DVectorData::AddElement
	(
	const JFloat x,
	const JFloat y,
	const JFloat vx,
	const JFloat vy
	)
{
	IgnoreDataChanges();
	itsXData->AppendElement(x);
	itsYData->AppendElement(y);
	itsVXData->AppendElement(vx);
	itsVYData->AppendElement(vy);
	ElementAdded();
	BroadcastCurveChanged();
}

/*********************************************************************************
 RemoveElement

 ********************************************************************************/

void
J2DVectorData::RemoveElement
	(
	const JIndex index
	)
{
	IgnoreDataChanges();
	itsXData->RemoveElement(index);
	itsYData->RemoveElement(index);
	itsVXData->RemoveElement(index);
	itsVYData->RemoveElement(index);
	ElementRemoved();
	BroadcastCurveChanged();
}

/*********************************************************************************
 IgnoreDataChanges

 ********************************************************************************/

void
J2DVectorData::IgnoreDataChanges()
{
	assert( itsIsValidFlag );
	if (itsIsListeningFlag)
		{
		StopListening(itsXData);
		itsXData = jnew JArray<JFloat>(*itsXData);
		assert( itsXData != NULL );

		StopListening(itsYData);
		itsYData = jnew JArray<JFloat>(*itsYData);
		assert( itsYData != NULL );

		StopListening(itsVXData);
		itsVXData = jnew JArray<JFloat>(*itsVXData);
		assert( itsVXData != NULL );

		StopListening(itsVYData);
		itsVYData = jnew JArray<JFloat>(*itsVYData);
		assert( itsVYData != NULL );

		itsIsListeningFlag = kJFalse;
		}
}

/*********************************************************************************
 Receive

 ********************************************************************************/

void
J2DVectorData::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsXData  || sender == itsYData ||
		sender == itsVXData || sender == itsVYData)
		{
		if (message.Is(JOrderedSetT::kElementChanged) ||
			message.Is(JOrderedSetT::kElementMoved) )
			{
			BroadcastCurveChanged();
			}
		else if (message.Is(JOrderedSetT::kElementsInserted) ||
				 message.Is(JOrderedSetT::kElementsRemoved) )
			{
			ValidateData();
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
J2DVectorData::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (sender == itsXData  || sender == itsYData ||
		sender == itsVXData || sender == itsVYData)
		{
		itsXData  = NULL;
		itsYData  = NULL;
		itsVXData = NULL;
		itsVYData = NULL;
		ValidateData();
		}
	else
		{
		JPlotDataBase::ReceiveGoingAway(sender);
		}
}

/*********************************************************************************
 ValidateData (private)

 ********************************************************************************/

void
J2DVectorData::ValidateData()
{
	itsIsValidFlag = JI2B(
		itsXData != NULL &&
		OKToCreate(*itsXData, *itsYData, *itsVXData, *itsVYData));

	SetElementCount(itsIsValidFlag ? itsXData->GetElementCount() : 0);
}
