/******************************************************************************
 TEFontManager.h

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TEFontManager
#define _H_TEFontManager

#include <JFontManager.h>

class TEFontManager : public JFontManager
{
public:

	TEFontManager();

	virtual ~TEFontManager();

	virtual void		GetFontNames(JPtrArray<JString>* fontNames) const;
	virtual void		GetMonospaceFontNames(JPtrArray<JString>* fontNames) const;
	virtual JBoolean	GetFontSizes(const JString& name, JSize* minSize,
									 JSize* maxSize, JArray<JSize>* sizeList) const;

protected:

	virtual JFontID			GetFontID(const JString& name, const JSize size,
									  const JFontStyle& style) const;
	virtual const JString&	GetFontName(const JFontID id) const;
	virtual JBoolean		IsExact(const JFontID id) const;

	virtual JSize	GetLineHeight(const JFontID fontID, const JSize size,
								  const JFontStyle& style,
								  JCoordinate* ascent, JCoordinate* descent) const;

	virtual JSize	GetCharWidth(const JFontID fontID, const JUtf8Character& c) const;
	virtual JSize	GetStringWidth(const JFontID fontID, const JString& str) const;

private:

	JPtrArray<JString>*	itsFontNames;

private:

	// not allowed

	TEFontManager(const TEFontManager& source);
	const TEFontManager& operator=(const TEFontManager& source);
};

#endif
