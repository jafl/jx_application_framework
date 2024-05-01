/*********************************************************************************
 J2DVectorData.cpp

	BASE CLASS = J2DPlotDataBase

	Copyright @ 1998 by Glenn W. Bach.

 ********************************************************************************/

#include "J2DVectorData.h"
#include <jx-af/jcore/JMinMax.h>
#include <jx-af/jcore/jAssert.h>

/*********************************************************************************
 Constructor function (static)

 ********************************************************************************/

bool
J2DVectorData::Create
	(
	J2DVectorData**			plotData,
	const JArray<JFloat>&	x,
	const JArray<JFloat>&	y,
	const JArray<JFloat>&	vx,
	const JArray<JFloat>&	vy,
	const bool			listen
	)
{
	if (OKToCreate(x,y, vx,vy))
	{
		*plotData = jnew J2DVectorData(x,y, vx,vy, listen);
		return true;
	}
	else
	{
		*plotData = nullptr;
		return false;
	}
}

/*********************************************************************************
 OKToCreate (static)

 ********************************************************************************/

bool
J2DVectorData::OKToCreate
	(
	const JArray<JFloat>& x,
	const JArray<JFloat>& y,
	const JArray<JFloat>& vx,
	const JArray<JFloat>& vy
	)
{
	return (x.GetItemCount()  == y.GetItemCount() &&
			y.GetItemCount()  == vx.GetItemCount() &&
			vx.GetItemCount() == vy.GetItemCount());
}

/*********************************************************************************
 Constructor

 ********************************************************************************/

J2DVectorData::J2DVectorData()
	:
	J2DPlotDataBase(kVectorPlot)
{
	J2DVectorDataX();

	itsXData  = jnew JArray<JFloat>(100);
	itsYData  = jnew JArray<JFloat>(100);
	itsVXData = jnew JArray<JFloat>(100);
	itsVYData = jnew JArray<JFloat>(100);

	itsIsListeningFlag = false;
}

// protected

J2DVectorData::J2DVectorData
	(
	const JArray<JFloat>&	x,
	const JArray<JFloat>&	y,
	const JArray<JFloat>&	vx,
	const JArray<JFloat>&	vy,
	const bool			listen
	)
	:
	J2DPlotDataBase(kVectorPlot)
{
	assert( OKToCreate(x,y, vx,vy) );

	J2DVectorDataX();

	const JSize count = x.GetItemCount();
	SetItemCount(count);

	itsIsListeningFlag = listen;
	if (listen)
	{
		itsXData = const_cast< JArray<JFloat>* >(&x);
		ListenTo(itsXData);

		itsYData = const_cast< JArray<JFloat>* >(&y);
		ListenTo(itsYData);

		itsVXData = const_cast< JArray<JFloat>* >(&vx);
		ListenTo(itsVXData);

		itsVYData = const_cast< JArray<JFloat>* >(&vy);
		ListenTo(itsVYData);
	}
	else
	{
		itsXData  = jnew JArray<JFloat>(x);
		itsYData  = jnew JArray<JFloat>(y);
		itsVXData = jnew JArray<JFloat>(vx);
		itsVYData = jnew JArray<JFloat>(vy);
	}
}

// private

void
J2DVectorData::J2DVectorDataX()
{
	itsIsValidFlag = true;

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
 GetItem (virtual)

 ********************************************************************************/

void
J2DVectorData::GetItem
	(
	const JIndex	index,
	J2DDataPoint*	data
	)
	const
{
	data->x = itsXData->GetItem(index);
	data->y = itsYData->GetItem(index);
}

/*********************************************************************************
 GetItem (virtual)

 ********************************************************************************/

void
J2DVectorData::GetItem
	(
	const JIndex	index,
	J2DVectorPoint*	data
	)
	const
{
	data->x  = itsXData->GetItem(index);
	data->y  = itsYData->GetItem(index);
	data->vx = itsVXData->GetItem(index);
	data->vy = itsVYData->GetItem(index);
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

		const JSize count = itsXData->GetItemCount();
		for (JIndex i=1; i<=count; i++)
		{
			const JFloat x1 = itsXData->GetItem(i);
			const JFloat x2 = x1 + itsVXData->GetItem(i);

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

bool
J2DVectorData::GetYRange
	(
	const JFloat	xMin,
	const JFloat	xMax,
	const bool	xLinear,
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

		const JSize count = itsYData->GetItemCount();
		for (JIndex i=1; i<=count; i++)
		{
			const JFloat y1 = itsYData->GetItem(i);
			const JFloat y2 = y1 + itsVYData->GetItem(i);

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

	return true;
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
	itsXData->AppendItem(x);
	itsYData->AppendItem(y);
	itsVXData->AppendItem(vx);
	itsVYData->AppendItem(vy);
	ItemAdded();
	BroadcastCurveChanged();
}

/*********************************************************************************
 RemoveItem

 ********************************************************************************/

void
J2DVectorData::RemoveItem
	(
	const JIndex index
	)
{
	IgnoreDataChanges();
	itsXData->RemoveItem(index);
	itsYData->RemoveItem(index);
	itsVXData->RemoveItem(index);
	itsVYData->RemoveItem(index);
	ItemRemoved();
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

		StopListening(itsYData);
		itsYData = jnew JArray<JFloat>(*itsYData);

		StopListening(itsVXData);
		itsVXData = jnew JArray<JFloat>(*itsVXData);

		StopListening(itsVYData);
		itsVYData = jnew JArray<JFloat>(*itsVYData);

		itsIsListeningFlag = false;
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
		if (message.Is(JListT::kItemsChanged) ||
			message.Is(JListT::kItemMoved) )
		{
			BroadcastCurveChanged();
		}
		else if (message.Is(JListT::kItemsInserted) ||
				 message.Is(JListT::kItemsRemoved) )
		{
			ValidateData();
			BroadcastCurveChanged();
		}
	}
	else
	{
		J2DPlotDataBase::Receive(sender, message);
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
		itsXData  = nullptr;
		itsYData  = nullptr;
		itsVXData = nullptr;
		itsVYData = nullptr;
		ValidateData();
	}
	else
	{
		J2DPlotDataBase::ReceiveGoingAway(sender);
	}
}

/*********************************************************************************
 ValidateData (private)

 ********************************************************************************/

void
J2DVectorData::ValidateData()
{
	itsIsValidFlag = itsXData != nullptr &&
		OKToCreate(*itsXData, *itsYData, *itsVXData, *itsVYData);

	SetItemCount(itsIsValidFlag ? itsXData->GetItemCount() : 0);
}
