/******************************************************************************
 JFontManager.cpp

	Abstract base class for accessing fonts.

	JFontID must be an index into the derived class' list of allocated
	fonts.  This way, we don't care how the system wants to reference a
	font, and 0 is guaranteed never to be used.

	*** Note that this definition means that the font ID is *not* independent
		of the size and style.

	Character sets are specified by appending them to the font name inside
	parentheses:  "Helvetica (iso2022-1)"  ExtractCharacterSet() is provided
	to separate the name and char set.  CombineNameAndCharacterSet()
	provides the inverse operation.

	Derived classes must implement the following routines:

		GetFontNames
			Return an alphabetical list of all the available font names.

		GetFontSizes
			Return min avail size, max avail size, and a sorted list of all the
			available font sizes for the given font name.  If all font sizes are
			supported (e.g. TrueType), return a reasonable min and max, and an
			empty list of sizes.  Return kJFalse if there is no such font.

		GetFontStyles
			Return JFontStyle with available styles set to kJTrue.  Because
			of JPainter, underline, strike, and colors are always available.

		GetFontCharSets
			Return character sets available for given font name and size.

		GetFontID
			Return a handle to the specified font.  This routine is responsible
			for finding the best approximation if the specified font is not available.

		GetFontName
			Return the name of the font with the given id.

		GetLineHeight
			Return the height of a line of text.

		GetCharWidth
			Return the width of the given character.

		GetStringWidth
			Return the width of the given string.

		IsExact
			Return kJTrue if the font matches the requested attributes.

	BASE CLASS = none

	Copyright © 1996-2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JFontManager.h>
#include <JRegex.h>
#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JFontManager::JFontManager()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JFontManager::~JFontManager()
{
}

/******************************************************************************
 GetStringWidth

 ******************************************************************************/

JSize
JFontManager::GetStringWidth
	(
	const JCharacter*	name,
	const JSize			size,
	const JFontStyle&	style,
	const JCharacter*	str
	)
	const
{
	return GetStringWidth(GetFontID(name, size, style), size, style, str, strlen(str));
}

JSize
JFontManager::GetStringWidth
	(
	const JFontID		fontID,
	const JSize			size,
	const JFontStyle&	style,
	const JCharacter*	str
	)
	const
{
	return GetStringWidth(fontID, size, style, str, strlen(str));
}

JSize
JFontManager::GetStringWidth
	(
	const JCharacter*	name,
	const JSize			size,
	const JFontStyle&	style,
	const JString&		str
	)
	const
{
	return GetStringWidth(GetFontID(name, size, style), size, style, str, str.GetLength());
}

JSize
JFontManager::GetStringWidth
	(
	const JFontID		fontID,
	const JSize			size,
	const JFontStyle&	style,
	const JString&		str
	)
	const
{
	return GetStringWidth(fontID, size, style, str, str.GetLength());
}

JSize
JFontManager::GetStringWidth
	(
	const JCharacter*	name,
	const JSize			size,
	const JFontStyle&	style,
	const JCharacter*	str,
	const JSize			charCount
	)
	const
{
	return GetStringWidth(GetFontID(name, size, style), size, style, str, charCount);
}

/******************************************************************************
 CombineNameAndCharacterSet (static)

 ******************************************************************************/

JString
JFontManager::CombineNameAndCharacterSet
	(
	const JCharacter* name,
	const JCharacter* charSet
	)
{
	JString s, s1;
	ExtractCharacterSet(name, &s, &s1);		// toss previous char set
	if (!JStringEmpty(charSet))
		{
		s += " (";
		s += charSet;
		s += ")";
		}
	return s;
}

/******************************************************************************
 ExtractCharacterSet (static)

	Returns kJTrue if *charSet is not empty.

 ******************************************************************************/

static const JRegex charSetRegex = "\\(.*\\)$";

JBoolean
JFontManager::ExtractCharacterSet
	(
	const JCharacter*	origName,
	JString*			fontName,
	JString*			charSet
	)
{
	*fontName = origName;
	charSet->Clear();

	JIndexRange r;
	if (charSetRegex.Match(*fontName, &r))
		{
		const JIndexRange r1(r.first+1, r.last-1);
		*charSet = fontName->GetSubstring(r1);
		fontName->RemoveSubstring(r);
		fontName->TrimWhitespace();
		}

	return !charSet->IsEmpty();
}
