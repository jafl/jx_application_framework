/******************************************************************************
 CBJavaTreeScanner.cpp

	Extracts class information from Java source file.

	BASE CLASS = CBJavaTreeFlexLexer

	Copyright (C) 1999 John Lindal.

 ******************************************************************************/

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
	itsCurrentClass = nullptr;
}
