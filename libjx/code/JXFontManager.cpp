/******************************************************************************
 JXFontManager.cpp

	http://keithp.com/~keithp/render/Xft.tutorial
	http://www.mail-archive.com/render@xfree86.org/msg00131.html
	http://linux.about.com/library/cmd/blcmdl3_fontconfig.htm

	Unicode:
		http://jrgraphix.net/research/unicode_blocks.php

	iso8859:
		http://czyborra.com/charsets/iso8859.html

	BASE CLASS = JFontManager

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXFontManager.h>
#include <JXDisplay.h>
#include <JXColormap.h>
#include <jXGlobals.h>
#include <JPtrArray-JString.h>
#include <JOrderedSetUtil.h>
#include <JMinMax.h>
#include <jStreamUtil.h>
#include <jMath.h>
#include <JRegex.h>
#include <sstream>
#include <string.h>
#include <ctype.h>
#include <jAssert.h>

#define ENABLE_TRUE_TYPE	1
#define ONLY_STD_PS_FONTS	1
#define ONLY_STD_MONOSPACE	1

static const JCharacter* kItalicStr  = "-i";
static const JCharacter* kObliqueStr = "-o";

static const JRegex nxmRegex = "^[0-9]+x[0-9]+$";

#if FC_MAJOR < 2 || (FC_MAJOR == 2 && FC_MINOR < 3)
	#define FcStrFree free
#endif

/******************************************************************************
 Constructor

 ******************************************************************************/

JXFontManager::JXFontManager
	(
	JXDisplay* display
	)
	:
	JFontManager()
{
	itsDisplay = display;

	itsFontList = new JArray<FontInfo>;
	assert( itsFontList != NULL );

	itsAllFontNames  = NULL;
	itsMonoFontNames = NULL;

	JFontStyle::SetDefaultColorIndex((display->GetColormap())->GetBlackColor());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXFontManager::~JXFontManager()
{
	const JSize count = itsFontList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		FontInfo info = itsFontList->GetElement(i);
		delete info.name;
		info.xfont.Free(itsDisplay);
		}
	delete itsFontList;

	delete itsAllFontNames;
	delete itsMonoFontNames;
}

/******************************************************************************
 IsMonospace (private)

	Returns monospace width or zero.

 ******************************************************************************/

inline JCoordinate
JXFontManager::IsMonospace
	(
	const XFont& xfont
	)
	const
{
	if (xfont.type == kStdType)
	{
		return (xfont.xfstd->min_bounds.width == xfont.xfstd->max_bounds.width ?
				xfont.xfstd->min_bounds.width : 0);
	}
	else
	{
		assert( xfont.type == kTrueType );

		XGlyphInfo g1, g2;
		XftTextExtents8(*itsDisplay, xfont.xftrue, (FcChar8*) "M", 1, &g1);
		XftTextExtents8(*itsDisplay, xfont.xftrue, (FcChar8*) "|", 1, &g2);
		return (g1.xOff == g2.xOff ? g1.xOff : 0);
	}
}

/******************************************************************************
 IsPostscript (private)

	Returns kJTrue if the font is available in Postscript.

	Until JPSPrinter can embed fonts in a Postscript file, we are limited
	to only the standard Postscript fonts.

 ******************************************************************************/

inline JBoolean
JXFontManager::IsPostscript
	(
	const JString& name
	)
	const
{
#if ONLY_STD_PS_FONTS

	return JI2B(name == "Arial"                     ||	// Helvetica sucks on OS X
				name.BeginsWith("Courier")          ||
				name.BeginsWith("Helvetica")        ||
				name == "Symbol"                    ||
				name == "Times"                     ||
				name.Contains("Bookman")            ||
				name.Contains("Century Schoolbook") ||
				name.Contains("Chancery")           ||
				name.Contains("Palatino"));

#else

	return kJTrue;

#endif
}

/******************************************************************************
 IsUseless (private)

	Returns kJTrue if the font is useless, i.e., no printing characters.

 ******************************************************************************/

inline JBoolean
JXFontManager::IsUseless
	(
	const JString& name
	)
	const
{
	return JI2B(name.Contains("Dingbats")         ||
				name.Contains("Standard Symbols") ||
				name.Contains("Cursor")           ||
				name.EndsWith(" Ti"));
}

