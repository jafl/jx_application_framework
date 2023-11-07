/******************************************************************************
 JFontManager.cpp

	JFontManager::Init() must be called after initializing jGlobals.

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
			empty list of sizes.  Return false if there is no such font.

		GetLineHeight
			Return the height of a line of text.

		GetCharWidth
			Return the width of the given character.

		GetStringWidth
			Return the width of the given string.

		IsExact
			Return true if the font matches the requested attributes.

	Implementation details:

	JFontID is an index into the global list of allocated fonts.  This way,
	we don't care how the system wants to reference a font, and 0 is
	guaranteed never to be used.

	JFontID's are allocated as needed.  Specific implementations should keep
	a parallel list with system-dependent data and fill this list lazily.

	*** Note that this definition means that the font ID is *not* independent
		of the size and style.

	Nothing is const because any function might change the derived class'
	internal data.

	BASE CLASS = none

	Copyright (C) 1996-2018 by John Lindal.

 ******************************************************************************/

#include "JFontManager.h"
#include "jGlobals.h"
#include <stdlib.h>
#include <stdarg.h>
#include "jAssert.h"

JString	JFontManager::theDefaultFontName("Helvetica", JString::kNoCopy);
JSize	JFontManager::theDefaultFontSize    = 10;
JSize	JFontManager::theDefaultRCHFontSize = 9;

JString	JFontManager::theDefaultMonospaceFontName("Courier", JString::kNoCopy);
JSize	JFontManager::theDefaultMonospaceFontSize = 10;

JArray<JFontManager::Font> JFontManager::theFontList;

JFontID JFontManager::theDefaultFontID          = kInvalidFontID;
JFontID JFontManager::theDefaultMonospaceFontID = kInvalidFontID;

/******************************************************************************
 Constructor (protected)

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
		theDefaultFontID = GetFontID(theDefaultFontName, theDefaultFontSize, JFontStyle());
	}

	const Font f = theFontList.GetItem(theDefaultFontID);
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
		theDefaultMonospaceFontID = GetFontID(theDefaultMonospaceFontName, theDefaultMonospaceFontSize, JFontStyle());
	}

	const Font f = theFontList.GetItem(theDefaultMonospaceFontID);
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
	const JFontStyle&	style
	)
{
	const JSize size = origSize > 0 ? origSize : theDefaultFontSize;
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
	const JSize count = theFontList.GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		const Font f = theFontList.GetItem(i);
		if (*f.name == name && f.size == size && f.style.SameSystemAttributes(style))
		{
			return i;
		}
	}

	Font f;
	f.size  = size;
	f.style = style;

	f.name = jnew JString(name);
	assert( f.name != nullptr );

	theFontList.AppendItem(f);
	return theFontList.GetItemCount();
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
	const Font f = theFontList.GetItem(id);
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
	const Font f = theFontList.GetItem(id);
	return *f.name;
}

/******************************************************************************
 Init (static)

 ******************************************************************************/

static void
jParseSize
	(
	JStringManager*		stringMgr,
	const JUtf8Byte*	key,
	JSize*				value
	)
{
	if (stringMgr->Contains(key))
	{
		const JString& fontSize = JGetString(key);

		JSize size;
		if (fontSize.ConvertToUInt(&size))
		{
			*value = size;
		}
	}
}

void
JFontManager::Init
	(
	const JUtf8Byte* defaultFontName,
	const JUtf8Byte* defaultMonospaceFontName
	)
{
	// default font

	JStringManager* stringMgr = JGetStringManager();

	if (stringMgr->Contains("NAME::FONT"))
	{
		theDefaultFontName = JGetString("NAME::FONT");
	}
	else if (defaultFontName != nullptr)
	{
		theDefaultFontName.Set(defaultFontName);
	}

	jParseSize(stringMgr, "SIZE::FONT", &theDefaultFontSize);
	jParseSize(stringMgr, "SIZE::ROWCOLHDR::FONT", &theDefaultRCHFontSize);

	// monospace font

	if (stringMgr->Contains("NAME::MONO::FONT"))
	{
		theDefaultFontName = JGetString("NAME::MONO::FONT");
	}
	else if (defaultMonospaceFontName != nullptr)
	{
		theDefaultMonospaceFontName.Set(defaultMonospaceFontName);
	}

	jParseSize(stringMgr, "SIZE::MONO::FONT", &theDefaultMonospaceFontSize);

	// clean up

	atexit(CleanUp);
}

/******************************************************************************
 CleanUp (static private)

 ******************************************************************************/

void
JFontManager::CleanUp()
{
	for (const auto& f : theFontList)
	{
		jdelete f.name;
	}
}
