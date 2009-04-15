/******************************************************************************
 jParseFunction.ph

	Protected interface for jParseFunction module.

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_jParseFunction_protected
#define _H_jParseFunction_protected

#include <JParseResult.h>
#include <jTypes.h>

class JVariableList;
class JFunction;

JBoolean JRecurseFunction(const JCharacter* origExpr, const JSize origLength,
						  const JVariableList* theVariableList, JFunction** theFunction,
						  const JBoolean allowUIF = kJFalse);

JParseResult JParseVariable(const JCharacter* origExpr, const JSize origLength,
							const JVariableList* theVariableList,
							JIndex* varIndex, JFunction** arrayIndex,
							const JBoolean allowUIF = kJFalse);

#endif