/******************************************************************************
 GetFontNames (virtual)

	Returns the subset of available fonts that can be printed.

 ******************************************************************************/

void
JXFontManager::GetFontNames
	(
	JPtrArray<JString>* fontNames
	)
	const
{
	if (itsAllFontNames != NULL)
		{
		fontNames->CopyObjects(*itsAllFontNames, fontNames->GetCleanUpAction(), kJFalse);
		}
	else
		{
		fontNames->CleanOut();
		fontNames->SetCompareFunction(JCompareStringsCaseInsensitive);
		fontNames->SetSortOrder(JOrderedSetT::kSortAscending);

		JString name;

#if ENABLE_TRUE_TYPE

		FcFontSet* set = XftListFonts(*itsDisplay, itsDisplay->GetScreen(), NULL, FC_FAMILY, NULL);
		for (int i=0; i < set->nfont; i++)
			{
			FcChar8* s = FcNameUnparse(set->fonts[i]);
			name.Set((JCharacter*) s);
//			cout << "tt  font: " << name << endl;

			if (!IsPostscript(name) || IsUseless(name))
				{
				FcStrFree(s);
				continue;
				}

			JBoolean isDuplicate;
			const JIndex index = fontNames->GetInsertionSortIndex(&name, &isDuplicate);
			if (!isDuplicate)
				{
				fontNames->InsertAtIndex(index, name);
				}

			FcStrFree(s);
			}
		FcFontSetDestroy(set);

#endif

		int nameCount;
		char** nameList = XListFonts(*itsDisplay, "-*-*-medium-r-normal-*-*-*-75-75-*-*-*-*",
									 INT_MAX, &nameCount);
		if (nameList == NULL)
			{
			return;
			}

		for (int i=0; i<nameCount; i++)
			{
			const std::string s(nameList[i], strlen(nameList[i]));
			std::istringstream input(s);
			input.ignore();						// initial dash
			JIgnoreUntil(input, '-');			// foundry name
			name = JReadUntil(input, '-');		// font name

			if (name.IsEmpty() || name == "nil")
				{
				continue;
				}

			ConvertToPSFontName(&name);
//			cout << "std font: " << name << endl;

			if (!IsPostscript(name) || IsUseless(name))
				{
				continue;
				}

			JBoolean isDuplicate;
			const JIndex index = fontNames->GetInsertionSortIndex(&name, &isDuplicate);
			if (!isDuplicate)
				{
				fontNames->InsertAtIndex(index, name);
				}
			}

		XFreeFontNames(nameList);

		// save names for next time

		itsAllFontNames = new JDCCPtrArray<JString>(*fontNames, JPtrArrayT::kDeleteAll);
		assert( itsAllFontNames != NULL );
		}
}

/******************************************************************************
 GetMonospaceFontNames (virtual)

 ******************************************************************************/

static const JCharacter* kMonospaceFontPattern[] =
{
	"-*-*-medium-r-normal-*-*-*-72-72-*-*-*-*",
	"-*-*-medium-r-normal-*-*-*-75-75-*-*-iso*-*",
};

const int kMonospaceFontPatternCount =
	sizeof(kMonospaceFontPattern) / sizeof(JCharacter*);

