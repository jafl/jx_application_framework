/******************************************************************************
 JXFontManager.cpp

	BASE CLASS = JFontManager

	Copyright (C) 1996-2018 by John Lindal.

 ******************************************************************************/

#include "jx-af/jx/JXFontManager.h"
#include "jx-af/jx/JXDisplay.h"
#include "jx-af/jx/jXGlobals.h"
#include <jx-af/jcore/JPtrArray-JString.h>
#include <jx-af/jcore/JListUtil.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/JMinMax.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jMath.h>
#include <sstream>
#include <string.h>
#include <jx-af/jcore/jAssert.h>

#define INCLUDE_X11_FONTS 0

static const JUtf8Byte* kItalicStr  = "-i";
static const JUtf8Byte* kObliqueStr = "-o";

static const JRegex nxmRegex = "^([0-9]+)x([0-9]+)$";

#if FC_MAJOR < 2 || (FC_MAJOR == 2 && FC_MINOR < 3)
	#define FcStrFree free
#endif

static const JUtf8Byte* kDefaultFontName =
	#ifdef _J_OSX
	"Arial";		// Latin, Greek, some Cyrillic, some Arabic
	#else
	"FreeSans";		// Arabic, Armenian, Cyrillic, Greek, Hebrew, Latin
	#endif

static const JUtf8Byte* kMonospaceFontName =
	#ifdef _J_OSX
	"Menlo";
	#else
	"DejaVu Sans Mono";
	#endif

static const JUtf8Byte* kFallbackFontNames[] =
{
#ifdef _J_OSX
	"Mshtakan",						// Armenian
	"Arial Unicode MS",				// Arabic, Armenian, CJK, Cyrillic, Greek, Hebrew
#else
	"Source Han Sans CN Normal",	// CJK - Fedora
	"Noto Sans CJK SC",				// CJK - Ubuntu
#endif
	kDefaultFontName
};

/******************************************************************************
 Init (static)

 ******************************************************************************/

