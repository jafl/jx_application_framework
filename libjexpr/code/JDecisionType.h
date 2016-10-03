/******************************************************************************
 JDecisionType.h

	Defines all known types of decisions.

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JDecisionType
#define _H_JDecisionType

#include <jTypes.h>

// Do not change these values once they are assigned
// because they may be stored in files

enum JDecisionType
{
	kJBooleanANDType = 0,
	kJBooleanORType  = 1,
	kJBooleanXORType = 2,
	kJBooleanNOTType = 3,

	kJDiscreteEqualityType    = 100,
	kJValueUnknownType        = 101,
	kJDiscreteVarEqualityType = 102,
	kJBooleanConstantType     = 103,

	kJFunctionEqualityType = 200,
	kJLessThanType         = 201,
	kJLessEqualThanType    = 202,
	kJGreaterThanType      = 203,
	kJGreaterEqualThanType = 204
};

#endif
