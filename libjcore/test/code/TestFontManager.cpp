/******************************************************************************
 TestFontManager.cpp

	BASE CLASS = JFontManager

	Written by John Lindal.

 ******************************************************************************/

#include "TestFontManager.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TestFontManager::TestFontManager()
	:
	JFontManager()
{
	itsFontNames = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsFontNames != nullptr );

	itsFontNames->Append(JString("Courier", 0, kJFalse));
	itsFontNames->Append(JString("Times", 0, kJFalse));
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TestFontManager::~TestFontManager()
{
	jdelete itsFontNames;
}

/******************************************************************************
 GetFontNames (virtual)

 ******************************************************************************/

void
TestFontManager::GetFontNames
	(
	JPtrArray<JString>* fontNames
	)
{
	fontNames->CopyObjects(*itsFontNames, JPtrArrayT::kDeleteAll, kJFalse);
}

/******************************************************************************
 GetMonospaceFontNames (virtual)

 ******************************************************************************/

void
TestFontManager::GetMonospaceFontNames
	(
	JPtrArray<JString>* fontNames
	)
{
	fontNames->CopyObjects(*itsFontNames, JPtrArrayT::kDeleteAll, kJFalse);
}

/******************************************************************************
 GetFontSizes (virtual)

 ******************************************************************************/

JBoolean
TestFontManager::GetFontSizes
	(
	const JString&	name,
	JSize*			minSize,
	JSize*			maxSize,
	JArray<JSize>*	sizeList
	)
{
	*minSize = 8;
	*maxSize = 24;
	sizeList->RemoveAll();
	return kJTrue;
}

/******************************************************************************
 IsExact (virtual protected)

 ******************************************************************************/

JBoolean
TestFontManager::IsExact
	(
	const JFontID id
	)
{
	return kJTrue;
}

/******************************************************************************
 HasGlyphForCharacter (virtual protected)

 ******************************************************************************/

JBoolean
TestFontManager::HasGlyphForCharacter
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
TestFontManager::GetSubstituteFontName
	(
	const JFont&			f,
	const JUtf8Character&	c,
	JString*				name
	)
{
	return kJFalse;
}

/******************************************************************************
 GetLineHeight (virtual)

 ******************************************************************************/

JSize
TestFontManager::GetLineHeight
	(
	const JFontID		fontID,
	const JSize			size,
	const JFontStyle&	style,

	JCoordinate*		ascent,
	JCoordinate*		descent
	)
{
	*ascent  = 5;
	*descent = 2;
	return (*ascent + *descent);
}

/******************************************************************************
 GetCharWidth (virtual)

 ******************************************************************************/

JSize
TestFontManager::GetCharWidth
	(
	const JFontID			fontID,
	const JUtf8Character&	c
	)
{
	return 7;
}

/******************************************************************************
 GetStringWidth (virtual)

 ******************************************************************************/

JSize
TestFontManager::GetStringWidth
	(
	const JFontID	fontID,
	const JString&	str
	)
{
	return 7*str.GetCharacterCount();
}
