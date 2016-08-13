/******************************************************************************
 jParserData.cpp

	Data for JDecision and JFunction parsers.

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <jParserData.h>
#include <JComplex.h>
#include <string.h>

static const JCharacter* kArgSeparatorString = ",";
const JSize kArgSeparatorStringLength        = 1;

static const JCharacter* kValueUnknownString = " is unknown";
const JSize kValueUnknownStringLength        = strlen(kValueUnknownString);

static const JCharacter* kBooleanNOTString   = "not ";
const JSize kBooleanNOTStringLength          = strlen(kBooleanNOTString);

const JCharacter*
JPGetArgSeparatorString()
{
	return kArgSeparatorString;
}

JSize
JPGetArgSeparatorStringLength()
{
	return kArgSeparatorStringLength;
}

const JCharacter*
JPGetValueUnknownString()
{
	return kValueUnknownString;
}

JSize
JPGetValueUnknownStringLength()
{
	return kValueUnknownStringLength;
}

const JCharacter*
JPGetBooleanNOTString()
{
	return kBooleanNOTString;
}

JSize
JPGetBooleanNOTStringLength()
{
	return kBooleanNOTStringLength;
}

/******************************************************************************
 Boolean operators

 ******************************************************************************/

static const JCharacter* kBooleanANDString = " and ";
static const JCharacter* kBooleanORString  = " or ";
static const JCharacter* kBooleanXORString = " xor ";

static JDecisionOperatorInfo kBooleanOpInfo[] =
{
	{kJBooleanORType,  kBooleanORString,  0},
	{kJBooleanXORType, kBooleanXORString, 1},
	{kJBooleanANDType, kBooleanANDString, 2}
};
const JSize kBooleanOpCount = sizeof(kBooleanOpInfo)/sizeof(JDecisionOperatorInfo);

const JCharacter*
JPGetBooleanANDString()
{
	return kBooleanANDString;
}

const JCharacter*
JPGetBooleanORString()
{
	return kBooleanORString;
}

const JCharacter*
JPGetBooleanXORString()
{
	return kBooleanXORString;
}

JSize
JPGetBooleanOpCount()
{
	return kBooleanOpCount;
}

const JDecisionOperatorInfo*
JPGetBooleanOpInfo()
{
	return kBooleanOpInfo;
}

/******************************************************************************
 Function comparisons

 ******************************************************************************/

static const JCharacter* kEqualityString         = "=";
const JSize kEqualityStringLength                = 1;

static const JCharacter* kLessThanString         = "<";
static const JCharacter* kLessEqualThanString    = "<=";
static const JCharacter* kGreaterThanString      = ">";
static const JCharacter* kGreaterEqualThanString = ">=";

static JDecisionOperatorInfo kComparisonOpInfo[] =
{
	// we must check for <=,>= before checking for <,>

	{kJLessEqualThanType,    kLessEqualThanString,    0},
	{kJGreaterEqualThanType, kGreaterEqualThanString, 1},

	// now we can check for everything else

	{kJFunctionEqualityType, kEqualityString,         2},
	{kJLessThanType,         kLessThanString,         3},
	{kJGreaterThanType,      kGreaterThanString,      4}
};
const JSize kComparisonOpCount = sizeof(kComparisonOpInfo)/sizeof(JDecisionOperatorInfo);

const JCharacter*
JPGetEqualityString()
{
	return kEqualityString;
}

JSize
JPGetEqualityStringLength()
{
	return kEqualityStringLength;
}

const JCharacter*
JPGetLessThanString()
{
	return kLessThanString;
}

const JCharacter*
JPGetLessEqualThanString()
{
	return kLessEqualThanString;
}

const JCharacter*
JPGetGreaterThanString()
{
	return kGreaterThanString;
}

const JCharacter*
JPGetGreaterEqualThanString()
{
	return kGreaterEqualThanString;
}

JSize
JPGetComparisonOpCount()
{
	return kComparisonOpCount;
}

const JDecisionOperatorInfo*
JPGetComparisonOpInfo()
{
	return kComparisonOpInfo;
}

/******************************************************************************
 Boolean constants

 ******************************************************************************/

