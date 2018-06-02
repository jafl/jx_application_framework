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

	Copyright (C) 1996-2018 by John Lindal.

 ******************************************************************************/

#include <JXFontManager.h>
#include <JXDisplay.h>
#include <jXGlobals.h>
#include <JPtrArray-JString.h>
#include <JListUtil.h>
#include <JStringIterator.h>
#include <JRegex.h>
#include <JMinMax.h>
#include <jStreamUtil.h>
#include <jMath.h>
#include <sstream>
#include <string.h>
#include <jAssert.h>

#define ONLY_STD_PS_FONTS	1
#define ONLY_STD_MONOSPACE	1

static const JUtf8Byte* kItalicStr  = "-i";
static const JUtf8Byte* kObliqueStr = "-o";

static const JRegex nxmRegex = "^([0-9]+)x([0-9]+)$";

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

	itsFontList = jnew JArray<FontInfo>;
	assert( itsFontList != NULL );

	itsAllFontNames  = NULL;
	itsMonoFontNames = NULL;
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
		info.xfont.Free(itsDisplay);
		}
	jdelete itsFontList;

	jdelete itsAllFontNames;
	jdelete itsMonoFontNames;
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
{
	if (itsAllFontNames != NULL)
		{
		fontNames->CopyObjects(*itsAllFontNames, fontNames->GetCleanUpAction(), kJFalse);
		}
	else
		{
		fontNames->CleanOut();
		fontNames->SetCompareFunction(JCompareStringsCaseInsensitive);
		fontNames->SetSortOrder(JListT::kSortAscending);

		JString name;

		FcFontSet* set = XftListFonts(*itsDisplay, itsDisplay->GetScreen(), NULL, FC_FAMILY, NULL);
		for (int i=0; i < set->nfont; i++)
			{
			FcChar8* s = FcNameUnparse(set->fonts[i]);
			name.Set((JUtf8Byte*) s);
//			std::cout << "tt  font: " << name << std::endl;

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

		int nameCount;
		char** nameList = XListFonts(*itsDisplay, "-*-*-medium-r-normal-*-*-*-75-75-*-*-iso10646-1",
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
//			std::cout << "std font: " << name << std::endl;

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

		itsAllFontNames = jnew JDCCPtrArray<JString>(*fontNames, JPtrArrayT::kDeleteAll);
		assert( itsAllFontNames != NULL );
		}
}

/******************************************************************************
 GetMonospaceFontNames (virtual)

 ******************************************************************************/

void
JXFontManager::GetMonospaceFontNames
	(
	JPtrArray<JString>* fontNames
	)
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
		fontNames->SetSortOrder(JListT::kSortAscending);

		JPtrArray<JString> allFontNames(JPtrArrayT::kDeleteAll);
		allFontNames.SetCompareFunction(JCompareStringsCaseInsensitive);
		allFontNames.SetSortOrder(JListT::kSortAscending);

		JString name;

		FcFontSet* set =
			XftListFonts(*itsDisplay, itsDisplay->GetScreen(),
						 FC_SPACING, FcTypeInteger, FC_MONO, NULL,
						 FC_FAMILY, NULL);
		for (int i=0; i < set->nfont; i++)
			{
			FcChar8* s = FcNameUnparse(set->fonts[i]);
			name.Set((JUtf8Byte*) s);
//			std::cout << "tt  mono: " << name << std::endl;

#if ONLY_STD_MONOSPACE

			if (!name.BeginsWith("Courier") &&
				!name.BeginsWith("Consolas") &&
				!name.Contains(" Mono")     &&
				name != "LucidaTypewriter")
				{
				FcStrFree(s);
				continue;
				}
#else
			if (IsUseless(name))
				{
				FcStrFree(s);
				continue;
				}
#endif
			JBoolean isDuplicate;
			const JIndex index =
				allFontNames.GetInsertionSortIndex(&name, &isDuplicate);
			if (!isDuplicate)
				{
				allFontNames.InsertAtIndex(index, name);

				JString* n = jnew JString(name);
				assert( n != NULL );
				const JBoolean ok = fontNames->InsertSorted(n, kJFalse);
				assert( ok );
				}

			FcStrFree(s);
			}
		FcFontSetDestroy(set);

		// save names for next time

		itsMonoFontNames = jnew JDCCPtrArray<JString>(*fontNames, JPtrArrayT::kDeleteAll);
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
{
	fontNames->CleanOut();
	fontNames->SetCompareFunction(
					compare != NULL ? compare : JCompareStringsCaseInsensitive);
	fontNames->SetSortOrder(JListT::kSortAscending);

	int nameCount;
	char** nameList = XListFonts(*itsDisplay, "*", INT_MAX, &nameCount);
	if (nameList == NULL)
		{
		return;
		}

	for (int i=0; i<nameCount; i++)
		{
		if (strcmp(nameList[i], "nil") != 0 &&
			regex.Match(JString(nameList[i], 0, kJFalse)))
			{
			JString name(nameList[i], 0, kJFalse);
			JBoolean isDuplicate;
			const JIndex index = fontNames->GetInsertionSortIndex(&name, &isDuplicate);
			if (!isDuplicate)
				{
				JString* n = jnew JString(name);
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
	const JString&	name,
	JSize*			minSize,
	JSize*			maxSize,
	JArray<JSize>*	sizeList
	)
{
	sizeList->RemoveAll();

	*minSize = 8;
	*maxSize = 24;
	return kJTrue;
}

/******************************************************************************
 GetXFont

	For X Windows only:  pass in complete string from XListFonts().

	*** Caller must jdelete the returned object.

 ******************************************************************************/

JBoolean
JXFontManager::GetXFont
	(
	const JString&	xFontStr,
	JFont**			font
	)
{
	*font = NULL;

	const JFontID id = GetFontID(xFontStr, 0, JFontStyle());
	while (!itsFontList->IndexValid(id))
		{
		itsFontList->AppendElement(FontInfo());
		}

	FontInfo info = itsFontList->GetElement(id);
	if (info.filled)
		{
		*font = jnew JFont(GetFont(id));
		assert( font != NULL );
		return kJTrue;
		}

	// falling through means we need to create a new entry

	XftFont* xft = XftFontOpenXlfd(*itsDisplay, itsDisplay->GetScreen(), xFontStr.GetBytes());
	if (xft != NULL)
		{
		info.xfont.type = kTrueType;
		info.xfont.xftt = xft;
		}
	else
		{
		char** missingCharsetList;
		int missingCharsetCount;
		char* defString;
		XFontSet set = XCreateFontSet(*itsDisplay, xFontStr.GetBytes(), &missingCharsetList, &missingCharsetCount, &defString);
		if (set != NULL)
			{
			XFreeStringList(missingCharsetList);
			info.xfont.type  = kStdType;
			info.xfont.xfset = set;
			}
		else
			{
			return kJFalse;
			}
		}

	info.filled    = kJTrue;
	info.exact     = kJTrue;
	info.monoWidth = IsMonospace(info.xfont);

	itsFontList->SetElement(id, info);

	*font = jnew JFont(GetFont(id));
	assert( font != NULL );
	return kJTrue;
}

/******************************************************************************
 IsExact (virtual)

 ******************************************************************************/

JBoolean
JXFontManager::IsExact
	(
	const JFontID id
	)
{
	return ResolveFontID(id).exact;
}

/******************************************************************************
 GetLineHeight (virtual protected)

 ******************************************************************************/

JSize
JXFontManager::GetLineHeight
	(
	const JFontID		id,
	const JSize			size,
	const JFontStyle&	style,

	JCoordinate*		ascent,
	JCoordinate*		descent
	)
{
	FontInfo info = ResolveFontID(id);
	if (info.xfont.type == kStdType && info.ascent == 0)
		{
		XFontStruct** fsList;
		char** nameList;
		const int count = XFontsOfFontSet(info.xfont.xfset, &fsList, &nameList);
		for (int i=0; i<count; i++)
			{
			info.ascent  = JMax(info.ascent,  (JCoordinate) fsList[i]->ascent);
			info.descent = JMax(info.descent, (JCoordinate) fsList[i]->descent);
			}
		itsFontList->SetElement(id, info);
		}
	else if (info.ascent == 0)
		{
		assert( info.xfont.type == kTrueType );

		XGlyphInfo g;
		XftTextExtentsUtf8(*itsDisplay, info.xfont.xftt, (FcChar8*) "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789|_", 64, &g);
		info.ascent  = g.y + 1 + size/10;
		info.descent = g.height - g.y;
		itsFontList->SetElement(id, info);
		}

	*ascent  = info.ascent;
	*descent = info.descent;

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
	const JFontID			id,
	const JUtf8Character&	c
	)
{
	const FontInfo info = ResolveFontID(id);
	if (info.monoWidth > 0)
		{
		return info.monoWidth;
		}
	else if (info.xfont.type == kStdType)
		{
		// Xutf8TextEscapement() would be more accurate, but it's not standard.
		return XmbTextEscapement(info.xfont.xfset, c.GetBytes(), c.GetByteCount());
		}
	else
		{
		assert( info.xfont.type == kTrueType );

		XGlyphInfo g;
		XftTextExtentsUtf8(*itsDisplay, info.xfont.xftt, (FcChar8*) c.GetBytes(), c.GetByteCount(), &g);
		return g.xOff;
		}
}

/******************************************************************************
 GetStringWidth (virtual protected)

 ******************************************************************************/

JSize
JXFontManager::GetStringWidth
	(
	const JFontID	id,
	const JString&	str
	)
{
	const FontInfo info = ResolveFontID(id);
	if (info.monoWidth > 0)
		{
		return str.GetCharacterCount() * info.monoWidth;
		}
	else
		{
		const JSize byteCount      = str.GetByteCount();
		const JSize chunkByteCount = itsDisplay->GetMaxStringByteCount();

		JSize width  = 0;
		JSize offset = 0;
		XGlyphInfo g;
		while (offset < byteCount)
			{
			const JUtf8Byte* s = str.GetRawBytes() + offset;	// GetRawBytes() because str may be a shadow

			JSize count = byteCount - offset;
			if (count > chunkByteCount)
				{
				count = chunkByteCount;
				JSize byteCount;
				while (!JUtf8Character::GetCharacterByteCount(s + count, &byteCount))
					{
					count--;
					}
				}

			if (info.xfont.type == kStdType)
				{
				// Xutf8TextEscapement() would be more accurate, but it's not standard.
				width += XmbTextEscapement(info.xfont.xfset, s, count);
				}
			else
				{
				assert( info.xfont.type == kTrueType );
				XftTextExtentsUtf8(*itsDisplay, info.xfont.xftt, (FcChar8*) s, count, &g);
				width += g.xOff;
				}

			offset += count;
			}

		return width;
		}
}

/******************************************************************************
 ResolveFontID (private)

 ******************************************************************************/

JXFontManager::FontInfo
JXFontManager::ResolveFontID
	(
	const JFontID id
	)
{
	while (!itsFontList->IndexValid(id))
		{
		itsFontList->AppendElement(FontInfo());
		}

	FontInfo info = itsFontList->GetElement(id);
	if (info.filled)
		{
		return info;
		}

	// falling through means we need to create a new entry

	const JFont f = GetFont(id);

	const JString xFontName = ConvertToXFontName(f.GetName());
	if (GetNewFont(xFontName, f.GetSize(), f.GetStyle(), &(info.xfont)))
		{
		info.exact = kJTrue;
		}
	else
		{
		info.exact = kJFalse;
		ApproximateFont(xFontName, f.GetSize(), f.GetStyle(), &(info.xfont));
		}

	info.filled    = kJTrue;
	info.monoWidth = IsMonospace(info.xfont);

	itsFontList->SetElement(id, info);
	return info;
}

/******************************************************************************
 GetNewFont (private)

 ******************************************************************************/

JBoolean
JXFontManager::GetNewFont
	(
	const JString&		origName,
	const JSize			origSize,
	const JFontStyle&	style,
	XFont*				xfont
	)
{
	JString xFontStr;

	if (BuildTrueTypeFontName(origName, origSize, style, &xFontStr))
		{
		XftFont* xft = XftFontOpenName(*itsDisplay, itsDisplay->GetScreen(), xFontStr.GetBytes());
		if (xft != NULL)
			{
			xfont->type = kTrueType;
			xfont->xftt = xft;
			return kJTrue;
			}
		}

	JString name     = origName;
	JSize pointSize  = origSize;
	JSize pixelSize  = 0;
	JSize pixelWidth = 0;

	const JUtf8Byte* spacing = "normal";

	const JStringMatch m = nxmRegex.Match(name, kJTrue);
	if (!m.IsEmpty())
		{
		JBoolean ok = m.GetSubstring(1).ConvertToUInt(&pixelWidth);
		assert(ok);

		ok = m.GetSubstring(2).ConvertToUInt(&pixelSize);
		assert(ok);

		name.Set("fixed");
		pointSize = 0;

		if (pixelWidth == 6 && (pixelSize == 12 || pixelSize == 13))
			{
			spacing = "semicondensed";
			}
		}

	const JUtf8Byte* italicStr = kObliqueStr;	// try oblique before italic

	XFontSet set;
	char** missingCharsetList;
	int missingCharsetCount;
	char* defString;
	while (1)
		{
		xFontStr = BuildStdFontName(name, pointSize, pixelSize, pixelWidth, spacing, style, italicStr);
		set      = XCreateFontSet(*itsDisplay, xFontStr.GetBytes(), &missingCharsetList, &missingCharsetCount, &defString);
		if (set != NULL)
			{
			XFreeStringList(missingCharsetList);
			xfont->type  = kStdType;
			xfont->xfset = set;
			return kJTrue;
			}

		if (strcmp(italicStr, kObliqueStr) == 0 && style.italic)
			{
			italicStr = kItalicStr;
			}
		else
			{
			return kJFalse;	// give up
			}
		}
}

/******************************************************************************
 BuildStdFontName (static private)

 ******************************************************************************/

JString
JXFontManager::BuildStdFontName
	(
	const JString&		xName,
	const JSize			pointSize,
	const JSize			pixelSize,
	const JSize			pixelWidth,
	const JUtf8Byte*	spacing,
	const JFontStyle&	style,
	const JUtf8Byte*	italicStr
	)
{
	// any foundry

	JString xFontStr("-*-", 0, kJFalse);

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

	// character spacing

	xFontStr.Append("-");
	xFontStr.Append(spacing);

	// extra foundry info

	xFontStr.Append("-*");

	// pixel size

	if (pixelSize > 0)
		{
		xFontStr.Append("-");
		xFontStr.Append( JString(pixelSize, JString::kBase10) );
		}
	else
		{
		xFontStr.Append("-*");
		}

	// font size

	if (pointSize > 0)
		{
		xFontStr.Append("-");
		xFontStr.Append( JString(10*(pointSize+2), JString::kBase10) );
		}
	else
		{
		xFontStr.Append("-*");
		}

	// screen resolution (apparently, we should always just use 75 dpi fonts),
	// any spacing

	xFontStr.Append("-75-75-*");

	// avg width

	if (pixelWidth > 0)
		{
		xFontStr.Append("-");
		xFontStr.Append( JString(10*pixelWidth, JString::kBase10) );
		}
	else
		{
		xFontStr.Append("-*");
		}

	// unicode charset

	xFontStr.Append("-iso10646-1");

	// return the result

	return xFontStr;
}

/******************************************************************************
 BuildTrueTypeFontName (static private)

 ******************************************************************************/

JBoolean
JXFontManager::BuildTrueTypeFontName
	(
	const JString&		xName,
	const JSize			size,
	const JFontStyle&	style,
	JString*			xFontStr
	)
{
	xFontStr->Clear();

	// NxM is not TrueType.

	if (nxmRegex.Match(xName))
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
	const JString&		origName,
	const JSize			origSize,
	const JFontStyle&	origStyle,
	XFont*				xfont
	)
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

			std::cerr << "The X server does not have any 75 dpi PostScript fonts, not even ";
			std::cerr << JGetDefaultFontName() << std::endl;
			std::cerr << "Please install the xorg-x11-fonts-75dpi package" << std::endl;
			JXApplication::Abort(JXDocumentManager::kServerDead, kJFalse);
			}

		if (GetNewFont(name, size, style, xfont))
			{
			break;
			}
		}
}

/******************************************************************************
 ConvertToXFontName (static private)

	Fold name to lower case.

 ******************************************************************************/

JString
JXFontManager::ConvertToXFontName
	(
	const JString& name
	)
{
	JString fontName = name;
	fontName.ToLower();
	return fontName;
}

/******************************************************************************
 ConvertToPSFontName (static private)

	Capitalize the first character of every word.

 ******************************************************************************/

void
JXFontManager::ConvertToPSFontName
	(
	JString* name
	)
{
	JStringIterator iter(name);
	JUtf8Character c, prev;
	while (iter.Next(&c))
		{
		if (prev.GetByteCount() == 0 || prev.IsSpace())
			{
			iter.SetPrev(c.ToUpper(), kJFalse);
			}
		prev = c;
		}
}

/******************************************************************************
 IsMonospace (private)

	Returns monospace width or zero.

 ******************************************************************************/

JCoordinate
JXFontManager::IsMonospace
	(
	const XFont& xfont
	)
{
	if (xfont.type == kStdType)
	{
		XFontStruct** fsList;
		char** nameList;
		const int count = XFontsOfFontSet(xfont.xfset, &fsList, &nameList);
		JCoordinate w   = fsList[0]->min_bounds.width;
		for (int i=0; i<count; i++)
			{
			if (fsList[i]->min_bounds.width != w ||
				fsList[i]->max_bounds.width != w)
				{
				return 0;
				}
			}

		return w;
	}
	else
	{
		assert( xfont.type == kTrueType );

		XGlyphInfo g1, g2;
		XftTextExtentsUtf8(*itsDisplay, xfont.xftt, (FcChar8*) "M", 1, &g1);
		XftTextExtentsUtf8(*itsDisplay, xfont.xftt, (FcChar8*) "|", 1, &g2);
		return (g1.xOff == g2.xOff ? g1.xOff : 0);
	}
}

/******************************************************************************
 IsPostscript (private)

	Returns kJTrue if the font is available in Postscript.

	Until JPSPrinter can embed fonts in a Postscript file, we are limited
	to only the standard Postscript fonts.

 ******************************************************************************/

JBoolean
JXFontManager::IsPostscript
	(
	const JString& name
	)
{
#if ONLY_STD_PS_FONTS

	return JI2B(name == "Arial"                     ||	// Helvetica sucks on OS X
				name.BeginsWith("Courier")          ||
				name.BeginsWith("Helvetica")        ||
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

JBoolean
JXFontManager::IsUseless
	(
	const JString& name
	)
{
	return JI2B(name.GetFirstCharacter() == '.'   ||
				name.Contains("Dingbats")         ||
				name == "Symbol"                  ||
				name.Contains("Standard Symbols") ||
				name.Contains("Cursor")           ||
				name.EndsWith(" Ti"));
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
	if (xfset != NULL)
	{
		assert( type == kStdType );
		XFreeFontSet(*display, xfset);
	}

	if (xftt != NULL)
	{
		assert( type == kTrueType );
		XftFontClose(*display, xftt);
	}
}