void
JXFontManager::GetMonospaceFontNames
	(
	JPtrArray<JString>* fontNames
	)
	const
{
	if (itsMonoFontNames != NULL)
		{
		fontNames->CopyObjects(*itsMonoFontNames, fontNames->GetCleanUpAction(), kJFalse);
		}
	else
		{
		(JXGetApplication())->DisplayBusyCursor();

		fontNames->CleanOut();
		fontNames->SetCompareFunction(JCompareStringsCaseInsensitive);
		fontNames->SetSortOrder(JOrderedSetT::kSortAscending);

		JPtrArray<JString> allFontNames(JPtrArrayT::kDeleteAll);
		allFontNames.SetCompareFunction(JCompareStringsCaseInsensitive);
		allFontNames.SetSortOrder(JOrderedSetT::kSortAscending);

		JString name;

#if ENABLE_TRUE_TYPE

		FcFontSet* set =
			XftListFonts(*itsDisplay, itsDisplay->GetScreen(),
						 FC_SPACING, FcTypeInteger, FC_MONO, NULL,
						 FC_FAMILY, NULL);
		for (int i=0; i < set->nfont; i++)
			{
			FcChar8* s = FcNameUnparse(set->fonts[i]);
			name.Set((JCharacter*) s);
//			cout << "tt  mono: " << name << endl;

#if ONLY_STD_MONOSPACE

			if (!name.BeginsWith("Courier") &&
				!name.BeginsWith("Consolas") &&
				!name.Contains(" Mono")     &&
				name != "LucidaTypewriter")
				{
				FcStrFree(s);
				continue;
				}

#endif

			if (IsUseless(name))
				{
				FcStrFree(s);
				continue;
				}

			JBoolean isDuplicate;
			const JIndex index =
				allFontNames.GetInsertionSortIndex(&name, &isDuplicate);
			if (!isDuplicate)
				{
				allFontNames.InsertAtIndex(index, name);

				JString* n = new JString(name);
				assert( n != NULL );
				const JBoolean ok = fontNames->InsertSorted(n, kJFalse);
				assert( ok );
				}

			FcStrFree(s);
			}
		FcFontSetDestroy(set);

#else

		for (int j=0; j<kMonospaceFontPatternCount; j++)
			{
			int nameCount;
			char** nameList = XListFonts(*itsDisplay, kMonospaceFontPattern[j],
										 INT_MAX, &nameCount);
			if (nameList == NULL)
				{
				return;
				}

			for (int i=0; i<nameCount; i++)
				{
				const std::string s(nameList[i], strlen(nameList[i]));
				std::istringstream input(s);
				input.ignore();						// initial dash
				JIgnoreUntil(input, '-');			// foundry name
				name = JReadUntil(input, '-');		// font name

				if (name.IsEmpty() || name == "nil")
					{
					continue;
					}

				ConvertToPSFontName(&name);
//				cout << "std mono: " << name << endl;

#if ONLY_STD_MONOSPACE

				if (name != "Clean" && name != "Fixed" && name != "Terminal" &&
					name != "Courier" && name != "Lucidatypewriter" &&
					name != "Profontwindows")
					{
					continue;
					}

#endif

				if (IsUseless(name))
					{
					continue;
					}

				JBoolean isDuplicate;
				const JIndex index =
					allFontNames.GetInsertionSortIndex(&name, &isDuplicate);
				if (!isDuplicate)
					{
					allFontNames.InsertAtIndex(index, name);

					XFontStruct* xfont = XLoadQueryFont(*itsDisplay, nameList[i]);
					if (xfont != NULL)
						{
						if (xfont->min_bounds.width == xfont->max_bounds.width)
							{
							JString* n = new JString(name);
							assert( n != NULL );
							const JBoolean ok = fontNames->InsertSorted(n, kJFalse);
							assert( ok );
							}
						XFreeFont(*itsDisplay, xfont);
						}
					}
				}

			XFreeFontNames(nameList);
			}

#endif

		// save names for next time

		itsMonoFontNames = new JDCCPtrArray<JString>(*fontNames, JPtrArrayT::kDeleteAll);
		assert( itsMonoFontNames != NULL );
		}
}

/******************************************************************************
 GetXFontNames

	This provides raw output from XListFonts().  For example, ^[0-9]x[0-9]+$
	would yield a list of clean, monospace fonts.

	If compare is NULL, we use JCompareStringsCaseInsensitive().

 ******************************************************************************/

