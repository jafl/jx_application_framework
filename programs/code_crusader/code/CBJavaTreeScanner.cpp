/******************************************************************************
 CBJavaTreeScanner.cpp

	Extracts class information from Java source file.

	BASE CLASS = CBJavaTreeFlexLexer

	Copyright © 1999 John Lindal.

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
	itsAbstractFlag = false;
	itsPublicFlag   = false;
	itsFinalFlag    = false;
	itsCurrentClass = nullptr;
}
