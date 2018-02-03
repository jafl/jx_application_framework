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

	Copyright (C) 1996-2000 by John Lindal.

 ******************************************************************************/

#include <JFontManager.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JFontManager::JFontManager()
	:
	itsDefaultFont(NULL),
	itsDefaultMonospaceFont(NULL)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JFontManager::~JFontManager()
{
	jdelete itsDefaultFont;
	jdelete itsDefaultMonospaceFont;
}

/******************************************************************************
 GetDefaultFont

 ******************************************************************************/

const JFont&
JFontManager::GetDefaultFont()
	const
{
	if (itsDefaultFont == NULL)
		{
		const_cast<JFontManager*>(this)->itsDefaultFont =
			jnew JFont(GetFont(JGetDefaultFontName()));
		assert( itsDefaultFont != NULL );
		}

	return *itsDefaultFont;
}

/******************************************************************************
 GetDefaultMonospaceFont

 ******************************************************************************/

const JFont&
JFontManager::GetDefaultMonospaceFont()
	const
{
	if (itsDefaultMonospaceFont == NULL)
		{
		const_cast<JFontManager*>(this)->itsDefaultMonospaceFont =
			jnew JFont(GetFont(JGetMonospaceFontName(), JGetDefaultMonoFontSize()));
		assert( itsDefaultMonospaceFont != NULL );
		}

	return *itsDefaultMonospaceFont;
}

/******************************************************************************
 GetFont

 ******************************************************************************/

JFont
JFontManager::GetFont
	(
	const JString&		name,
	const JSize			origSize,
	const JFontStyle	style
	)
	const
{
	const JSize size = origSize > 0 ? origSize : JGetDefaultFontSize();
	return JFont(this, GetFontID(name, size, style), size, style);
}
