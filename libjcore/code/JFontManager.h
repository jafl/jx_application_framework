/******************************************************************************
 JFontManager.h

	Interface for the JFontManager class.

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JFontManager
#define _H_JFontManager

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

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
	virtual JBoolean	GetFontSizes(const JCharacter* name, JSize* minSize,
									 JSize* maxSize, JArray<JSize>* sizeList) const = 0;

	const JFont&	GetDefaultFont() const;
	const JFont&	GetDefaultMonospaceFont() const;
	JFont			GetFont(const JCharacter* name,
							const JSize size = kJDefaultFontSize,
							const JFontStyle style = JFontStyle()) const;
	JFont			GetFont(const JString& name,
							const JSize size = kJDefaultFontSize,
							const JFontStyle style = JFontStyle()) const;

protected:

	virtual JFontID				GetFontID(const JCharacter* name, const JSize size,
										  const JFontStyle& style) const = 0;
	virtual const JCharacter*	GetFontName(const JFontID id) const = 0;
	virtual JBoolean			IsExact(const JFontID id) const = 0;

	JSize	GetStrikeThickness(const JSize fontSize) const;
	JSize	GetUnderlineThickness(const JSize fontSize) const;

	virtual JSize	GetLineHeight(const JFontID fontID, const JSize size,
								  const JFontStyle& style,
								  JCoordinate* ascent, JCoordinate* descent) const = 0;

	virtual JSize	GetCharWidth(const JFontID fontID, const JCharacter c) const = 0;

	virtual JSize	GetStringWidth(const JFontID fontID, const JCharacter* str,
								   const JSize charCount) const = 0;

private:

	JFont*	itsDefaultFont;
	JFont*	itsDefaultMonospaceFont;

private:

	// not allowed

	JFontManager(const JFontManager& source);
	const JFontManager& operator=(const JFontManager& source);
};


/******************************************************************************
 GetFont

 ******************************************************************************/

inline JFont
JFontManager::GetFont
	(
	const JCharacter*	name,
	const JSize			size,
	const JFontStyle	style
	)
	const
{
	return JFont(this, GetFontID(name, size, style), size, style);
}

inline JFont
JFontManager::GetFont
	(
	const JString&		name,
	const JSize			size,
	const JFontStyle	style
	)
	const
{
	return JFont(this, GetFontID(name, size, style), size, style);
}

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
