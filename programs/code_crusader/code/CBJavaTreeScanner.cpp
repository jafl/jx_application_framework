/******************************************************************************
 CBJavaTreeScanner.cpp

	Extracts class information from Java source file.

	BASE CLASS = CBJavaTreeFlexLexer

	Copyright © 1999 John Lindal. All rights reserved.

 ******************************************************************************/

#include <cbStdInc.h>
#include "CBJavaTreeScanner.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

CBJavaTreeScanner::CBJavaTreeScanner()
	:
	CBJavaTreeFlexLexer()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBJavaTreeScanner::~CBJavaTreeScanner()
{
}

/******************************************************************************
 ResetState (private)

 ******************************************************************************/

void
CBJavaTreeScanner::ResetState()
{
	itsAbstractFlag = kJFalse;
	itsPublicFlag   = kJFalse;
	itsFinalFlag    = kJFalse;
	itsGenericDepth = 0;
	itsCurrentClass = NULL;
}
