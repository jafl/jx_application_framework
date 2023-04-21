/******************************************************************************
 test_jColor.cpp

	Program to test jColor utilities.

	Written by John Lindal.

 ******************************************************************************/

#include <jx-af/jcore/JTestManager.h>
#include <jx-af/jcore/jColor.h>
#include <sstream>
#include <jx-af/jcore/jAssert.h>

int main()
{
	return JTestManager::Execute();
}

JTEST(RGB)
{
	JRGB rgb(100,100,100);
	rgb *= 2;
	JAssertEqual(JRGB(200,200,200), rgb);
	rgb /= 2;
	JAssertEqual(JRGB(100,100,100), rgb);

	std::ostringstream output;
	output << rgb;

	std::istringstream input(output.str());
	JRGB rgb2;
	input >> rgb2;
	JAssertEqual(rgb, rgb2);
	JAssertEqual(std::weak_ordering::equivalent, JCompareRGBValues(rgb, rgb2));

	rgb2.Set(0,0,0);
	JRGB rgb3 = JBlend(rgb, rgb2, 0.5);
	JAssertEqual(JRGB(50,50,50), rgb3);

	JAssertTrue(rgb != rgb2);

	rgb2 = rgb3 * 2;
	JAssertEqual(JRGB(100,100,100), rgb2);
	rgb2 = 2 * rgb2;
	JAssertEqual(JRGB(200,200,200), rgb2);

	rgb2 = rgb2 / 2;
	JAssertEqual(JRGB(100,100,100), rgb2);
}

JTEST(HSB)
{
	JHSB hsb(0, kJMaxHSBValue, kJMaxHSBValue);
	JAssertEqual(JRGB(kJMaxRGBValue,0,0), JRGB(hsb));
	JAssertEqual(JHSB(JRGB(kJMaxRGBValue,0,0)), hsb);

	hsb.Set(0, kJMaxHSBValue/2, kJMaxHSBValue/2);
	JAssertEqual(JRGB(32767, 16384 ,16384), JRGB(hsb));

	std::ostringstream output;
	output << hsb;

	std::istringstream input(output.str());
	JHSB hsb2;
	input >> hsb2;
	JAssertEqual(hsb, hsb2);
	JAssertEqual(std::weak_ordering::equivalent, JCompareHSBValues(hsb, hsb2));

	JAssertFalse(hsb != hsb2);
}
