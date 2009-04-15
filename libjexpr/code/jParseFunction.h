/******************************************************************************
 jParseFunction.h

	Interface for jParseFunction module.

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_jParseFunction
#define _H_jParseFunction

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jTypes.h>

class JVariableList;
class JFunction;

JBoolean JParseFunction(const JCharacter* expr, const JVariableList* theVariableList,
						JFunction** theFunction, const JBoolean allowUIF = kJFalse);

JBoolean JNameValid(const JCharacter* str);

#endif