void
JXFontManager::GetXFontNames
	(
	const JRegex&		regex,
	JPtrArray<JString>*	fontNames,
	JSortXFontNamesFn	compare
	)
	const
{
	fontNames->CleanOut();
	fontNames->SetCompareFunction(
					compare != NULL ? compare : JCompareStringsCaseInsensitive);
	fontNames->SetSortOrder(JOrderedSetT::kSortAscending);

	int nameCount;
	char** nameList = XListFonts(*itsDisplay, "*", INT_MAX, &nameCount);
	if (nameList == NULL)
		{
		return;
		}

	for (int i=0; i<nameCount; i++)
		{
		if (regex.Match(nameList[i]) && strcmp(nameList[i], "nil") != 0)
			{
			JString name = nameList[i];
			JBoolean isDuplicate;
			const JIndex index =
				fontNames->GetInsertionSortIndex(&name, &isDuplicate);
			if (!isDuplicate)
				{
				JString* n = new JString(name);
				assert( n != NULL );
				fontNames->InsertAtIndex(index, n);
				}
			}
		}

	XFreeFontNames(nameList);
}

/******************************************************************************
 GetFontSizes (virtual)

	If all font sizes are supported (e.g. TrueType), returns reasonable
	min and max, and empty sizeList.

	Returns kJFalse if there is no font with the specified name.

 ******************************************************************************/

JBoolean
JXFontManager::GetFontSizes
	(
	const JCharacter*	name,
	JSize*				minSize,
	JSize*				maxSize,
	JArray<JSize>*		sizeList
	)
	const
{
	sizeList->RemoveAll();

#if ENABLE_TRUE_TYPE

	*minSize = 8;
	*maxSize = 24;
	return kJTrue;

#else

	*minSize = *maxSize = 0;
	sizeList->SetCompareFunction(JCompareSizes);
	sizeList->SetSortOrder(JOrderedSetT::kSortAscending);

	const JString xFontName = ConvertToXFontName(name)

	// check for rescalable font

	JString regexStr = "-*-" + xFontName + "-medium-r-normal-*-*-0-75-75-*-*-*";

	int nameCount;
	char** nameList = XListFonts(*itsDisplay, regexStr, INT_MAX, &nameCount);
	if (nameList != NULL)
		{
		*minSize = 8;
		*maxSize = 24;
		XFreeFontNames(nameList);
		return kJTrue;
		}

	// get list of available sizes

	regexStr = "-*-" + xFontName + "-medium-r-normal-*-*-*-75-75-*-*-*";

	nameList = XListFonts(*itsDisplay, regexStr, INT_MAX, &nameCount);
	if (nameList == NULL)
		{
		return kJFalse;
		}

	JSize fontSize;
	for (int i=0; i<nameCount; i++)
		{
		const std::string s(nameList[i], strlen(nameList[i]));
		std::istringstream input(s);
		input.ignore();							// initial dash
		JIgnoreUntil(input, '-');				// foundry name
		JIgnoreUntil(input, '-');				// font name
		JIgnoreUntil(input, '-');				// medium/bold
		JIgnoreUntil(input, '-');				// roman/oblique/italic
		JIgnoreUntil(input, '-');				// character spacing
		JIgnoreUntil(input, '-');				// sans
		JIgnoreUntil(input, '-');				// pixel height
		input >> fontSize;						// 10*(point size)

		if (fontSize < 10)
			{
			continue;		// we already checked for rescalable version
			}

		fontSize /= 10;
		if (sizeList->IsEmpty())
			{
			*minSize = *maxSize = fontSize;
			}

		JBoolean isDuplicate;
		const JIndex index =
			sizeList->GetInsertionSortIndex(fontSize, &isDuplicate);
		if (!isDuplicate)
			{
			sizeList->InsertElementAtIndex(index, fontSize);

			if (fontSize < *minSize)
				{
				*minSize = fontSize;
				}
			else if (fontSize > *maxSize)
				{
				*maxSize = fontSize;
				}
			}
		}

	XFreeFontNames(nameList);

#endif

	return !sizeList->IsEmpty();
}

/******************************************************************************
 GetFontID (virtual protected)

 ******************************************************************************/

JFontID
JXFontManager::GetFontID
	(
	const JCharacter*	name,
	const JSize			size,
	const JFontStyle&	style
	)
	const
{
	const JSize count = itsFontList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const FontInfo info = itsFontList->GetElement(i);
		if (*(info.name) == name && info.size == size &&
			info.style.bold == style.bold &&
			info.style.italic == style.italic)
			{
			return i;
			}
		}

	// falling through means we need to create a new entry

	const JString xFontName = ConvertToXFontName(name);

	FontInfo info;
	if (GetNewFont(xFontName, size, style, &(info.xfont)))
		{
		info.exact = kJTrue;
		}
	else
		{
		info.exact = kJFalse;
		ApproximateFont(xFontName, size, style, &(info.xfont));
		}

	info.name = new JString(name);
	assert( info.name != NULL );

	info.size      = size;
	info.style     = style;
	info.monoWidth = IsMonospace(info.xfont);

	itsFontList->AppendElement(info);
	return itsFontList->GetElementCount();
}

