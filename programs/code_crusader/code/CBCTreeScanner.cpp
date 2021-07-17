/******************************************************************************
 CBCTreeScanner.cpp

	Extracts class information from C header file.

	BASE CLASS = CBCTreeFlexLexer

	Copyright © 2021 John Lindal.

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
	itsTemplateFlag = false;
	itsCurrentClass = nullptr;
}
