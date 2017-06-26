/******************************************************************************
 TEFontManager.cpp

	BASE CLASS = JFontManager

	Written by John Lindal.

 ******************************************************************************/

#include "TEFontManager.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TEFontManager::TEFontManager()
	:
	JFontManager()
{
	itsFontNames = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsFontNames != NULL );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TEFontManager::~TEFontManager()
{
	jdelete itsFontNames;
}

/******************************************************************************
 GetFontNames (virtual)

 ******************************************************************************/

void
TEFontManager::GetFontNames
	(
	JPtrArray<JString>* fontNames
	)
	const
{
	fontNames->CopyObjects(*itsFontNames, JPtrArrayT::kDeleteAll, kJFalse);
}

/******************************************************************************
 GetMonospaceFontNames (virtual)

 ******************************************************************************/

void
TEFontManager::GetMonospaceFontNames
	(
	JPtrArray<JString>* fontNames
	)
	const
{
	fontNames->CopyObjects(*itsFontNames, JPtrArrayT::kDeleteAll, kJFalse);
}

/******************************************************************************
 GetFontSizes (virtual)

 ******************************************************************************/

JBoolean
TEFontManager::GetFontSizes
	(
	const JString&	name,
	JSize*			minSize,
	JSize*			maxSize,
	JArray<JSize>*	sizeList
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
TEFontManager::GetFontID
	(
	const JString&		name,
	const JSize			size,
	const JFontStyle&	style
	)
	const
{
	const JSize count = itsFontNames->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const JString* n = itsFontNames->GetElement(i);
		if (*n == name)
			{
			return i;
			}
		}

	// falling through means we need to create a new entry

	itsFontNames->Append(name);
	return itsFontNames->GetElementCount();
}

/******************************************************************************
 GetFontName (virtual)

 ******************************************************************************/

const JString&
TEFontManager::GetFontName
	(
	const JFontID id
	)
	const
{
	return *(itsFontNames->GetElement(id));
}

/******************************************************************************
 IsExact (virtual)

 ******************************************************************************/

JBoolean
TEFontManager::IsExact
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
TEFontManager::GetLineHeight
	(
	const JFontID		fontID,
	const JSize			size,
	const JFontStyle&	style,

	JCoordinate*		ascent,
	JCoordinate*		descent
	)
	const
{
	*ascent  = 5;
	*descent = 2;
	return (*ascent + *descent);
}

/******************************************************************************
 GetCharWidth (virtual)

 ******************************************************************************/

JSize
TEFontManager::GetCharWidth
	(
	const JFontID			fontID,
	const JUtf8Character&	c
	)
	const
{
	return 10;
}

/******************************************************************************
 GetStringWidth (virtual)

 ******************************************************************************/

JSize
TEFontManager::GetStringWidth
	(
	const JFontID	fontID,
	const JString&	str
	)
	const
{
	return 10*str.GetCharacterCount();
}
