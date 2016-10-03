/******************************************************************************
 JBooleanXOR.cpp

								The Boolean XOR Class

	This class performs the Boolean XOR of two other decisions.

	BASE CLASS = JBooleanDecision

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JBooleanXOR.h>
#include <jParserData.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JBooleanXOR::JBooleanXOR()
	:
	JBooleanDecision(JPGetBooleanXORString(), kJBooleanXORType)
{
}

JBooleanXOR::JBooleanXOR
	(
	JDecision* arg1,
	JDecision* arg2
	)
	:
	JBooleanDecision(arg1, arg2, JPGetBooleanXORString(), kJBooleanXORType)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JBooleanXOR::~JBooleanXOR()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JBooleanXOR::JBooleanXOR
	(
	const JBooleanXOR& source
	)
	:
	JBooleanDecision(source)
{
}

/******************************************************************************
 Copy

	This provides a way to copy an object without knowing its exact class.

 ******************************************************************************/

JDecision*
JBooleanXOR::Copy()
	const
{
	JBooleanXOR* newDecision = new JBooleanXOR(*this);
	assert( newDecision != NULL );
	return newDecision;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

JBoolean
JBooleanXOR::Evaluate()
	const
{
	const JBoolean arg1 = (GetArg1())->Evaluate();
	const JBoolean arg2 = (GetArg2())->Evaluate();

	return JConvertToBoolean((arg1 && !arg2) || (!arg1 && arg2));
}
