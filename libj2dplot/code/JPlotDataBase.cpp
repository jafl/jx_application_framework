/*********************************************************************************
 JPlotDataBase.cpp

	JPlotDataBase class.

	Copyright @ 1997 by Glenn W. Bach. All rights reserved.

 ********************************************************************************/

#include <JCoreStdInc.h>
#include <JPlotDataBase.h>
#include <jAssert.h>

const JCharacter* JPlotDataBase::kCurveChanged = "CurveChanged::JPlotDataBase";

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
	itsBroadcastFlag(kJTrue)
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
	assert( 0 /* The programmer forgot to override JPlotDataBase::GetElement(J2DDataPoint) */ );
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
	assert( 0 /* The programmer forgot to override JPlotDataBase::GetElement(J2DVectorPoint) */ );
}

/*********************************************************************************
 GetZRange (virtual)

 ********************************************************************************/

JBoolean
JPlotDataBase::GetZRange
	(
	const JFloat	xMin,
	const JFloat	xMax,
	const JBoolean	xLinear,
	const JFloat	yMin,
	const JFloat	yMax,
	const JBoolean	yLinear,
	JFloat*			zMin,
	JFloat*			zMax
	)
	const
{
	return kJFalse;
}

/*********************************************************************************
 Get4thRange (virtual)

 ********************************************************************************/

JBoolean
JPlotDataBase::Get4thRange
	(
	const JFloat	xMin,
	const JFloat	xMax,
	const JBoolean	xLinear,
	const JFloat	yMin,
	const JFloat	yMax,
	const JBoolean	yLinear,
	const JFloat	zMin,
	const JFloat	zMax,
	const JBoolean	zLinear,
	JFloat*			min,
	JFloat*			max
	)
	const
{
	return kJFalse;
}

/*********************************************************************************
 HasXErrors (virtual)

 ********************************************************************************/

JBoolean
JPlotDataBase::HasXErrors()
	const
{
	return kJFalse;
}

/*********************************************************************************
 HasYErrors (virtual)

 ********************************************************************************/

JBoolean
JPlotDataBase::HasYErrors()
	const
{
	return kJFalse;
}

/*********************************************************************************
 HasSymmetricXErrors (virtual)

 ********************************************************************************/

JBoolean
JPlotDataBase::HasSymmetricXErrors()
	const
{
	return kJFalse;
}

/*********************************************************************************
 HasSymmetricYErrors (virtual)

 ********************************************************************************/

JBoolean
JPlotDataBase::HasSymmetricYErrors()
	const
{
	return kJFalse;
}

/*********************************************************************************
 IsFunction (virtual)

 ********************************************************************************/

JBoolean
JPlotDataBase::IsFunction()
	const
{
	return kJFalse;
}
