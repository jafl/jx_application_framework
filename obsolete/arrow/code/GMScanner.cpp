/******************************************************************************
 GMScanner.cc

	BASE CLASS = public GMFlexLexer

	Copyright (C) 1997 by Glenn W. Bach.  All rights reserved.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#include <GMScanner.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

GMScanner::GMScanner
	(
	std::istream* is
	)
	:
	GMFlexLexer(is)
{
	itsCurrentHeaderStart = 0;
	itsCurrentPosition = 0;
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GMScanner::~GMScanner()
{
}
