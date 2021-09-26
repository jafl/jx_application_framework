/*********************************************************************************
 J2DPlotDataBase.cpp

	J2DPlotDataBase class.

	Copyright @ 1997 by Glenn W. Bach.

 ********************************************************************************/

#include "jx-af/j2dplot/J2DPlotDataBase.h"
#include <jx-af/jcore/jAssert.h>

const JUtf8Byte* J2DPlotDataBase::kCurveChanged = "CurveChanged::J2DPlotDataBase";

/*********************************************************************************
 Constructor

 ********************************************************************************/

J2DPlotDataBase::J2DPlotDataBase
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

J2DPlotDataBase::~J2DPlotDataBase()
{
}

/*********************************************************************************
 GetElement (virtual)

 ********************************************************************************/

void
J2DPlotDataBase::GetElement
	(
	const JIndex	index,
	J2DDataPoint*	data
	)
	const
{
	assert_msg( 0, "The programmer forgot to override J2DPlotDataBase::GetElement(J2DDataPoint)" );
}

/*********************************************************************************
 GetElement (virtual)

 ********************************************************************************/

void
J2DPlotDataBase::GetElement
	(
	const JIndex	index,
	J2DVectorPoint*	data
	)
	const
{
	assert_msg( 0, "The programmer forgot to override J2DPlotDataBase::GetElement(J2DVectorPoint)" );
}

/*********************************************************************************
 HasXErrors (virtual)

 ********************************************************************************/

bool
J2DPlotDataBase::HasXErrors()
	const
{
	return false;
}

/*********************************************************************************
 HasYErrors (virtual)

 ********************************************************************************/

bool
J2DPlotDataBase::HasYErrors()
	const
{
	return false;
}

/*********************************************************************************
 HasSymmetricXErrors (virtual)

 ********************************************************************************/

bool
J2DPlotDataBase::HasSymmetricXErrors()
	const
{
	return false;
}

/*********************************************************************************
 HasSymmetricYErrors (virtual)

 ********************************************************************************/

bool
J2DPlotDataBase::HasSymmetricYErrors()
	const
{
	return false;
}

/*********************************************************************************
 IsFunction (virtual)

 ********************************************************************************/

bool
J2DPlotDataBase::IsFunction()
	const
{
	return false;
}
