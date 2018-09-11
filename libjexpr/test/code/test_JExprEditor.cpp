/******************************************************************************
 test_JExprEditor.cpp

	Tests for JExprEditor.

	Written by John Lindal.

 *****************************************************************************/

#include <JTestManager.h>
#include <JExprEditor.h>
#include "TestVarList.h"
#include "TestFontManager.h"
#include <JFunction.h>
#include <JSummation.h>
#include <JConstantValue.h>
#include <fstream>
#include <typeinfo>
#include <jAssert.h>

int main()
{
	return JTestManager::Execute();
}

JTEST(rtti)
{
	JFunction* c1 = jnew JConstantValue(3.5);
	assert( c1 != nullptr );

	JFunction* c2 = jnew JConstantValue(-2);
	assert( c2 != nullptr );

	JSummation* sum = jnew JSummation;
	assert( sum != nullptr );
	sum->AppendArg(c1);
	sum->AppendArg(c2);

	const std::type_info& i1 = typeid(*c1);
	const std::type_info& i2 = typeid(*c2);
	JAssertTrue(i1 == i2);

	JAssertTrue(i1 == typeid(JConstantValue));

	const std::type_info* i3 = & typeid(*sum);

	JAssertTrue(*i3 == typeid(JSummation));

	const JFunction* sum2 = sum;

	JAssertTrue(typeid(*sum2) == typeid(JSummation));
}
