/******************************************************************************
 CBGoTreeScanner.cpp

	Extracts class information from Go source file.

	BASE CLASS = CBGoTreeFlexLexer

	Copyright (C) 1999 John Lindal.

 ******************************************************************************/

#include "CBGoTreeScanner.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

CBGoTreeScanner::CBGoTreeScanner()
	:
	CBGoTreeFlexLexer()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBGoTreeScanner::~CBGoTreeScanner()
{
}

/******************************************************************************
 ResetState (private)

 ******************************************************************************/

void
CBGoTreeScanner::ResetState()
{
	itsCurrentClass = nullptr;
}