void
JXFontManager::Init()
{
	JFontManager::Init(kDefaultFontName, kMonospaceFontName);
}

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
	assert( itsFontList != nullptr );

	itsAllFontNames  = nullptr;
	itsMonoFontNames = nullptr;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXFontManager::~JXFontManager()
{
	for (auto info : *itsFontList)
	{
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

inline void
jInsertFontName
	(
	JPtrArray<JString>*	fontNames,
	JString*			name
	)
{
	bool isDuplicate;
	const JIndex index = fontNames->GetInsertionSortIndex(name, &isDuplicate);
	if (!isDuplicate)
	{
		fontNames->InsertAtIndex(index, *name);
	}
}

void
JXFontManager::GetFontNames
	(
	JPtrArray<JString>* fontNames
	)
{
	if (itsAllFontNames != nullptr)
	{
		fontNames->CopyObjects(*itsAllFontNames, fontNames->GetCleanUpAction(), false);
		return;
	}

	fontNames->CleanOut();
	fontNames->SetCompareFunction(JCompareStringsCaseInsensitive);
	fontNames->SetSortOrder(JListT::kSortAscending);

	JString name;

	FcFontSet* set =
		XftListFonts(*itsDisplay, itsDisplay->GetScreen(),
					 FC_LANG, FcTypeString,
						JGetStringManager()->GetBCP47Locale().GetBytes(),
					 nullptr,
					 FC_FAMILY, nullptr);

	for (int i=0; i < set->nfont; i++)
	{
		FcChar8* s = FcNameUnparse(set->fonts[i]);
		name.Set((JUtf8Byte*) s);
		FcStrFree(s);
		s = nullptr;

		TrimTrueTypeFontName(&name);
		if (ShouldIgnore(name))
		{
			continue;
		}
//		std::cout << "tt  font: " << name << std::endl;
//		FcPatternPrint(set->fonts[i]);

		jInsertFontName(fontNames, &name);
	}
	FcFontSetDestroy(set);

	for (const auto* n : kFallbackFontNames)
	{
		name.Set(n);
		jInsertFontName(fontNames, &name);
	}

#if INCLUDE_X11_FONTS

	int nameCount;
	char** nameList = XListFonts(*itsDisplay, "-*-*-medium-r-normal-*-0-0-75-75-*-*-iso10646-1",
								 INT_MAX, &nameCount);
	if (nameList == nullptr)
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

		if (ShouldIgnore(name))
		{
			continue;
		}
//		std::cout << "std font: " << name << std::endl;

		jInsertFontName(fontNames, &name);
	}
	XFreeFontNames(nameList);

	// save names for next time

	itsAllFontNames = jnew JDCCPtrArray<JString>(*fontNames, JPtrArrayT::kDeleteAll);
	assert( itsAllFontNames != nullptr );

#endif
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
	if (itsMonoFontNames != nullptr)
	{
		fontNames->CopyObjects(*itsMonoFontNames, fontNames->GetCleanUpAction(), false);
		return;
	}

	JXGetApplication()->DisplayBusyCursor();

	fontNames->CleanOut();
	fontNames->SetCompareFunction(JCompareStringsCaseInsensitive);
	fontNames->SetSortOrder(JListT::kSortAscending);

	JString name;

	FcFontSet* set =
		XftListFonts(*itsDisplay, itsDisplay->GetScreen(),
					 FC_SPACING, FcTypeInteger, FC_MONO,
					 FC_LANG, FcTypeString,
						JGetStringManager()->GetBCP47Locale().GetBytes(),
					 nullptr,
					 FC_FAMILY, nullptr);

	for (int i=0; i < set->nfont; i++)
	{
		FcChar8* s = FcNameUnparse(set->fonts[i]);
		name.Set((JUtf8Byte*) s);
		FcStrFree(s);
		s = nullptr;
//		std::cout << "tt  mono: " << name << std::endl;

		TrimTrueTypeFontName(&name);
		if (ShouldIgnore(name))
		{
			continue;
		}

		jInsertFontName(fontNames, &name);
	}
	FcFontSetDestroy(set);

	// save names for next time

	itsMonoFontNames = jnew JDCCPtrArray<JString>(*fontNames, JPtrArrayT::kDeleteAll);
	assert( itsMonoFontNames != nullptr );
}

/******************************************************************************
 GetXFontNames

	This provides raw output from XListFonts().  For example, ^[0-9]x[0-9]+$
	would yield a list of clean, monospace fonts.

	If compare is nullptr, we use JCompareStringsCaseInsensitive().

 ******************************************************************************/

void
JXFontManager::GetXFontNames
	(
	const JRegex&		regex,
	JPtrArray<JString>*	fontNames,

	const std::function<JListT::CompareResult(JString * const &, JString * const &)>* compare
	)
{
	fontNames->CleanOut();
	fontNames->SetCompareFunction(
					compare != nullptr ? *compare : JCompareStringsCaseInsensitive);
	fontNames->SetSortOrder(JListT::kSortAscending);

	int nameCount;
	char** nameList = XListFonts(*itsDisplay, "*", INT_MAX, &nameCount);
	if (nameList == nullptr)
	{
		return;
	}

	for (int i=0; i<nameCount; i++)
	{
		if (strcmp(nameList[i], "nil") != 0 &&
			regex.Match(JString(nameList[i], JString::kNoCopy)))
		{
			JString name(nameList[i], JString::kNoCopy);
			bool isDuplicate;
			const JIndex index = fontNames->GetInsertionSortIndex(&name, &isDuplicate);
			if (!isDuplicate)
			{
				auto* n = jnew JString(name);
				assert( n != nullptr );
				fontNames->InsertAtIndex(index, n);
			}
		}
	}

	fontNames->ClearCompareFunction();
	XFreeFontNames(nameList);
}

/******************************************************************************
 GetFontSizes (virtual)

	If all font sizes are supported (e.g. TrueType), returns reasonable
	min and max, and empty sizeList.

	Returns false if there is no font with the specified name.

 ******************************************************************************/

bool
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
	return true;
}

/******************************************************************************
 GetXFont

	For X Windows only:  pass in complete string from XListFonts().

	*** Caller must jdelete the returned object.

 ******************************************************************************/

bool
JXFontManager::GetXFont
	(
	const JString&	xFontStr,
	JFont**			font
	)
{
	*font = nullptr;

	const JFontID id = GetFontID(xFontStr, 0, JFontStyle());
	while (!itsFontList->IndexValid(id))
	{
		itsFontList->AppendElement(FontInfo());
	}

	FontInfo info = itsFontList->GetElement(id);
	if (info.filled)
	{
		*font = jnew JFont(GetFont(id));
		assert( font != nullptr );
		return true;
	}

	// falling through means we need to create a new entry

	XftFont* xft = XftFontOpenXlfd(*itsDisplay, itsDisplay->GetScreen(), xFontStr.GetBytes());
	if (xft != nullptr)
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
		if (set != nullptr)
		{
			XFreeStringList(missingCharsetList);
			info.xfont.type  = kStdType;
			info.xfont.xfset = set;
		}
		else
		{
			return false;
		}
	}

	info.filled    = true;
	info.exact     = true;
	info.monoWidth = IsMonospace(info.xfont);

	itsFontList->SetElement(id, info);

	*font = jnew JFont(GetFont(id));
	assert( font != nullptr );
	return true;
}

/******************************************************************************
 IsExact (virtual protected)

 ******************************************************************************/

bool
JXFontManager::IsExact
	(
	const JFontID id
	)
{
	return ResolveFontID(id).exact;
}

/******************************************************************************
 HasGlyphForCharacter (virtual protected)

 ******************************************************************************/

bool
JXFontManager::HasGlyphForCharacter
	(
	const JFontID			id,
	const JUtf8Character&	c
	)
{
	XFont info = GetXFontInfo(id);
	if (info.type == kTrueType)
	{
		return FcCharSetHasChar(info.xftt->charset, c.GetUtf32());
	}
	else
	{
		return true;	// no way to tell?
	}
}