static const JCharacter* kTrueConstDecisionString  = "True";
static const JCharacter* kFalseConstDecisionString = "False";

static JBoolConstInfo kBoolConstInfo[] =
{
	{kTrueConstDecisionString,  kJTrue},
	{kFalseConstDecisionString, kJFalse},
};
const JSize kBoolConstCount = sizeof(kBoolConstInfo)/sizeof(JBoolConstInfo);

const JCharacter*
JPGetTrueConstDecisionString()
{
	return kTrueConstDecisionString;
}

const JCharacter*
JPGetFalseConstDecisionString()
{
	return kFalseConstDecisionString;
}

JSize
JPGetBoolConstCount()
{
	return kBoolConstCount;
}

const JBoolConstInfo*
JPGetBoolConstInfo()
{
	return kBoolConstInfo;
}

/******************************************************************************
 Function operators

 ******************************************************************************/

static const JCharacter* kAdditionString       = "+";
static const JCharacter* kSubtractionString    = "-";
static const JCharacter* kMultiplicationString = "*";
static const JCharacter* kDivisionString       = "/";
static const JCharacter* kExponentString       = "^";

const JSize kFunctionOpStringLength = 1;

const JCharacter*
JPGetAdditionString()
{
	return kAdditionString;
}

JSize
JPGetAdditionStringLength()
{
	return kFunctionOpStringLength;
}

const JCharacter*
JPGetSubtractionString()
{
	return kSubtractionString;
}

JSize
JPGetSubtractionStringLength()
{
	return kFunctionOpStringLength;
}

const JCharacter*
JPGetMultiplicationString()
{
	return kMultiplicationString;
}

JSize
JPGetMultiplicationStringLength()
{
	return kFunctionOpStringLength;
}

const JCharacter*
JPGetDivisionString()
{
	return kDivisionString;
}

JSize
JPGetDivisionStringLength()
{
	return kFunctionOpStringLength;
}

const JCharacter*
JPGetExponentString()
{
	return kExponentString;
}

JSize
JPGetExponentStringLength()
{
	return kFunctionOpStringLength;
}

/******************************************************************************
 Functions of a discrete variable

 ******************************************************************************/

static const JCharacter* kDiscVarValueIndexString = "index(";
static const JCharacter* kDiscVarValueString      = "value(";

static const JCharacter* kMathematicaDiscVarValueIndexString = "index[";
static const JCharacter* kMathematicaDiscVarValueString      = "value[";

static JFunctionOfDiscreteInfo kFnOfDiscreteInfo[] =
{
	{kJDiscVarValueIndexType, kDiscVarValueIndexString},
	{kJDiscVarValueIndexType, kMathematicaDiscVarValueIndexString},
	{kJDiscVarValueType,      kDiscVarValueString},
	{kJDiscVarValueType,      kMathematicaDiscVarValueString}
};
const JSize kFnOfDiscreteCount = sizeof(kFnOfDiscreteInfo)/sizeof(JFunctionOfDiscreteInfo);

const JCharacter*
JPGetDiscVarValueIndexString()
{
	return kDiscVarValueIndexString;
}

const JCharacter*
JPGetDiscVarValueString()
{
	return kDiscVarValueString;
}

const JCharacter*
JPGetMathematicaDiscVarValueIndexString()
{
	return kMathematicaDiscVarValueIndexString;
}

const JCharacter*
JPGetMathematicaDiscVarValueString()
{
	return kMathematicaDiscVarValueString;
}

JSize
JPGetFnOfDiscreteCount()
{
	return kFnOfDiscreteCount;
}

const JFunctionOfDiscreteInfo*
JPGetFnOfDiscreteInfo()
{
	return kFnOfDiscreteInfo;
}

/******************************************************************************
 Functions taking arguments

 ******************************************************************************/

static const JCharacter* kSquareRootString = "sqrt(";
static const JCharacter* kAbsValueString   = "abs(";
static const JCharacter* kSignString       = "sign(";
static const JCharacter* kRoundString      = "round(";
static const JCharacter* kTruncateString   = "truncate(";

static const JCharacter* kLog10String = "log10(";
static const JCharacter* kLogEString  = "ln(";
static const JCharacter* kLog2String  = "log2(";