/******************************************************************************
 GetFontID

	For X Windows only:  pass in complete string from XListFonts().

 ******************************************************************************/

JBoolean
JXFontManager::GetFontID
	(
	const JCharacter*	xFontStr,
	JFontID*			fontID
	)
	const
{
	const JSize count = itsFontList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const FontInfo info = itsFontList->GetElement(i);
		if (*(info.name) == xFontStr && info.size == 0)
			{
			*fontID = i;
			return kJTrue;
			}
		}

	// falling through means we need to create a new entry

	FontInfo info;

#if ENABLE_TRUE_TYPE

	XftFont* xft = XftFontOpenXlfd(*itsDisplay, itsDisplay->GetScreen(), xFontStr);
	if (xft != NULL)
		{
		info.xfont.type   = kTrueType;
		info.xfont.xftrue = xft;
		}
	else

#endif

		{
		XFontStruct* xfs = XLoadQueryFont(*itsDisplay, xFontStr);
		if (xfs != NULL)
			{
			info.xfont.type  = kStdType;
			info.xfont.xfstd = xfs;
			}
		else
			{
			*fontID = 0;
			return kJFalse;
			}
		}

	info.name = new JString(xFontStr);
	assert( info.name != NULL );

	info.size      = 0;
	info.exact     = kJTrue;
	info.monoWidth = IsMonospace(info.xfont);

	itsFontList->AppendElement(info);

	*fontID = itsFontList->GetElementCount();
	return kJTrue;
}

/******************************************************************************
 GetNewFont (private)

 ******************************************************************************/

JBoolean
JXFontManager::GetNewFont
	(
	const JCharacter*	name,
	const JSize			size,
	const JFontStyle&	style,
	XFont*				xfont
	)
	const
{
	JString xFontStr;

#if ENABLE_TRUE_TYPE

	if (BuildTrueTypeFontName(name, size, style, &xFontStr))
		{
		XftFont* xft = XftFontOpenName(*itsDisplay, itsDisplay->GetScreen(), xFontStr);
		if (xft != NULL)
			{
			xfont->type   = kTrueType;
			xfont->xftrue = xft;
			return kJTrue;
			}
		}

#endif

	const JCharacter* italicStr = kObliqueStr;	// try oblique before italic
	const JCharacter* iso       = "iso8859-1";

	XFontStruct* xfs = NULL;
	while (1)
		{
		xFontStr = BuildStdFontName(name, size, style, italicStr, iso);
		xfs      = XLoadQueryFont(*itsDisplay, xFontStr);
		if (xfs != NULL)
			{
			xfont->type  = kStdType;
			xfont->xfstd = xfs;
			return kJTrue;
			}

		if (strcmp(italicStr, kObliqueStr) == 0 && style.italic)
			{
			italicStr = kItalicStr;
			}
		else if (iso[1] != '\0')
			{
			iso = "*";
			}
		else
			{
			return kJFalse;	// give up
			}
		}
}

/******************************************************************************
 BuildStdFontName (private)

 ******************************************************************************/

