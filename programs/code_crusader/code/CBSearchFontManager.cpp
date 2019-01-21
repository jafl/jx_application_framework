/******************************************************************************
 CBSearchFontManager.cpp

	BASE CLASS = JFontManager

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "CBSearchFontManager.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBSearchFontManager::CBSearchFontManager()
	:
	JFontManager()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBSearchFontManager::~CBSearchFontManager()
{
}

/******************************************************************************
 GetFontNames (virtual)

 ******************************************************************************/

void
CBSearchFontManager::GetFontNames
	(
	JPtrArray<JString>* fontNames
	)
{
	fontNames->CleanOut();
}

/******************************************************************************
 GetMonospaceFontNames (virtual)

 ******************************************************************************/

void
CBSearchFontManager::GetMonospaceFontNames
	(
	JPtrArray<JString>* fontNames
	)
{
	fontNames->CleanOut();
}

/******************************************************************************
 GetFontSizes (virtual)

 ******************************************************************************/

JBoolean
CBSearchFontManager::GetFontSizes
	(
	const JString&	name,
	JSize*			minSize,
	JSize*			maxSize,
	JArray<JSize>*	sizeList
	)
{
	*minSize = *maxSize = 0;
	sizeList->RemoveAll();
	return kJFalse;
}

/******************************************************************************
 GetLineHeight (virtual)

 ******************************************************************************/

JSize
CBSearchFontManager::GetLineHeight
	(
	const JFontID		fontID,
	const JSize			size,
	const JFontStyle&	style,

	JCoordinate*		ascent,
	JCoordinate*		descent
	)
{
	*ascent  = 1;
	*descent = 1;
	return (*ascent + *descent);
}

/******************************************************************************
 GetCharWidth (virtual)

 ******************************************************************************/

JSize
CBSearchFontManager::GetCharWidth
	(
	const JFontID			fontID,
	const JUtf8Character&	c
	)
{
	return 1;
}

/******************************************************************************
 GetStringWidth (virtual)

 ******************************************************************************/

JSize
CBSearchFontManager::GetStringWidth
	(
	const JFontID	fontID,
	const JString&	str
	)
{
	return str.GetCharacterCount();
}

/******************************************************************************
 IsExact (virtual)

 ******************************************************************************/

JBoolean
CBSearchFontManager::IsExact
	(
	const JFontID id
	)
{
	return kJTrue;
}

/******************************************************************************
 HasGlyphForCharacter (virtual)

 ******************************************************************************/

JBoolean
CBSearchFontManager::HasGlyphForCharacter
	(
	const JFontID			id,
	const JUtf8Character&	c
	)
{
	return kJTrue;
}

/******************************************************************************
 GetSubstituteFontName (virtual protected)

 ******************************************************************************/

JBoolean
CBSearchFontManager::GetSubstituteFontName
	(
	const JFont&			f,
	const JUtf8Character&	c,
	JString*				name
	)
{
	return kJFalse;
}
