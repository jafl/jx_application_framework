/******************************************************************************
 TestFontManager.cpp

	BASE CLASS = JFontManager

	Written by John Lindal.

 ******************************************************************************/

#include "TestFontManager.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TestFontManager::TestFontManager()
	:
	JFontManager()
{
	itsFontNames = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsFontNames != nullptr );

	itsFontNames->Append(JString("Courier", JString::kNoCopy));
	itsFontNames->Append(JString("Times", JString::kNoCopy));
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
	fontNames->CopyObjects(*itsFontNames, JPtrArrayT::kDeleteAll, false);
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
	fontNames->CopyObjects(*itsFontNames, JPtrArrayT::kDeleteAll, false);
}

/******************************************************************************
 GetFontSizes (virtual)

 ******************************************************************************/

bool
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
	return true;
}

/******************************************************************************
 IsExact (virtual protected)

 ******************************************************************************/

bool
TestFontManager::IsExact
	(
	const JFontID id
	)
{
	return true;
}

/******************************************************************************
 HasGlyphForCharacter (virtual protected)

 ******************************************************************************/

bool
TestFontManager::HasGlyphForCharacter
	(
	const JFontID			id,
	const JUtf8Character&	c
	)
{
	return true;
}

/******************************************************************************
 GetSubstituteFontName (virtual protected)

 ******************************************************************************/

bool
TestFontManager::GetSubstituteFontName
	(
	const JFont&			f,
	const JUtf8Character&	c,
	JString*				name
	)
{
	return false;
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
