/******************************************************************************
 JFontManager.h

	Interface for the JFontManager class.

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JFontManager
#define _H_JFontManager

#include <JPtrArray-JString.h>
#include <JFont.h>

class JString;

class JFontManager
{
	friend class JFont;

public:

	JFontManager();

	virtual ~JFontManager();

	virtual void		GetFontNames(JPtrArray<JString>* fontNames) const = 0;
	virtual void		GetMonospaceFontNames(JPtrArray<JString>* fontNames) const = 0;
	virtual JBoolean	GetFontSizes(const JString& name, JSize* minSize,
									 JSize* maxSize, JArray<JSize>* sizeList) const = 0;

	const JFont&	GetDefaultFont() const;
	const JFont&	GetDefaultMonospaceFont() const;
	JFont			GetFont(const JString& name,
							const JSize size = 0,
							const JFontStyle style = JFontStyle()) const;

protected:

	virtual JFontID			GetFontID(const JString& name, const JSize size,
									  const JFontStyle& style) const = 0;
	virtual const JString&	GetFontName(const JFontID id) const = 0;
	virtual JBoolean		IsExact(const JFontID id) const = 0;

	JSize	GetStrikeThickness(const JSize fontSize) const;
	JSize	GetUnderlineThickness(const JSize fontSize) const;

	virtual JSize	GetLineHeight(const JFontID fontID, const JSize size,
								  const JFontStyle& style,
								  JCoordinate* ascent, JCoordinate* descent) const = 0;

	virtual JSize	GetCharWidth(const JFontID fontID, const JUtf8Character& c) const = 0;
	virtual JSize	GetStringWidth(const JFontID fontID, const JString& str) const = 0;

private:

	JFont*	itsDefaultFont;
	JFont*	itsDefaultMonospaceFont;

private:

	// not allowed

	JFontManager(const JFontManager& source);
	const JFontManager& operator=(const JFontManager& source);
};


/******************************************************************************
 Get line thicknesses

 ******************************************************************************/

inline JSize
JFontManager::GetStrikeThickness
	(
	const JSize fontSize
	)
	const
{
	return (fontSize>=24 ? fontSize/12 : 1);
}

inline JSize
JFontManager::GetUnderlineThickness
	(
	const JSize fontSize
	)
	const
{
	return (1 + fontSize/32);
}

#endif
