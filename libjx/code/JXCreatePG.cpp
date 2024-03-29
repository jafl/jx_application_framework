/******************************************************************************
 JXCreatePG.cpp

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXCreatePG.h"
#include "JXStandAlonePG.h"
#include <jx-af/jcore/jAssert.h>

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
	return pg;
}
