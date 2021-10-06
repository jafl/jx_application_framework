/******************************************************************************
 JFont.cpp

	Stores complete font information required to render.

	Copyright (C) 2016-18 by John Lindal.

 ******************************************************************************/

#include "jx-af/jcore/JFont.h"
#include "jx-af/jcore/JFontManager.h"
#include "jx-af/jcore/JStringIterator.h"
#include "jx-af/jcore/jGlobals.h"
#include "jx-af/jcore/jAssert.h"

/******************************************************************************
 Set

 ******************************************************************************/

void
JFont::Set
	(
	const JString&		name,
	const JSize			size,
	const JFontStyle&	style
	)
{
	itsSize  = size > 0 ? size : JFontManager::GetDefaultFontSize();
	itsStyle = style;
	SetName(name);
}

void
JFont::Set
	(
	const JFont& f
	)
{
	itsSize  = f.itsSize;
	itsStyle = f.itsStyle;
	SetName(f.GetName());
}

/******************************************************************************
 SetFontID (private)

 ******************************************************************************/

void
JFont::SetID
	(
	const JFontID id
	)
{
	itsID = id;
}

/******************************************************************************
 FontName

 ******************************************************************************/

const JString&
JFont::GetName()
	const
{
	return JFontManager::GetFontName(itsID);
}

void
JFont::SetName
	(
	const JString& name
	)
{
	itsID = JFontManager::GetFontID(name, itsSize, itsStyle);
}

/******************************************************************************
 FontSize

 ******************************************************************************/

void
JFont::SetSize
	(
	const JSize size
	)
{
	if (size != itsSize)
	{
		itsSize = size;
		UpdateID();
	}
}

/******************************************************************************
 FontStyle

 ******************************************************************************/

void
JFont::SetStyle
	(
	const JFontStyle& style
	)
{
	if (style != itsStyle)
	{
		itsStyle = style;
		UpdateID();
	}
}

void
JFont::SetBold
	(
	const bool bold
	)
{
	if (bold != itsStyle.bold)
	{
		itsStyle.bold = bold;
		UpdateID();
	}
}

void
JFont::SetItalic
	(
	const bool italic
	)
{
	if (italic != itsStyle.italic)
	{
		itsStyle.italic = italic;
		UpdateID();
	}
}

void
JFont::SetUnderlineCount
	(
	const JSize count
	)
{
	itsStyle.underlineCount = count;
}

void
JFont::SetStrike
	(
	const bool strike
	)
{
	itsStyle.strike = strike;
}

void
JFont::SetColor
	(
	const JColorID color
	)
{
	itsStyle.color = color;
}

/******************************************************************************
 Mirror JFontManager

 ******************************************************************************/

JSize
JFont::GetLineHeight
	(
	JFontManager* fontManager
	)
	const
{
	JCoordinate ascent, descent;
	return fontManager->GetLineHeight(itsID, itsSize, itsStyle, &ascent, &descent);
}

JSize
JFont::GetLineHeight
	(
	JFontManager*	fontManager,
	JCoordinate*	ascent,
	JCoordinate*	descent
	)
	const
{
	return fontManager->GetLineHeight(itsID, itsSize, itsStyle, ascent, descent);
}

JSize
JFont::GetCharWidth
	(
	JFontManager*			fontManager,
	const JUtf8Character&	c
	)
	const
{
	return fontManager->GetCharWidth(itsID, c);
}

JSize
JFont::GetStringWidth
	(
	JFontManager*	fontManager,
	const JString&	str
	)
	const
{
	return fontManager->GetStringWidth(itsID, str);
}

JSize
JFont::GetStrikeThickness()
	const
{
	return JFontManager::GetStrikeThickness(itsSize);
}

JSize
JFont::GetUnderlineThickness()
	const
{
	return JFontManager::GetUnderlineThickness(itsSize);
}

/******************************************************************************
 IsExact

	Returns true if the font matches exactly what was requested.

 ******************************************************************************/

bool
JFont::IsExact
	(
	JFontManager* fontManager
	)
	const
{
	return fontManager->IsExact(itsID);
}

/******************************************************************************
 HasGlyphsForString

 ******************************************************************************/

bool
JFont::HasGlyphsForString
	(
	JFontManager*	fontManager,
	const JString&	s
	)
	const
{
	JStringIterator iter(s);
	JUtf8Character c;
	while (iter.Next(&c))
	{
		if (!fontManager->HasGlyphForCharacter(itsID, c))
		{
			return false;
		}
	}

	return true;
}

/******************************************************************************
 HasGlyphForCharacter

 ******************************************************************************/

bool
JFont::HasGlyphForCharacter
	(
	JFontManager*			fontManager,
	const JUtf8Character&	c
	)
	const
{
	return fontManager->HasGlyphForCharacter(itsID, c);
}

/******************************************************************************
 SubstituteToDisplayGlyph

	Returns true if the font name had to be changed

 ******************************************************************************/

bool
JFont::SubstituteToDisplayGlyph
	(
	JFontManager*			fontManager,
	const JUtf8Character&	c
	)
{
	if (!fontManager->HasGlyphForCharacter(itsID, c))
	{
		JString name;
		if (fontManager->GetSubstituteFontName(*this, c, &name))
		{
			SetName(name);
			return true;
		}
	}

	return false;
}

/******************************************************************************
 UpdateID (private)

 ******************************************************************************/

void
JFont::UpdateID()
{
	itsID = JFontManager::GetFontID(JFontManager::GetFontName(itsID), itsSize, itsStyle);
}

/******************************************************************************
 Font operators

 ******************************************************************************/

int
operator==
	(
	const JFont& f1,
	const JFont& f2
	)
{
	return (f1.itsID == f2.itsID && f1.itsSize == f2.itsSize && f1.itsStyle == f2.itsStyle);
}
