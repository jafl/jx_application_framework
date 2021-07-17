/******************************************************************************
 TestFontManagerMissingGlyphs.h

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestFontManagerMissingGlyphs
#define _H_TestFontManagerMissingGlyphs

#include "TestFontManager.h"

class TestFontManagerMissingGlyphs : public TestFontManager
{
public:

	TestFontManagerMissingGlyphs();

protected:

	virtual JSize	GetCharWidth(const JFontID fontID, const JUtf8Character& c);
	virtual JSize	GetStringWidth(const JFontID fontID, const JString& str);

	virtual bool	HasGlyphForCharacter(const JFontID id, const JUtf8Character& c);
	virtual bool	GetSubstituteFontName(const JFont& f, const JUtf8Character& c, JString* name);

private:

	// not allowed

	TestFontManagerMissingGlyphs(const TestFontManagerMissingGlyphs& source);
	const TestFontManagerMissingGlyphs& operator=(const TestFontManagerMissingGlyphs& source);
};

#endif
