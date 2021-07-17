/******************************************************************************
 CBDTreeScanner.cpp

	Extracts class information from D source file.

	BASE CLASS = CBDTreeFlexLexer

	Copyright © 2021 John Lindal.

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
	itsAbstractFlag = false;
	itsFinalFlag    = false;
	itsTemplateFlag = false;
	itsCurrentClass = nullptr;
}
