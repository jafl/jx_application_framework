/******************************************************************************
 TestFontManagerMissingGlyphs.cpp

	BASE CLASS = TestFontManager

	Written by John Lindal.

 ******************************************************************************/

#include "TestFontManagerMissingGlyphs.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TestFontManagerMissingGlyphs::TestFontManagerMissingGlyphs()
	:
	TestFontManager()
{
}

/******************************************************************************
 HasGlyphForCharacter (virtual protected)

 ******************************************************************************/

JBoolean
TestFontManagerMissingGlyphs::HasGlyphForCharacter
	(
	const JFontID			id,
	const JUtf8Character&	c
	)
{
	return JI2B(id == 1 || (c != 'w' && c != 'm'));
}

/******************************************************************************
 GetSubstituteFontName (virtual protected)

 ******************************************************************************/

JBoolean
TestFontManagerMissingGlyphs::GetSubstituteFontName
	(
	const JFont&			f,
	const JUtf8Character&	c,
	JString*				name
	)
{
	if (f.GetID() == 2 && (c == 'w' || c == 'm'))
	{
		*name = GetFontName(1);
		return kJTrue;
	}
	else
	{
		return kJFalse;
	}
}

/******************************************************************************
 GetCharWidth (virtual)

 ******************************************************************************/

JSize
TestFontManagerMissingGlyphs::GetCharWidth
	(
	const JFontID			fontID,
	const JUtf8Character&	c
	)
{
	return (fontID == 2 ? 10 : 7);
}

/******************************************************************************
 GetStringWidth (virtual)

 ******************************************************************************/

JSize
TestFontManagerMissingGlyphs::GetStringWidth
	(
	const JFontID	fontID,
	const JString&	str
	)
{
	return (fontID == 2 ? 10 : 7)*str.GetCharacterCount();
}