JString
JXFontManager::BuildStdFontName
	(
	const JCharacter*	xName,
	const JSize			size,
	const JFontStyle&	style,
	const JCharacter*	italicStr,
	const JCharacter*	iso
	)
	const
{
	// handle NxM separately

	if (nxmRegex.Match(xName))
		{
		JString xFontStr = xName;
		if (style.bold)
			{
			xFontStr += "bold";
			}
		return xFontStr;
		}

	// any foundry

	JString xFontStr = "-*-";

	// given name

	xFontStr.Append(xName);

	// regular or boldface

	if (style.bold)
		{
		xFontStr.Append("-bold");
		}
	else
		{
		xFontStr.Append("-medium");
		}

	// regular or italic

	if (style.italic)
		{
		xFontStr.Append(italicStr);
		}
	else
		{
		xFontStr.Append("-r");
		}

	// normal character spacing, any pixel size

	xFontStr.Append("-normal-*-*-");

	// font size

	xFontStr.Append( JString(10*(size+2), JString::kBase10) );

	// screen resolution (apparently, we should always just use 75 dpi fonts),
	// any spacing, any avg width, charset to match unicode

	xFontStr.Append("-75-75-*-*-");
	xFontStr.Append(iso);

	// return the result

	return xFontStr;
}

/******************************************************************************
 BuildTrueTypeFontName (private)

 ******************************************************************************/

JBoolean
JXFontManager::BuildTrueTypeFontName
	(
	const JCharacter*	xName,
	const JSize			size,
	const JFontStyle&	style,
	JString*			xFontStr
	)
	const
{
	xFontStr->Clear();

	// NxM is not TrueType.  Symbol is strange.

	if (nxmRegex.Match(xName) ||
		JStringCompare(xName, "Symbol", kJFalse) == 0)
		{
		return kJFalse;
		}

	// name

	xFontStr->Append(xName);

	// size

	xFontStr->Append("-");
	xFontStr->Append( JString(size, JString::kBase10) );

	// regular or boldface

	if (style.bold)
		{
		xFontStr->Append(":weight=bold");
		}
	else
		{
		xFontStr->Append(":weight=light");
		}

	// regular or italic

	if (style.italic)
		{
		xFontStr->Append(":slant=oblique,italic");
		}
	else
		{
		xFontStr->Append(":slant=roman");
		}

	// success

	return kJTrue;
}

/******************************************************************************
 ApproximateFont (private)

	This is called when a requested font is not available.  We are required
	to find an existing font that is reasonably close to the requested one.

	The result of this function is guaranteed to be a valid font.

 ******************************************************************************/

void
JXFontManager::ApproximateFont
	(
	const JCharacter*	origName,
	const JSize			origSize,
	const JFontStyle&	origStyle,
	XFont*				xfont
	)
	const
{
	JString name     = origName;
	JSize size       = origSize;
	JFontStyle style = origStyle;

	while (1)
		{
		if (size % 2 == 1)
			{
			size--;		// even sizes are more common
			}
		else if (size < 12)
			{
			size += 2;
			}
		else if (size > 12)
			{
			size -= 2;
			}
		else if (style.bold)
			{
			style.bold = kJFalse;
			size       = origSize;
			}
		else if (style.italic)
			{
			style.italic = kJFalse;
			size         = origSize;
			}
		else if (name != JGetDefaultFontName())
			{
			name  = JGetDefaultFontName();
			size  = origSize;
			style = origStyle;
			}
		else
			{
			// this should never happen, but it does with some Win95 X servers

			cerr << "The X server does not have any 75 dpi PostScript fonts, not even ";
			cerr << JGetDefaultFontName() << endl;
			cerr << "Please install the xorg-x11-fonts-75dpi package" << endl;
			JXApplication::Abort(JXDocumentManager::kServerDead, kJFalse);
			}

		if (GetNewFont(name, size, style, xfont))
			{
			break;
			}
		}
}

/******************************************************************************
 GetFontName (virtual protected)

 ******************************************************************************/

const JCharacter*
JXFontManager::GetFontName
	(
	const JFontID id
	)
	const
{
	const FontInfo info = itsFontList->GetElement(id);
	return *(info.name);
}

/******************************************************************************
 IsExact (virtual protected)

 ******************************************************************************/

JBoolean
JXFontManager::IsExact
	(
	const JFontID id
	)
	const
{
	const FontInfo info = itsFontList->GetElement(id);
	return info.exact;
}

/******************************************************************************
 GetLineHeight (virtual protected)

 ******************************************************************************/

