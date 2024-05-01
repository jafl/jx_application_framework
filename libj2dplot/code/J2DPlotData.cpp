/*********************************************************************************
 J2DPlotData.cpp

	J2DPlotData class.

	Copyright @ 1997 by Glenn W. Bach.

 ********************************************************************************/

#include "J2DPlotData.h"
#include <jx-af/jcore/jAssert.h>

/*********************************************************************************
 Create

 ********************************************************************************/

bool
J2DPlotData::Create
	(
	J2DPlotData**			plotData,
	const JArray<JFloat>&	x,
	const JArray<JFloat>&	y,
	const bool				listen
	)
{
	if (OKToCreate(x,y))
	{
		*plotData = jnew J2DPlotData(x,y,listen);
		return true;
	}
	else
	{
		*plotData = nullptr;
		return false;
	}
}

/*********************************************************************************
 OKToCreate

 ********************************************************************************/

bool
J2DPlotData::OKToCreate
	(
	const JArray<JFloat>& x,
	const JArray<JFloat>& y
	)
{
	return x.GetItemCount() == y.GetItemCount();
}

/*********************************************************************************
 Constructor (protected)

 ********************************************************************************/

J2DPlotData::J2DPlotData
	(
	const JArray<JFloat>&	x,
	const JArray<JFloat>&	y,
	const bool				listen
	)
	:
	J2DPlotDataBase(kScatterPlot)
{
	assert( OKToCreate(x,y) );

	SetItemCount(x.GetItemCount());
	assert(y.GetItemCount() == x.GetItemCount());

	if (listen)
	{
		itsXData = const_cast< JArray<JFloat>* >(&x);
		ListenTo(itsXData);

		itsYData = const_cast< JArray<JFloat>* >(&y);
		ListenTo(itsYData);
	}
	else
	{
		itsXData = jnew JArray<JFloat>(x);
		itsYData = jnew JArray<JFloat>(y);
	}

	itsXPErrorData = nullptr;
	itsXMErrorData = nullptr;
	itsYPErrorData = nullptr;
	itsYMErrorData = nullptr;

	itsIsValidFlag     = true;
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
 GetItem

 ********************************************************************************/

void
J2DPlotData::GetItem
	(
	const JIndex	index,
	J2DDataPoint*	data
	)
	const
{
	data->x = itsXData->GetItem(index);
	data->y = itsYData->GetItem(index);

	if (itsXPErrorData != nullptr)
	{
		data->xerr = itsXPErrorData->GetItem(index);
		if (itsXMErrorData != nullptr)
		{
			data->xmerr = itsXMErrorData->GetItem(index);
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
		data->yerr = itsYPErrorData->GetItem(index);
		if (itsYMErrorData != nullptr)
		{
			data->ymerr = itsYMErrorData->GetItem(index);
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
		const JSize count = GetItemCount();
		for (JIndex i=1; i<=count; i++)
		{
			GetItem(i, &pt);
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

bool
J2DPlotData::GetYRange
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

		J2DDataPoint pt;
		const JSize count = GetItemCount();
		for (JIndex i=1; i<=count; i++)
		{
			GetItem(i, &pt);
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

	return true;
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

		if (itsXPErrorData != nullptr)
		{
			StopListening(itsXPErrorData);
			itsXPErrorData = jnew JArray<JFloat>(*itsXPErrorData);
		}
		if (itsXMErrorData != nullptr)
		{
			StopListening(itsXMErrorData);
			itsXMErrorData = jnew JArray<JFloat>(*itsXMErrorData);
		}

		StopListening(itsYData);
		itsYData = jnew JArray<JFloat>(*itsYData);

		if (itsYPErrorData != nullptr)
		{
			StopListening(itsYPErrorData);
			itsYPErrorData = jnew JArray<JFloat>(*itsYPErrorData);
		}
		if (itsYMErrorData != nullptr)
		{
			StopListening(itsYMErrorData);
			itsYMErrorData = jnew JArray<JFloat>(*itsYMErrorData);
		}

		itsIsListeningFlag = false;
	}
}

/*********************************************************************************
 SetXErrors

 ********************************************************************************/

bool
J2DPlotData::SetXErrors
	(
	const JArray<JFloat>& xErr
	)
{
	if (xErr.GetItemCount() != GetItemCount())
	{
		return false;
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
	}

	BroadcastCurveChanged();
	return true;
}

/*********************************************************************************
 SetXErrors

 ********************************************************************************/

bool
J2DPlotData::SetXErrors
	(
	const JArray<JFloat>& xPErr,
	const JArray<JFloat>& xMErr
	)
{
	if ((xPErr.GetItemCount() != GetItemCount()) ||
		(xMErr.GetItemCount() != GetItemCount()))
	{
		return false;
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

		jdelete itsXMErrorData;
		itsXMErrorData = jnew JArray<JFloat>(xMErr);
	}

	BroadcastCurveChanged();
	return true;
}

/*********************************************************************************
 SetYErrors

 ********************************************************************************/

bool
J2DPlotData::SetYErrors
	(
	const JArray<JFloat>& yErr
	)
{
	if (yErr.GetItemCount() != GetItemCount())
	{
		return false;
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
	}

	BroadcastCurveChanged();
	return true;
}

/*********************************************************************************
 SetYErrors

 ********************************************************************************/

bool
J2DPlotData::SetYErrors
	(
	const JArray<JFloat>& yPErr,
	const JArray<JFloat>& yMErr
	)
{
	if ((yPErr.GetItemCount() != GetItemCount()) ||
		(yMErr.GetItemCount() != GetItemCount()))
	{
		return false;
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

		jdelete itsYMErrorData;
		itsYMErrorData = jnew JArray<JFloat>(yMErr);
	}

	BroadcastCurveChanged();
	return true;
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
		if (message.Is(JListT::kItemsChanged) ||
			message.Is(JListT::kItemMoved) )
		{
			BroadcastCurveChanged();
		}
		else if (message.Is(JListT::kItemsInserted) ||
				 message.Is(JListT::kItemsRemoved) )
		{
			ValidateCurve();
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
		J2DPlotDataBase::ReceiveGoingAway(sender);
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
		itsIsValidFlag = false;
		SetItemCount(0);
		return;
	}

	const JSize xCount = itsXData->GetItemCount();
	const JSize yCount = itsYData->GetItemCount();
	itsIsValidFlag = xCount == yCount;

	if (itsXPErrorData != nullptr &&
		itsXPErrorData->GetItemCount() != xCount)
	{
		itsIsValidFlag = false;
	}
	if (itsXMErrorData != nullptr &&
		itsXMErrorData->GetItemCount() != xCount)
	{
		itsIsValidFlag = false;
	}

	if (itsYPErrorData != nullptr &&
		itsYPErrorData->GetItemCount() != xCount)
	{
		itsIsValidFlag = false;
	}
	if (itsYMErrorData != nullptr &&
		itsYMErrorData->GetItemCount() != xCount)
	{
		itsIsValidFlag = false;
	}

	SetItemCount(itsIsValidFlag ? xCount : 0);
}
