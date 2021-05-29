/******************************************************************************
 CBCTreeScanner.cpp

	Extracts class information from C header file.

	BASE CLASS = CBCTreeFlexLexer

	Copyright (C) 2021 John Lindal.

 ******************************************************************************/

#include "CBCTreeScanner.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

CBCTreeScanner::CBCTreeScanner()
	:
	CBCTreeFlexLexer()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBCTreeScanner::~CBCTreeScanner()
{
}

/******************************************************************************
 ResetState (private)

 ******************************************************************************/

void
CBCTreeScanner::ResetState()
{
	itsTemplateFlag = kJFalse;
	itsCurrentClass = nullptr;
}