static const JCharacter* kSineString    = "sin(";
static const JCharacter* kCosineString  = "cos(";
static const JCharacter* kTangentString = "tan(";
static const JCharacter* kArcsinString  = "arcsin(";
static const JCharacter* kArccosString  = "arccos(";
static const JCharacter* kArctanString  = "arctan(";

static const JCharacter* kHypSineString       = "sinh(";
static const JCharacter* kHypCosineString     = "cosh(";
static const JCharacter* kHypTangentString    = "tanh(";
static const JCharacter* kArcHypSineString    = "arcsinh(";
static const JCharacter* kArcHypCosineString  = "arccosh(";
static const JCharacter* kArcHypTangentString = "arctanh(";

static const JCharacter* kRealPartString   = "re(";
static const JCharacter* kImagPartString   = "im(";
static const JCharacter* kPhaseAngleString = "arg(";
static const JCharacter* kConjugateString  = "conjugate(";

static const JCharacter* kLogBString    = "log(";
static const JCharacter* kArctan2String = "arctan2(";
static const JCharacter* kRotateString  = "rotate(";

static const JCharacter* kMaxFuncString  = "max(";
static const JCharacter* kMinFuncString  = "min(";
static const JCharacter* kParallelString = "parallel(";

static const JCharacter* kMathematicaSquareRootString = "Sqrt[";
static const JCharacter* kMathematicaAbsValueString   = "Abs[";
static const JCharacter* kMathematicaSignString       = "Sign[";
static const JCharacter* kMathematicaRoundString      = "round[";
static const JCharacter* kMathematicaTruncateString   = "trunc[";

static const JCharacter* kMathematicaLog10String = "log10[";
static const JCharacter* kMathematicaLogEString  = "Log[";
static const JCharacter* kMathematicaLog2String  = "log2[";

static const JCharacter* kMathematicaSineString    = "Sin[";
static const JCharacter* kMathematicaCosineString  = "Cos[";
static const JCharacter* kMathematicaTangentString = "Tan[";
static const JCharacter* kMathematicaArcsinString  = "ArcSin[";
static const JCharacter* kMathematicaArccosString  = "ArcCos[";
static const JCharacter* kMathematicaArctanString  = "ArcTan[";

static const JCharacter* kMathematicaHypSineString       = "Sinh[";
static const JCharacter* kMathematicaHypCosineString     = "Cosh[";
static const JCharacter* kMathematicaHypTangentString    = "Tanh[";
static const JCharacter* kMathematicaArcHypSineString    = "ArcSinh(";
static const JCharacter* kMathematicaArcHypCosineString  = "ArcCosh(";
static const JCharacter* kMathematicaArcHypTangentString = "ArcTanh(";

static const JCharacter* kMathematicaRealPartString   = "Re[";
static const JCharacter* kMathematicaImagPartString   = "Im[";
static const JCharacter* kMathematicaPhaseAngleString = "Arg[";
static const JCharacter* kMathematicaConjugateString  = "Conjugate[";

static const JCharacter* kMathematicaLogBString    = "log[";
static const JCharacter* kMathematicaArctan2String = "arctan2[";
static const JCharacter* kMathematicaRotateString  = "rotate[";

static const JCharacter* kMathematicaMaxFuncString  = "Max[";
static const JCharacter* kMathematicaMinFuncString  = "Min[";
static const JCharacter* kMathematicaParallelString = "parallel[";

