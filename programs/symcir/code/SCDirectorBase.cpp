/******************************************************************************
 SCDirectorBase.cpp

	Base class for analysis windows.

	BASE CLASS = JXWindowDirector

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <scStdInc.h>
#include "SCDirectorBase.h"
#include "SCCircuitDocument.h"
#include "SCXferFnDirector.h"
#include "SCZinDirector.h"
#include "SCEETDirector.h"
#include "SCFeedbackDirector.h"
#include "SCNDIDirector.h"
#include "SCScratchDirector.h"
#include <JXWindow.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SCDirectorBase::SCDirectorBase
	(
	SCCircuitDocument* supervisor
	)
	:
	JXWindowDirector(supervisor)
{
	SCDirectorBaseX(supervisor);
}

SCDirectorBase::SCDirectorBase
	(
	istream&			input,
	const JFileVersion	vers,
	SCCircuitDocument*	supervisor
	)
	:
	JXWindowDirector(supervisor)
{
	SCDirectorBaseX(supervisor);
}

// private

void
SCDirectorBase::SCDirectorBaseX
	(
	SCCircuitDocument* doc
	)
{
	itsDoc = doc;
	itsDoc->ExprCreated(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SCDirectorBase::~SCDirectorBase()
{
}

/******************************************************************************
 GetCircuit

 ******************************************************************************/

const SCCircuit*
SCDirectorBase::GetCircuit()
	const
{
	return itsDoc->GetCircuit();
}

/******************************************************************************
 GetVarList

 ******************************************************************************/

const SCCircuitVarList*
SCDirectorBase::GetVarList()
	const
{
	return itsDoc->GetVarList();
}

/******************************************************************************
 StreamIn (static)

	Read the type of director and create the appropriate object.

 ******************************************************************************/

SCDirectorBase*
SCDirectorBase::StreamIn
	(
	istream&			input,
	const JFileVersion	vers,
	SCCircuitDocument*	supervisor
	)
{
	Type type;
	input >> type;

	SCDirectorBase* dir = NULL;
	if (type == kXferFnType)
		{
		dir = new SCXferFnDirector(input, vers, supervisor);
		}
	else if (type == kZinType)
		{
		dir = new SCZinDirector(input, vers, supervisor);
		}
	else if (type == kEETType)
		{
		dir = new SCEETDirector(input, vers, supervisor);
		}
	else if (type == kFeedbackType)
		{
		dir = new SCFeedbackDirector(input, vers, supervisor);
		}
	else if (type == kNDIType)
		{
		dir = new SCNDIDirector(input, vers, supervisor);
		}
	else if (type == kScratchType)
		{
		dir = new SCScratchDirector(input, vers, supervisor);
		}
	else
		{
		assert( 0 );	// this should never happen
		}
	assert( dir != NULL );

	return dir;
}

/******************************************************************************
 Global functions for SCDirectorBase::Type

 ******************************************************************************/

istream&
operator>>
	(
	istream&				input,
	SCDirectorBase::Type&	type
	)
{
	long temp;
	input >> temp;

	// we don't check it here because SCDirectorBase::StreamIn will check it anyway

	type = (SCDirectorBase::Type) temp;
	return input;
}

ostream&
operator<<
	(
	ostream&					output,
	const SCDirectorBase::Type	type
	)
{
	output << (long) type;
	return output;
}
