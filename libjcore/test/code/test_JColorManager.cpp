/******************************************************************************
 test_JColorManager.cpp

	Program to test JColorManager class.

	Written by John Lindal.

 ******************************************************************************/

#include <JTestManager.h>
#include <JColorManager.h>
#include <jAssert.h>

int main()
{
	return JTestManager::Execute();
}

JTEST(ColorID)
{
	JRGB rgb(2367, 16235, 21809);
	const JColorID id = JColorManager::GetColorID(rgb);
	JAssertEqual(rgb, JColorManager::GetRGB(id));
}