static JStdFunctionInfo kStdFunctionInfo[] =
{
	{kJSquareRootType, kSquareRootString,            1},
	{kJSquareRootType, kMathematicaSquareRootString, 1},
	{kJAbsValueType,   kAbsValueString,              1},
	{kJAbsValueType,   kMathematicaAbsValueString,   1},
	{kJSignType,       kSignString,                  1},
	{kJSignType,       kMathematicaSignString,       1},
	{kJRoundType,      kRoundString,                 1},
	{kJRoundType,      kMathematicaRoundString,      1},
	{kJTruncateType,   kTruncateString,              1},
	{kJTruncateType,   kMathematicaTruncateString,   1},

	{kJLog10Type,      kLog10String,                 1},
	{kJLog10Type,      kMathematicaLog10String,      1},
	{kJLogEType,       kLogEString,                  1},
	{kJLogEType,       kMathematicaLogEString,       1},
	{kJLog2Type,       kLog2String,                  1},
	{kJLog2Type,       kMathematicaLog2String,       1},

	{kJSineType,       kSineString,                  1},
	{kJSineType,       kMathematicaSineString,       1},
	{kJCosineType,     kCosineString,                1},
	{kJCosineType,     kMathematicaCosineString,     1},
	{kJTangentType,    kTangentString,               1},
	{kJTangentType,    kMathematicaTangentString,    1},
	{kJArcSineType,    kArcsinString,                1},
	{kJArcSineType,    kMathematicaArcsinString,     1},
	{kJArcCosineType,  kArccosString,                1},
	{kJArcCosineType,  kMathematicaArccosString,     1},
	{kJArcTangentType, kArctanString,                1},
	{kJArcTangentType, kMathematicaArctanString,     1},

	{kJHypSineType,       kHypSineString,                  1},
	{kJHypSineType,       kMathematicaHypSineString,       1},
	{kJHypCosineType,     kHypCosineString,                1},
	{kJHypCosineType,     kMathematicaHypCosineString,     1},
	{kJHypTangentType,    kHypTangentString,               1},
	{kJHypTangentType,    kMathematicaHypTangentString,    1},
	{kJArcHypSineType,    kArcHypSineString,               1},
	{kJArcHypSineType,    kMathematicaArcHypSineString,    1},
	{kJArcHypCosineType,  kArcHypCosineString,             1},
	{kJArcHypCosineType,  kMathematicaArcHypCosineString,  1},
	{kJArcHypTangentType, kArcHypTangentString,            1},
	{kJArcHypTangentType, kMathematicaArcHypTangentString, 1},

	{kJRealPartType,   kRealPartString,              1},
	{kJRealPartType,   kMathematicaRealPartString,   1},
	{kJImagPartType,   kImagPartString,              1},
	{kJImagPartType,   kMathematicaImagPartString,   1},
	{kJPhaseAngleType, kPhaseAngleString,            1},
	{kJPhaseAngleType, kMathematicaPhaseAngleString, 1},
	{kJConjugateType,  kConjugateString,             1},
	{kJConjugateType,  kMathematicaConjugateString,  1},

	{kJLogBType,        kLogBString,                  2},
	{kJLogBType,        kMathematicaLogBString,       2},
	{kJArcTangent2Type, kArctan2String,               2},
	{kJArcTangent2Type, kMathematicaArctan2String,    2},
	{kJRotateType,      kRotateString,                2},
	{kJRotateType,      kMathematicaRotateString,     2},

	{kJMaxFuncType,    kMaxFuncString,               kJUnlimitedArgs},
	{kJMaxFuncType,    kMathematicaMaxFuncString,    kJUnlimitedArgs},
	{kJMinFuncType,    kMinFuncString,               kJUnlimitedArgs},
	{kJMinFuncType,    kMathematicaMinFuncString,    kJUnlimitedArgs},
	{kJParallelType,   kParallelString,              kJUnlimitedArgs},
	{kJParallelType,   kMathematicaParallelString,   kJUnlimitedArgs}
};
const JSize kStdFunctionCount = sizeof(kStdFunctionInfo)/sizeof(JStdFunctionInfo);

// If you modify these arrays, be sure to update JFnNameIndex

static const JCharacter* kStdFnName[] =
{
	kAdditionString, kSubtractionString, kMultiplicationString,
	kDivisionString, kExponentString,

	kSquareRootString, kAbsValueString, kSignString, kRoundString, kTruncateString,
	kLogEString,
	kSineString, kCosineString, kTangentString,
	kArcsinString, kArccosString, kArctanString,
	kHypSineString, kHypCosineString, kHypTangentString,
	kArcHypSineString, kArcHypCosineString, kArcHypTangentString,
	kRealPartString, kImagPartString, kPhaseAngleString, kConjugateString,
	kLogBString, kArctan2String, kRotateString,
	kMaxFuncString, kMinFuncString, kParallelString
};

