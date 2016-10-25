/******************************************************************************
 JPrinter.cpp

	This class abstracts the concepts of Page Setup and Print Setup.
	Derived classes must implement the concepts with dialogs and broadcast
	our messages when the dialogs are closed.

	BASE CLASS = virtual JBroadcaster

	Copyright (C) 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JPrinter.h>

// JBroadcaster message types

const JUtf8Byte* JPrinter::kPageSetupFinished  = "PageSetupFinished::JPrinter";
const JUtf8Byte* JPrinter::kPrintSetupFinished = "PrintSetupFinished::JPrinter";

/******************************************************************************
 Constructor

 ******************************************************************************/

JPrinter::JPrinter()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JPrinter::~JPrinter()
{
}
