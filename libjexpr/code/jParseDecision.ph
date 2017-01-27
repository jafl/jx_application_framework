/******************************************************************************
 jParseDecision.ph

	Protected interface for jParseDecision module.

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_jParseDecision_protected
#define _H_jParseDecision_protected

#include <jTypes.h>

class JVariableList;
class JDecision;

JBoolean JRecurseDecision(const JCharacter* origExpr, const JSize origLength,
						  const JVariableList* theVariableList, JDecision** theDecision);

#endif
