/******************************************************************************
 scGlobals.cpp

	Access to xsymcir global objects and factories.

	Copyright (C) 1998 John Lindal.

 ******************************************************************************/

#include "scGlobals.h"
#include "scHelpText.h"
#include <JMathematicaSM.h>
#include <JNamedConstant.h>
#include <JComplex.h>
#include <stdlib.h>
#include <jAssert.h>

static SymcirApp*		theApplication  = NULL;		// owns itself
static JSymbolicMath*	theSymbolicMath = NULL;

/******************************************************************************
 CBCreateGlobals

 ******************************************************************************/

void
SCCreateGlobals
	(
	SymcirApp* app
	)
{
	theApplication = app;

	JMathematicaSM* mathSM;
	if (JMathematicaSM::Create(&mathSM))
		{
		theSymbolicMath = mathSM;
		}
	else
		{
		exit(1);
		}

	JXInitHelp(NULL, kSCHelpSectionCount, kSCHelpSectionName);

	JXCreateDefaultDocumentManager(kJFalse);

	// This program is for Electrical Engineers.

	JUseEEImag(kJTrue);
	JSetComplexDisplayMode(kDisplayMagAndPhase);
}

/******************************************************************************
 CBDeleteGlobals

 ******************************************************************************/

void
SCDeleteGlobals()
{
	theApplication = NULL;

	delete theSymbolicMath;
	theSymbolicMath = NULL;
}

/******************************************************************************
 CBGetApplication

 ******************************************************************************/

SymcirApp*
SCGetApplication()
{
	assert( theApplication != NULL );
	return theApplication;
}

/******************************************************************************
 SCGetSymbolicMath

 ******************************************************************************/

JSymbolicMath*
SCGetSymbolicMath()
{
	assert( theSymbolicMath != NULL );
	return theSymbolicMath;
}
