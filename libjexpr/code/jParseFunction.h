/******************************************************************************
 jParseFunction.h

	Interface for jParseFunction module.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_jParseFunction
#define _H_jParseFunction

#include <jTypes.h>

class JVariableList;
class JFunction;

JBoolean JParseFunction(const JCharacter* expr, const JVariableList* theVariableList,
						JFunction** theFunction, const JBoolean allowUIF = kJFalse);

JBoolean JNameValid(const JCharacter* str);

#endif