/******************************************************************************
 GetSubstituteFontName (virtual protected)

 ******************************************************************************/

bool
JXFontManager::GetSubstituteFontName
	(
	const JFont&			f,
	const JUtf8Character&	c,
	JString*				name
	)
{
	JFont f1 = f;
	for (const auto* n : kFallbackFontNames)
	{
		f1.SetName(JString(n, JString::kNoCopy));
		if (HasGlyphForCharacter(f1.GetID(), c))
		{
			name->Set(n);
			return true;
		}
	}

	return false;
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
	assert( id > 0 );

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

	if (GetNewFont(f.GetName(), f.GetSize(), f.GetStyle(), &(info.xfont)))
	{
		info.exact = true;
	}
	else
	{
		info.exact = false;
		ApproximateFont(f.GetName(), f.GetSize(), f.GetStyle(), &(info.xfont));
	}

	info.filled    = true;
	info.monoWidth = IsMonospace(info.xfont);

	itsFontList->SetElement(id, info);
	return info;
}

/******************************************************************************
 GetNewFont (private)

 ******************************************************************************/

bool
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
		if (xft != nullptr)
		{
			xfont->type = kTrueType;
			xfont->xftt = xft;
			return true;
		}
	}

	JString name     = origName;
	JSize pointSize  = origSize;
	JSize pixelSize  = 0;
	JSize pixelWidth = 0;

	const JUtf8Byte* spacing = "normal";

	const JStringMatch m = nxmRegex.Match(name, JRegex::kIncludeSubmatches);
	if (!m.IsEmpty())
	{
		bool ok = m.GetSubstring(1).ConvertToUInt(&pixelWidth);
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
	while (true)
	{
		xFontStr = BuildStdFontName(name, pointSize, pixelSize, pixelWidth, spacing, style, italicStr);
		set      = XCreateFontSet(*itsDisplay, xFontStr.GetBytes(), &missingCharsetList, &missingCharsetCount, &defString);
		if (set != nullptr)
		{
			XFreeStringList(missingCharsetList);
			xfont->type  = kStdType;
			xfont->xfset = set;
			return true;
		}

		if (strcmp(italicStr, kObliqueStr) == 0 && style.italic)
		{
			italicStr = kItalicStr;
		}
		else
		{
			return false;	// give up
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

	JString xFontStr("-*-");

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
		xFontStr.Append( JString((JUInt64) pixelSize) );
	}
	else
	{
		xFontStr.Append("-*");
	}

	// font size

	if (pointSize > 0)
	{
		xFontStr.Append("-");
		xFontStr.Append( JString((JUInt64) 10*(pointSize+2)) );
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
		xFontStr.Append( JString((JUInt64) 10*pixelWidth) );
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

bool
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
		return false;
	}

	// name

	xFontStr->Append(xName);

	// size

	xFontStr->Append("-");
	xFontStr->Append( JString((JUInt64) size) );

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

	// language

	xFontStr->Append(":lang=");
	xFontStr->Append(JGetStringManager()->GetBCP47Locale());

	// success

	return true;
}

/******************************************************************************
 TrimTrueTypeFontName (static private)

 ******************************************************************************/

static const JRegex nameEndMarkerPattern = "[:,]";

void
JXFontManager::TrimTrueTypeFontName
	(
	JString* s
	)
{
	JStringIterator iter(s);
	if (iter.Next(nameEndMarkerPattern))
	{
		iter.SkipPrev();
		iter.RemoveAllNext();
	}
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

	while (true)
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
			style.bold = false;
			size       = origSize;
		}
		else if (style.italic)
		{
			style.italic = false;
			size         = origSize;
		}
		else if (name != JFontManager::GetDefaultFontName())
		{
			name  = JFontManager::GetDefaultFontName();
			size  = origSize;
			style = origStyle;
		}
		else
		{
			// this should never happen, but it does with some Win95 X servers

			std::cerr << "The X server does not have any 75 dpi PostScript fonts, not even ";
			std::cerr << JFontManager::GetDefaultFontName() << std::endl;
			std::cerr << "Please install the xorg-x11-fonts-75dpi package" << std::endl;
			JXApplication::Abort(JXDocumentManager::kServerDead, false);
		}

		if (GetNewFont(name, size, style, xfont))
		{
			break;
		}
	}
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
			iter.SetPrev(c.ToUpper(), kJIteratorStay);
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
 ShouldIgnore (private)

	Returns true if the font should be ignored.

 ******************************************************************************/

bool
JXFontManager::ShouldIgnore
	(
	const JString& name
	)
{
	return name.GetFirstCharacter() == '.';
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
	if (xfset != nullptr)
{
		assert( type == kStdType );
		XFreeFontSet(*display, xfset);
}

	if (xftt != nullptr)
{
		assert( type == kTrueType );
		XftFontClose(*display, xftt);
}
}