JSize
JXFontManager::GetLineHeight
	(
	const JFontID		fontID,
	const JSize			size,
	const JFontStyle&	style,

	JCoordinate*		ascent,
	JCoordinate*		descent
	)
	const
{
	FontInfo info = itsFontList->GetElement(fontID);
	if (info.xfont.type == kStdType)
		{
		*ascent  = info.xfont.xfstd->ascent;
		*descent = info.xfont.xfstd->descent;
		}
	else
		{
		assert( info.xfont.type == kTrueType );

		if (info.ascent == 0)
			{
			XGlyphInfo g;
			XftTextExtents8(*itsDisplay, info.xfont.xftrue, (FcChar8*) "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789|_", 64, &g);
			info.ascent  = g.y + 1 + size/10;
			info.descent = g.height - g.y;
			itsFontList->SetElement(fontID, info);
			}

		*ascent  = info.ascent;
		*descent = info.descent;
		}

	const JCoordinate underlineHeight = 2 * GetUnderlineThickness(size) * style.underlineCount;
	if (*descent < underlineHeight)
		{
		*descent = underlineHeight;
		}

	return (*ascent + *descent);
}

/******************************************************************************
 GetCharWidth (virtual protected)

 ******************************************************************************/

JSize
JXFontManager::GetCharWidth
	(
	const JFontID		fontID,
	const JCharacter	c
	)
	const
{
	const FontInfo info = itsFontList->GetElement(fontID);
	if (info.monoWidth > 0)
		{
		return info.monoWidth;
		}
	else if (info.xfont.type == kStdType)
		{
		return XTextWidth(info.xfont.xfstd, &c, 1);
		}
	else
		{
		assert( info.xfont.type == kTrueType );

		XGlyphInfo g;
		XftTextExtents8(*itsDisplay, info.xfont.xftrue, (FcChar8*) &c, 1, &g);
		return g.xOff;
		}
}

/******************************************************************************
 GetStringWidth (virtual protected)

 ******************************************************************************/

JSize
JXFontManager::GetStringWidth
	(
	const JFontID		fontID,
	const JCharacter*	str,
	const JSize			charCount
	)
	const
{
	const FontInfo info = itsFontList->GetElement(fontID);
	if (info.monoWidth > 0)
		{
		return charCount * info.monoWidth;
		}
	else
		{
		const JSize maxStringLength = itsDisplay->GetMaxStringLength();

		JSize width  = 0;
		JSize offset = 0;
		XGlyphInfo g;
		while (offset < charCount)
			{
			const JSize count = JMin(charCount - offset, maxStringLength);

			if (info.xfont.type == kStdType)
				{
				width += XTextWidth(info.xfont.xfstd, str + offset, count);
				}
			else
				{
				assert( info.xfont.type == kTrueType );
				XftTextExtents8(*itsDisplay, info.xfont.xftrue, (FcChar8*) (str + offset), count, &g);
				width += g.xOff;
				}

			offset += count;
			}

		return width;
		}
}

/******************************************************************************
 GetXFontInfo

 ******************************************************************************/

JXFontManager::XFont
JXFontManager::GetXFontInfo
	(
	const JFontID id
	)
	const
{
	const FontInfo info = itsFontList->GetElement(id);
	return info.xfont;
}

/******************************************************************************
 ConvertToXFontName (private)

	Fold name to lower case.

 ******************************************************************************/

JString
JXFontManager::ConvertToXFontName
	(
	const JCharacter* name
	)
	const
{
	JString fontName = name;
	fontName.ToLower();
	return fontName;
}

/******************************************************************************
 ConvertToPSFontName (private)

	Capitalize the first character of every word.

 ******************************************************************************/

void
JXFontManager::ConvertToPSFontName
	(
	JString* name
	)
	const
{
	const JSize length = name->GetLength();
	for (JIndex i=1; i<=length; i++)
		{
		if (i == 1 || name->GetCharacter(i-1) == ' ')
			{
			name->SetCharacter(i, JToUpper(name->GetCharacter(i)));
			}
		}
}

/******************************************************************************
 XFont::Free

 ******************************************************************************/

void
JXFontManager::XFont::Free
	(
	JXDisplay* display
	)
{
	if (type == kStdType)
	{
		XFreeFont(*display, xfstd);
	}
	else
	{
		assert( type == kTrueType );
		XftFontClose(*display, xftrue);
	}
}
