/******************************************************************************
 test_JComplex.cpp

	Program to test JComplex class.

	Written by John Lindal.

 ******************************************************************************/

#include "JTestManager.h"
#include "JComplex.h"
#include "jAssert.h"

int main()
{
	return JTestManager::Execute();
}

JTEST(Print)
{
	JAssertStringsEqual("0", JPrintComplexNumber(JComplex(0,0)));
	JAssertStringsEqual("1", JPrintComplexNumber(JComplex(1,0)));
	JAssertStringsEqual("i", JPrintComplexNumber(JComplex(0,1)));
	JAssertStringsEqual("-1", JPrintComplexNumber(JComplex(-1,0)));
	JAssertStringsEqual("-i", JPrintComplexNumber(JComplex(0,-1)));
	JAssertStringsEqual("-2 i", JPrintComplexNumber(JComplex(0,-2)));
	JAssertStringsEqual("1 + i", JPrintComplexNumber(JComplex(1,1)));
	JAssertStringsEqual("1 - i", JPrintComplexNumber(JComplex(1,-1)));
	JAssertStringsEqual("1 + 2 i", JPrintComplexNumber(JComplex(1,2)));
	JAssertStringsEqual("1 - 2 i", JPrintComplexNumber(JComplex(1,-2)));

	JAssertFalse(JUsingEEImag());
	JUseEEImag(true);
	JAssertTrue(JUsingEEImag());
	JAssertStringsEqual("1 + 2 j", JPrintComplexNumber(JComplex(1,2)));

	JSetComplexDisplayMode(kDisplayMagAndPhase);
	JAssertEqual(kDisplayMagAndPhase, JGetComplexDisplayMode());
	JAssertStringsEqual("0", JPrintComplexNumber(JComplex(0,0)));
	JAssertStringsEqual("1", JPrintComplexNumber(JComplex(1,0)));
	JAssertStringsEqual("1, 90 deg", JPrintComplexNumber(JComplex(0,1)));
	JAssertStringsEqual("-1", JPrintComplexNumber(JComplex(-1,0)));
	JAssertStringsEqual("1, -90 deg", JPrintComplexNumber(JComplex(0,-1)));
	JAssertStringsEqual("2, -90 deg", JPrintComplexNumber(JComplex(0,-2)));
	JAssertTrue(JPrintComplexNumber(JComplex(1,1)).StartsWith("1.414"));
	JAssertTrue(JPrintComplexNumber(JComplex(1,1)).EndsWith(", 45 deg"));
}
