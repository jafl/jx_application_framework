/******************************************************************************
 jFunctionUtil.h

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_FunctionUtil
#define _H_FunctionUtil

#include <jTypes.h>

class JFunction;

JBoolean ParenthesizeArgForPrint(const JFunction& f, const JFunction& arg);
JBoolean ParenthesizeArgForRender(const JFunction& f, const JFunction& arg);

enum JNamedConstIndex
{
	kPiJNamedConstIndex = 1,
	kEJNamedConstIndex,
	kIJNamedConstIndex,

	kJNamedConstCount = kIJNamedConstIndex
};

#endif
