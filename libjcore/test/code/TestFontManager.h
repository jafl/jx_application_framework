/******************************************************************************
 TestFontManager.h

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestFontManager
#define _H_TestFontManager

#include <jx-af/jcore/JFontManager.h>

class TestFontManager : public JFontManager
{
public:

	TestFontManager();

	~TestFontManager() override;

	void	GetFontNames(JPtrArray<JString>* fontNames) override;
	void	GetMonospaceFontNames(JPtrArray<JString>* fontNames) override;
	bool	GetFontSizes(const JString& name, JSize* minSize,
						 JSize* maxSize, JArray<JSize>* sizeList) override;

protected:

	JSize	GetLineHeight(const JFontID fontID, const JSize size,
						  const JFontStyle& style,
						  JCoordinate* ascent, JCoordinate* descent) override;

	JSize	GetCharWidth(const JFontID fontID, const JUtf8Character& c) override;
	JSize	GetStringWidth(const JFontID fontID, const JString& str) override;

	bool	IsExact(const JFontID id) override;
	bool	HasGlyphForCharacter(const JFontID id, const JUtf8Character& c) override;
	bool	GetSubstituteFontName(const JFont& f, const JUtf8Character& c, JString* name) override;

private:

	JPtrArray<JString>*	itsFontNames;
};

#endif
