/******************************************************************************
 JFontManager.cpp

	Abstract base class for accessing fonts.

	Derived classes must implement the following functions:

		GetFontNames
			Return an alphabetical list of all the available font names.

		GetMonospaceFontNames
			Return an alphabetical list of all the available monospace font names.

		GetFontSizes
			Return min avail size, max avail size, and a sorted list of all the
			available font sizes for the given font name.  If all font sizes are
			supported (e.g. TrueType), return a reasonable min and max, and an
			empty list of sizes.  Return kJFalse if there is no such font.

		GetLineHeight
			Return the height of a line of text.

		GetCharWidth
			Return the width of the given character.

		GetStringWidth
			Return the width of the given string.

		IsExact
			Return kJTrue if the font matches the requested attributes.

	Implementation details:

	JFontID is an index into the global list of allocated fonts.  This way,
	we don't care how the system wants to reference a font, and 0 is
	guaranteed never to be used.

	JFontID's are allocated as needed.  Specific implementations should keep
	a parallel list with system-dependent data and fill this list lazily.

	*** Note that this definition means that the font ID is *not* independent
		of the size and style.

	BASE CLASS = none

	Copyright (C) 1996-2018 by John Lindal.

 ******************************************************************************/

#include <JFontManager.h>
#include <jGlobals.h>
#include <jAssert.h>

JArray<JFontManager::Font> JFontManager::theFontList;

JFontID JFontManager::theDefaultFontID          = kInvalidFontID;
JFontID JFontManager::theDefaultMonospaceFontID = kInvalidFontID;

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
 GetDefaultFont (static)

 ******************************************************************************/

JFont
JFontManager::GetDefaultFont()
{
	if (theDefaultFontID == kInvalidFontID)
		{
		theDefaultFontID = GetFontID(JGetDefaultFontName(), JGetDefaultFontSize(), JFontStyle());
		}

	const Font f = theFontList.GetElement(theDefaultFontID);
	return JFont(theDefaultFontID, f.size, f.style);
}

/******************************************************************************
 GetDefaultMonospaceFont (static)

 ******************************************************************************/

JFont
JFontManager::GetDefaultMonospaceFont()
{
	if (theDefaultMonospaceFontID == kInvalidFontID)
		{
		theDefaultMonospaceFontID = GetFontID(JGetDefaultMonospaceFontName(), JGetDefaultMonospaceFontSize(), JFontStyle());
		}

	const Font f = theFontList.GetElement(theDefaultMonospaceFontID);
	return JFont(theDefaultMonospaceFontID, f.size, f.style);
}

/******************************************************************************
 GetFont (static)

 ******************************************************************************/

JFont
JFontManager::GetFont
	(
	const JString&		name,
	const JSize			origSize,
	const JFontStyle	style
	)
{
	const JSize size = origSize > 0 ? origSize : JGetDefaultFontSize();
	return JFont(GetFontID(name, size, style), size, style);
}

/******************************************************************************
 GetFontID (static protected)

 ******************************************************************************/

JFontID
JFontManager::GetFontID
	(
	const JString&		name,
	const JSize			size,
	const JFontStyle&	style
	)
{
	const JSize count = theFontList.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const Font f = theFontList.GetElement(i);
		if (*f.name == name && f.size == size && f.style.SameSystemAttributes(style))
			{
			return i;
			}
		}

	Font f;
	f.size  = size;
	f.style = style;

	f.name = jnew JString(name);
	assert( f.name != NULL );

	theFontList.AppendElement(f);
	return theFontList.GetElementCount();
}

/******************************************************************************
 GetFont (static protected)

 ******************************************************************************/

JFont
JFontManager::GetFont
	(
	const JFontID id
	)
{
	const Font f = theFontList.GetElement(id);
	return JFont(id, f.size, f.style);
}

/******************************************************************************
 GetFontName (static protected)

 ******************************************************************************/

const JString&
JFontManager::GetFontName
	(
	const JFontID id
	)
{
	const Font f = theFontList.GetElement(id);
	return *f.name;
}
