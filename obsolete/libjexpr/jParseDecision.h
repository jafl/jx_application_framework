/******************************************************************************
 jParseDecision.h

	Interface for jParseDecision module.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_jParseDecision
#define _H_jParseDecision

#include <jTypes.h>

class JVariableList;
class JDecision;

JBoolean JParseDecision(const JCharacter* expr, const JVariableList* theVariableList,
						JDecision** theDecision);

JBoolean JDiscreteValueNameValid(const JCharacter* str);

#endif
