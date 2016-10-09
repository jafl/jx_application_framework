/******************************************************************************
 JXCreatePG.cpp

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXCreatePG.h>
#include <JXStandAlonePG.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXCreatePG::JXCreatePG()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXCreatePG::~JXCreatePG()
{
}

/******************************************************************************
 New (virtual)

 ******************************************************************************/

JProgressDisplay*
JXCreatePG::New()
{
	JProgressDisplay* pg = jnew JXStandAlonePG;
	assert( pg != NULL );
	return pg;
}
