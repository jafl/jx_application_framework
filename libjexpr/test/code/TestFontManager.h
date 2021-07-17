/******************************************************************************
 TestFontManager.h

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestFontManager
#define _H_TestFontManager

#include <JFontManager.h>

class TestFontManager : public JFontManager
{
public:

	TestFontManager();

	virtual ~TestFontManager();

	virtual void		GetFontNames(JPtrArray<JString>* fontNames);
	virtual void		GetMonospaceFontNames(JPtrArray<JString>* fontNames);
	virtual bool	GetFontSizes(const JString& name, JSize* minSize,
									 JSize* maxSize, JArray<JSize>* sizeList);

protected:

	virtual JSize	GetLineHeight(const JFontID fontID, const JSize size,
								  const JFontStyle& style,
								  JCoordinate* ascent, JCoordinate* descent);

	virtual JSize	GetCharWidth(const JFontID fontID, const JUtf8Character& c);
	virtual JSize	GetStringWidth(const JFontID fontID, const JString& str);

	virtual bool	IsExact(const JFontID id);
	virtual bool	HasGlyphForCharacter(const JFontID id, const JUtf8Character& c);
	virtual bool	GetSubstituteFontName(const JFont& f, const JUtf8Character& c, JString* name);

private:

	JPtrArray<JString>*	itsFontNames;

private:

	// not allowed

	TestFontManager(const TestFontManager& source);
	const TestFontManager& operator=(const TestFontManager& source);
};

#endif
