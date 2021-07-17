/******************************************************************************
 jFunctionUtil.h

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_FunctionUtil
#define _H_FunctionUtil

#include <jTypes.h>

class JString;
class JFunction;

bool	JGetArgCount(const JString& fnName, JSize* argCount);

bool JParenthesizeArgForPrint(const JFunction& f, const JFunction& arg);
bool JParenthesizeArgForRender(const JFunction& f, const JFunction& arg);

enum JNamedConstIndex
{
	kPiJNamedConstIndex = 1,
	kEJNamedConstIndex,
	kIJNamedConstIndex,

	kJNamedConstCount = kIJNamedConstIndex
};

#endif
