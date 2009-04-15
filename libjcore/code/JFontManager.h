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

#include <JPtrArray.h>
#include <JFontStyle.h>

class JString;

class JFontManager
{
public:

	JFontManager();

	virtual ~JFontManager();

	virtual void		GetFontNames(JPtrArray<JString>* fontNames) const = 0;
	virtual void		GetMonospaceFontNames(JPtrArray<JString>* fontNames) const = 0;
	virtual JBoolean	GetFontSizes(const JCharacter* name, JSize* minSize,
									 JSize* maxSize, JArray<JSize>* sizeList) const = 0;
	virtual JFontStyle	GetFontStyles(const JCharacter* name, const JSize size) const = 0;
	virtual JBoolean	GetFontCharSets(const JCharacter* name, const JSize size,
										JPtrArray<JString>* charSetList) const = 0;

	virtual JFontID				GetFontID(const JCharacter* name, const JSize size,
										  const JFontStyle& style) const = 0;
	virtual const JCharacter*	GetFontName(const JFontID id) const = 0;
	virtual JBoolean			IsExact(const JFontID id) const = 0;

	JFontID	UpdateFontID(const JFontID fontID, const JSize size,
						 const JFontStyle& style) const;

	JSize	GetStrikeThickness(const JSize fontSize) const;
	JSize	GetUnderlineThickness(const JSize fontSize) const;

	JSize			GetLineHeight(const JCharacter* name, const JSize size,
								  const JFontStyle& style,
								  JCoordinate* ascent, JCoordinate* descent) const;
	virtual JSize	GetLineHeight(const JFontID fontID, const JSize size,
								  const JFontStyle& style,
								  JCoordinate* ascent, JCoordinate* descent) const = 0;

	JSize			GetLineHeight(const JCharacter* name, const JSize size,
								  const JFontStyle& style) const;
	JSize			GetLineHeight(const JFontID fontID, const JSize size,
								  const JFontStyle& style) const;

	JSize			GetCharWidth(const JCharacter* name, const JSize size,
								 const JFontStyle& style, const JCharacter c) const;
	virtual JSize	GetCharWidth(const JFontID fontID, const JSize size,
								 const JFontStyle& style, const JCharacter c) const = 0;

	JSize			GetStringWidth(const JCharacter* name, const JSize size,
								   const JFontStyle& style, const JCharacter* str) const;
	JSize			GetStringWidth(const JFontID fontID, const JSize size,
								   const JFontStyle& style, const JCharacter* str) const;

	JSize			GetStringWidth(const JCharacter* name, const JSize size,
								   const JFontStyle& style, const JString& str) const;
	JSize			GetStringWidth(const JFontID fontID, const JSize size,
								   const JFontStyle& style, const JString& str) const;

	JSize			GetStringWidth(const JCharacter* name, const JSize size,
								   const JFontStyle& style, const JCharacter* str,
								   const JSize charCount) const;
	virtual JSize	GetStringWidth(const JFontID fontID, const JSize size,
								   const JFontStyle& style, const JCharacter* str,
								   const JSize charCount) const = 0;

	static JString	CombineNameAndCharacterSet(const JCharacter* name,
											   const JCharacter* charSet);
	static JBoolean	ExtractCharacterSet(const JCharacter* origName,
										JString* fontName, JString* charSet);

private:

	// not allowed

	JFontManager(const JFontManager& source);
	const JFontManager& operator=(const JFontManager& source);
};


/******************************************************************************
 UpdateFontID

	Convenience function to recalculate the font ID after changing the size
	or style.

 ******************************************************************************/

inline JFontID
JFontManager::UpdateFontID
	(
	const JFontID		fontID,
	const JSize			size,
	const JFontStyle&	style
	)
	const
{
	return GetFontID(GetFontName(fontID), size, style);
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

/******************************************************************************
 GetLineHeight

 ******************************************************************************/

inline JSize
JFontManager::GetLineHeight
	(
	const JCharacter*	name,
	const JSize			size,
	const JFontStyle&	style,

	JCoordinate*		ascent,
	JCoordinate*		descent
	)
	const
{
	return GetLineHeight(GetFontID(name, size, style), size, style, ascent, descent);
}

inline JSize
JFontManager::GetLineHeight
	(
	const JCharacter*	name,
	const JSize			size,
	const JFontStyle&	style
	)
	const
{
	JCoordinate ascent, descent;
	return GetLineHeight(GetFontID(name, size, style), size, style, &ascent, &descent);
}

inline JSize
JFontManager::GetLineHeight
	(
	const JFontID		fontID,
	const JSize			size,
	const JFontStyle&	style
	)
	const
{
	JCoordinate ascent, descent;
	return GetLineHeight(fontID, size, style, &ascent, &descent);
}

/******************************************************************************
 GetCharWidth

 ******************************************************************************/

inline JSize
JFontManager::GetCharWidth
	(
	const JCharacter*	name,
	const JSize			size,
	const JFontStyle&	style,
	const JCharacter	c
	)
	const
{
	return GetCharWidth(GetFontID(name, size, style), size, style, c);
}

#endif
