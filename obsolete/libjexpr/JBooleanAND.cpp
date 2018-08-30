/******************************************************************************
 JBooleanAND.cpp

								The Boolean AND Class

	This class performs the Boolean AND of two other decisions.

	BASE CLASS = JBooleanDecision

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include <JBooleanAND.h>
#include <jParserData.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JBooleanAND::JBooleanAND()
	:
	JBooleanDecision(JPGetBooleanANDString(), kJBooleanANDType)
{
}

JBooleanAND::JBooleanAND
	(
	JDecision* arg1,
	JDecision* arg2
	)
	:
	JBooleanDecision(arg1, arg2, JPGetBooleanANDString(), kJBooleanANDType)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JBooleanAND::~JBooleanAND()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JBooleanAND::JBooleanAND
	(
	const JBooleanAND& source
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
JBooleanAND::Copy()
	const
{
	JBooleanAND* newDecision = jnew JBooleanAND(*this);
	assert( newDecision != NULL );
	return newDecision;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

JBoolean
JBooleanAND::Evaluate()
	const
{
	return JConvertToBoolean( (GetArg1())->Evaluate() && (GetArg2())->Evaluate() );
}
