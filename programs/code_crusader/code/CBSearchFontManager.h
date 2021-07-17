/******************************************************************************
 CBSearchFontManager.h

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBSearchFontManager
#define _H_CBSearchFontManager

#include <JFontManager.h>

class CBSearchFontManager : public JFontManager
{
public:

	CBSearchFontManager();

	virtual ~CBSearchFontManager();

	virtual void		GetFontNames(JPtrArray<JString>* fontNames) override;
	virtual void		GetMonospaceFontNames(JPtrArray<JString>* fontNames) override;
	virtual bool	GetFontSizes(const JString& name, JSize* minSize,
									 JSize* maxSize, JArray<JSize>* sizeList) override;

protected:

	virtual JSize	GetLineHeight(const JFontID fontID, const JSize size,
								  const JFontStyle& style,
								  JCoordinate* ascent, JCoordinate* descent) override;

	virtual JSize	GetCharWidth(const JFontID fontID, const JUtf8Character& c) override;
	virtual JSize	GetStringWidth(const JFontID fontID, const JString& str) override;

	virtual bool	IsExact(const JFontID id) override;
	virtual bool	HasGlyphForCharacter(const JFontID id, const JUtf8Character& c) override;
	virtual bool	GetSubstituteFontName(const JFont& f, const JUtf8Character& c, JString* name) override;

private:

	// not allowed

	CBSearchFontManager(const CBSearchFontManager& source);
	const CBSearchFontManager& operator=(const CBSearchFontManager& source);
};

#endif
