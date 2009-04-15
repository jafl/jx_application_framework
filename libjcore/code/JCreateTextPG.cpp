/******************************************************************************
 JCreateTextPG.cpp

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JCreateTextPG.h>
#include <JTextProgressDisplay.h>
#include <jAssert.h>

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
	JProgressDisplay* pg = new JTextProgressDisplay;
	assert( pg != NULL );
	return pg;
}
