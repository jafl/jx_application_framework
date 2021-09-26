/******************************************************************************
 JCreateTextPG.cpp

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "jx-af/jcore/JCreateTextPG.h"
#include "jx-af/jcore/JTextProgressDisplay.h"
#include "jx-af/jcore/jAssert.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JCreateTextPG::JCreateTextPG()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JCreateTextPG::~JCreateTextPG()
{
}

/******************************************************************************
 New (virtual)

 ******************************************************************************/

JProgressDisplay*
JCreateTextPG::New()
{
	JProgressDisplay* pg = jnew JTextProgressDisplay;
	assert( pg != nullptr );
	return pg;
}
