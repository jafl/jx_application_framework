/******************************************************************************
 JBooleanOR.cpp

								The Boolean OR Class

	This class performs the Boolean OR of two other decisions.

	BASE CLASS = JBooleanDecision

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JBooleanOR.h>
#include <jParserData.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JBooleanOR::JBooleanOR()
	:
	JBooleanDecision(JPGetBooleanORString(), kJBooleanORType)
{
}

JBooleanOR::JBooleanOR
	(
	JDecision* arg1,
	JDecision* arg2
	)
	:
	JBooleanDecision(arg1, arg2, JPGetBooleanORString(), kJBooleanORType)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JBooleanOR::~JBooleanOR()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JBooleanOR::JBooleanOR
	(
	const JBooleanOR& source
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
JBooleanOR::Copy()
	const
{
	JBooleanOR* newDecision = new JBooleanOR(*this);
	assert( newDecision != NULL );
	return newDecision;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

JBoolean
JBooleanOR::Evaluate()
	const
{
	return JConvertToBoolean( (GetArg1())->Evaluate() || (GetArg2())->Evaluate() );
}
