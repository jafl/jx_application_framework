/*********************************************************************************
 JPlotDataBase.cpp

	JPlotDataBase class.

	Copyright @ 1997 by Glenn W. Bach.

 ********************************************************************************/

#include "JPlotDataBase.h"
#include <jAssert.h>

const JUtf8Byte* JPlotDataBase::kCurveChanged = "CurveChanged::JPlotDataBase";

/*********************************************************************************
 Constructor

 ********************************************************************************/

JPlotDataBase::JPlotDataBase
	(
	const Type type
	)
	:
	JCollection(),
	itsType(type),
	itsBroadcastFlag(true)
{
}

/*********************************************************************************
 Destructor

 ********************************************************************************/

JPlotDataBase::~JPlotDataBase()
{
}

/*********************************************************************************
 GetElement (virtual)

 ********************************************************************************/

void
JPlotDataBase::GetElement
	(
	const JIndex	index,
	J2DDataPoint*	data
	)
	const
{
	assert_msg( 0, "The programmer forgot to override JPlotDataBase::GetElement(J2DDataPoint)" );
}

/*********************************************************************************
 GetElement (virtual)

 ********************************************************************************/

void
JPlotDataBase::GetElement
	(
	const JIndex	index,
	J2DVectorPoint*	data
	)
	const
{
	assert_msg( 0, "The programmer forgot to override JPlotDataBase::GetElement(J2DVectorPoint)" );
}

/*********************************************************************************
 GetZRange (virtual)

 ********************************************************************************/

bool
JPlotDataBase::GetZRange
	(
	const JFloat	xMin,
	const JFloat	xMax,
	const bool	xLinear,
	const JFloat	yMin,
	const JFloat	yMax,
	const bool	yLinear,
	JFloat*			zMin,
	JFloat*			zMax
	)
	const
{
	return false;
}

/*********************************************************************************
 Get4thRange (virtual)

 ********************************************************************************/

bool
JPlotDataBase::Get4thRange
	(
	const JFloat	xMin,
	const JFloat	xMax,
	const bool	xLinear,
	const JFloat	yMin,
	const JFloat	yMax,
	const bool	yLinear,
	const JFloat	zMin,
	const JFloat	zMax,
	const bool	zLinear,
	JFloat*			min,
	JFloat*			max
	)
	const
{
	return false;
}

/*********************************************************************************
 HasXErrors (virtual)

 ********************************************************************************/

bool
JPlotDataBase::HasXErrors()
	const
{
	return false;
}

/*********************************************************************************
 HasYErrors (virtual)

 ********************************************************************************/

bool
JPlotDataBase::HasYErrors()
	const
{
	return false;
}

/*********************************************************************************
 HasSymmetricXErrors (virtual)

 ********************************************************************************/

bool
JPlotDataBase::HasSymmetricXErrors()
	const
{
	return false;
}

/*********************************************************************************
 HasSymmetricYErrors (virtual)

 ********************************************************************************/

bool
JPlotDataBase::HasSymmetricYErrors()
	const
{
	return false;
}

/*********************************************************************************
 IsFunction (virtual)

 ********************************************************************************/

bool
JPlotDataBase::IsFunction()
	const
{
	return false;
}
