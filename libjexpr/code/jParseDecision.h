/******************************************************************************
 jParseDecision.h

	Interface for jParseDecision module.

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_jParseDecision
#define _H_jParseDecision

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jTypes.h>

class JVariableList;
class JDecision;

JBoolean JParseDecision(const JCharacter* expr, const JVariableList* theVariableList,
						JDecision** theDecision);

JBoolean JDiscreteValueNameValid(const JCharacter* str);

#endif
