/******************************************************************************
 JFunctionType.h

	Defines all known types of functions.

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_FunctionType
#define _H_FunctionType

#include <jTypes.h>

// Do not change these values once they are assigned
// because they may be stored in files.

enum JFunctionType
{
	kJVariableValueType = 100,	// special
	kJConstantValueType = 101,
	kJNamedConstantType = 102,

	kJDiscVarValueIndexType = 110,
	kJDiscVarValueType      = 111,

	kJUserInputType = 120,

	kJSquareRootType = 200,		// unary functions
	kJAbsValueType   = 201,
	kJSignType       = 202,
	kJRoundType      = 203,
	kJTruncateType   = 204,

	kJLog10Type   = 210,
	kJLogEType    = 211,
	kJLog2Type    = 212,

	kJSineType       = 220,
	kJCosineType     = 221,
	kJTangentType    = 222,
	kJArcSineType    = 223,
	kJArcCosineType  = 224,
	kJArcTangentType = 225,

	kJHypSineType       = 230,
	kJHypCosineType     = 231,
	kJHypTangentType    = 232,
	kJArcHypSineType    = 233,
	kJArcHypCosineType  = 234,
	kJArcHypTangentType = 235,

	kJRealPartType   = 240,
	kJImagPartType   = 241,
	kJPhaseAngleType = 242,
	kJConjugateType  = 243,

	kJNegationType = 250,		// unary operators

	kJArcTangent2Type    = 300,	// binary functions
	kJLogBType           = 301,
	kJRotateType         = 302,

	kJAdditionType       = 350,	// binary operators
	kJSubtractionType    = 351,
	kJMultiplicationType = 352,
	kJDivisionType       = 353,
	kJExponentType       = 354,

	kJMaxFuncType   = 400,		// n-ary functions
	kJMinFuncType   = 401,

	kJSummationType = 450,		// n-ary operators
	kJProductType   = 451,
	kJParallelType  = 452
};

class JFunction;

JBoolean ParenthesizeArgForPrint(const JFunction& f, const JFunction& arg);
JBoolean ParenthesizeArgForRender(const JFunction& f, const JFunction& arg);

enum JFnNameIndex
{
	kJAdditionNameIndex = 1,
	kJSubtractionNameIndex,
	kJMultiplicationNameIndex,
	kJDivisionNameIndex,
	kJExponentNameIndex,

	kJSquareRootNameIndex,
	kJAbsValueNameIndex,
	kJSignNameIndex,
	kJRoundNameIndex,
	kJTruncateNameIndex,

	kJLogENameIndex,

	kJSineNameIndex,
	kJCosineNameIndex,
	kJTangentNameIndex,
	kJArcSineNameIndex,
	kJArcCosineNameIndex,
	kJArcTangentNameIndex,

	kJHypSineNameIndex,
	kJHypCosineNameIndex,
	kJHypTangentNameIndex,
	kJArcHypSineNameIndex,
	kJArcHypCosineNameIndex,
	kJArcHypTangentNameIndex,

	kJRealPartNameIndex,
	kJImagPartNameIndex,
	kJPhaseAngleNameIndex,
	kJConjugateNameIndex,

	kJLogBNameIndex,
	kJArcTangent2NameIndex,
	kJRotateNameIndex,

	kJMaxFuncNameIndex,
	kJMinFuncNameIndex,
	kJParallelNameIndex,

	kJFnNameCount = kJParallelNameIndex
};

enum JNamedConstIndex
{
	kPiJNamedConstIndex = 1,
	kEJNamedConstIndex,
	kIJNamedConstIndex,

	kJNamedConstCount = kIJNamedConstIndex
};

#endif
