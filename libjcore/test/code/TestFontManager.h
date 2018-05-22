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

	virtual void		GetFontNames(JPtrArray<JString>* fontNames) const;
	virtual void		GetMonospaceFontNames(JPtrArray<JString>* fontNames) const;
	virtual JBoolean	GetFontSizes(const JString& name, JSize* minSize,
									 JSize* maxSize, JArray<JSize>* sizeList) const;

	virtual JBoolean	IsExact(const JFontID id) const;

protected:

	virtual JSize	GetLineHeight(const JFontID fontID, const JSize size,
								  const JFontStyle& style,
								  JCoordinate* ascent, JCoordinate* descent) const;

	virtual JSize	GetCharWidth(const JFontID fontID, const JUtf8Character& c) const;
	virtual JSize	GetStringWidth(const JFontID fontID, const JString& str) const;

private:

	JPtrArray<JString>*	itsFontNames;

private:

	// not allowed

	TestFontManager(const TestFontManager& source);
	const TestFontManager& operator=(const TestFontManager& source);
};

#endif
