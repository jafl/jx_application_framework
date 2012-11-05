/******************************************************************************
 CBSearchFontManager.cpp

	BASE CLASS = JFontManager

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cbStdInc.h>
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
	const
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
	const
{
	fontNames->CleanOut();
}

/******************************************************************************
 GetFontSizes (virtual)

 ******************************************************************************/

JBoolean
CBSearchFontManager::GetFontSizes
	(
	const JCharacter*	name,
	JSize*				minSize,
	JSize*				maxSize,
	JArray<JSize>*		sizeList
	)
	const
{
	*minSize = *maxSize = 0;
	sizeList->RemoveAll();
	return kJFalse;
}

/******************************************************************************
 GetFontID (virtual)

 ******************************************************************************/

JFontID
CBSearchFontManager::GetFontID
	(
	const JCharacter*	name,
	const JSize			size,
	const JFontStyle&	style
	)
	const
{
	return 0;
}

/******************************************************************************
 GetFontName (virtual)

 ******************************************************************************/

const JCharacter*
CBSearchFontManager::GetFontName
	(
	const JFontID id
	)
	const
{
	return "font";
}

/******************************************************************************
 GetFontName (virtual)

 ******************************************************************************/

JBoolean
CBSearchFontManager::IsExact
	(
	const JFontID id
	)
	const
{
	return kJTrue;
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
	const
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
	const JFontID		fontID,
	const JSize			size,
	const JFontStyle&	style,
	const JCharacter	c
	)
	const
{
	return 1;
}

/******************************************************************************
 GetStringWidth (virtual)

 ******************************************************************************/

JSize
CBSearchFontManager::GetStringWidth
	(
	const JFontID		fontID,
	const JSize			size,
	const JFontStyle&	style,
	const JCharacter*	str,
	const JSize			charCount
	)
	const
{
	return charCount;
}