static const JCharacter* kMathematicaFnName[] =
{
	kAdditionString, kSubtractionString, kMultiplicationString,
	kDivisionString, kExponentString,

	kMathematicaSquareRootString, kMathematicaAbsValueString, kMathematicaSignString,
	kMathematicaRoundString, kMathematicaTruncateString,
	kMathematicaLogEString,
	kMathematicaSineString, kMathematicaCosineString, kMathematicaTangentString,
	kMathematicaArcsinString, kMathematicaArccosString, kMathematicaArctanString,
	kMathematicaHypSineString, kMathematicaHypCosineString, kMathematicaHypTangentString,
	kMathematicaArcHypSineString, kMathematicaArcHypCosineString, kMathematicaArcHypTangentString,
	kMathematicaRealPartString, kMathematicaImagPartString,
	kMathematicaPhaseAngleString, kMathematicaConjugateString,
	kMathematicaLogBString, kMathematicaArctan2String,
	kMathematicaRotateString,
	kMathematicaMaxFuncString, kMathematicaMinFuncString,
	kMathematicaParallelString
};

JSize
JPGetStdFunctionCount()
{
	return kStdFunctionCount;
}

const JStdFunctionInfo*
JPGetStdFunctionInfo()
{
	return kStdFunctionInfo;
}

const JCharacter*
JPGetStdFnName
	(
	const JIndex i
	)
{
	return kStdFnName[ i-1 ];
}

const JCharacter*
JPGetMathematicaFnName
	(
	const JIndex i
	)
{
	return kMathematicaFnName[ i-1 ];
}

/******************************************************************************
 Named constants

 ******************************************************************************/

static const JCharacter* kPiString    = "pi";
static const JCharacter* kPiAltString = "`p";
static const JCharacter* kEString     = "e";

static const JCharacter* kMathematicaPiString   = "Pi";
static const JCharacter* kMathematicaEString    = "E";
static const JCharacter* kMathematicaImagString = "I";

static JNamedConstInfo kNamedConstInfo[] =
{
	{kPiString,              kPiJNamedConstIndex},
	{kPiAltString,           kPiJNamedConstIndex},
	{kMathematicaPiString,   kPiJNamedConstIndex},
	{kEString,               kEJNamedConstIndex},
	{kMathematicaEString,    kEJNamedConstIndex},
	{JGetMathImagString(),   kIJNamedConstIndex},
	{JGetEEImagString(),     kIJNamedConstIndex},
	{kMathematicaImagString, kIJNamedConstIndex}
};
const JSize kNamedConstCount = sizeof(kNamedConstInfo)/sizeof(JNamedConstInfo);

// If you modify these arrays, be sure to update JNamedConstIndex (JFunctionType.h)
// and kNamedConstValues (JNamedConstant.cc)

static const JCharacter* kStdNamedConstName[] =
{
	kPiString, kEString, JGetMathImagString()
};

static const JCharacter* kMathematicaNamedConstName[] =
{
	kMathematicaPiString, kMathematicaEString, kMathematicaImagString
};

const JCharacter*
JPGetPiString()
{
	return kPiString;
}

JSize
JPGetNamedConstCount()
{
	return kNamedConstCount;
}

const JNamedConstInfo*
JPGetNamedConstInfo()
{
	return kNamedConstInfo;
}

const JCharacter*
JPGetStdNamedConstName
	(
	const JIndex i
	)
{
	return kStdNamedConstName[ i-1 ];
}

const JCharacter*
JPGetMathematicaNamedConstName
	(
	const JIndex i
	)
{
	return kMathematicaNamedConstName[ i-1 ];
}

/******************************************************************************
 Special characters

 ******************************************************************************/

// this must be a single character

const JCharacter kGreekCharPrefix            = '`';
static const JCharacter* kGreekCharPrefixStr = "`";
const JSize kGreekCharPrefixLength           = 1;

JCharacter
JPGetGreekCharPrefixChar()
{
	return kGreekCharPrefix;
}

const JCharacter*
JPGetGreekCharPrefixString()
{
	return kGreekCharPrefixStr;
}

JSize
JPGetGreekCharPrefixLength()
{
	return kGreekCharPrefixLength;
}
