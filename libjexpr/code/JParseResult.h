/******************************************************************************
 JParseResult.h

	Defines data type for result of intermediate parsing.

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JParseResult
#define _H_JParseResult

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

enum JParseResult
{
	kJParsedOK,			// function parsed it successfully
	kJParseError,		// function parsed it and found an error
	kJNotMyProblem		// function doesn't understand it (i.e. try something else)
};

#endif
