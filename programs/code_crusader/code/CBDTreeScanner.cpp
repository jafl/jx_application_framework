/******************************************************************************
 CBDTreeScanner.cpp

	Extracts class information from D source file.

	BASE CLASS = CBDTreeFlexLexer

	Copyright (C) 2021 John Lindal.

 ******************************************************************************/

#include "CBDTreeScanner.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

CBDTreeScanner::CBDTreeScanner()
	:
	CBDTreeFlexLexer()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBDTreeScanner::~CBDTreeScanner()
{
}

/******************************************************************************
 ResetState (private)

 ******************************************************************************/

void
CBDTreeScanner::ResetState()
{
	itsAbstractFlag = kJFalse;
	itsFinalFlag    = kJFalse;
	itsTemplateFlag = kJFalse;
	itsCurrentClass = nullptr;
}
