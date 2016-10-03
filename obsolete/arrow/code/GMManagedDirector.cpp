/******************************************************************************
 GMManagedDirector.cc

	BASE CLASS = public JXWindowDirector

	Copyright (C) 1999 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#include <GMManagedDirector.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

GMManagedDirector::GMManagedDirector
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GMManagedDirector::~GMManagedDirector()
{
}

/******************************************************************************
 GetShortcut (virtual)

 ******************************************************************************/

JBoolean
GMManagedDirector::GetShortcut
	(
	JString* shortcut
	)
	const
{
	return kJFalse;
}

/******************************************************************************
 GetID (virtual)

 ******************************************************************************/

JBoolean
GMManagedDirector::GetID
	(
	JString* id
	)
	const
{
	return kJFalse;
}
