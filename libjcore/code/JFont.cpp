/******************************************************************************
 JFont.cpp

	Stores complete font information required to render.

	Copyright (C) 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JFont.h>
#include <JFontManager.h>
#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 Set

 ******************************************************************************/

void
JFont::Set
	(
	const JString&		name,
	const JSize			size,
	const JFontStyle	style
	)
{
	itsSize  = size;
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
	assert( itsFontMgr != NULL );
	assert( itsFontMgr->GetFontName(id) != NULL );
	itsID = id;
}

/******************************************************************************
 FontName

 ******************************************************************************/

const JString&
JFont::GetName()
	const
{
	assert( itsFontMgr != NULL );
	return itsFontMgr->GetFontName(itsID);
}

void
JFont::SetName
	(
	const JString& name
	)
{
	assert( itsFontMgr != NULL );
	itsID = itsFontMgr->GetFontID(name, itsSize, itsStyle);
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
	const JBoolean bold
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
	const JBoolean italic
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
	const JBoolean strike
	)
{
	itsStyle.strike = strike;
}

void
JFont::SetColor
	(
	const JColorIndex color
	)
{
	itsStyle.color = color;
}

/******************************************************************************
 Mirror JFontManager

 ******************************************************************************/

JSize
JFont::GetLineHeight()
	const
{
	assert( itsFontMgr != NULL );

	JCoordinate ascent, descent;
	return itsFontMgr->GetLineHeight(itsID, itsSize, itsStyle, &ascent, &descent);
}

JSize
JFont::GetLineHeight
	(
	JCoordinate* ascent,
	JCoordinate* descent
	)
	const
{
	assert( itsFontMgr != NULL );
	return itsFontMgr->GetLineHeight(itsID, itsSize, itsStyle, ascent, descent);
}

JSize
JFont::GetCharWidth
	(
	const JUtf8Character& c
	)
	const
{
	assert( itsFontMgr != NULL );
	return itsFontMgr->GetCharWidth(itsID, c);
}

JSize
JFont::GetStringWidth
	(
	const JString& str
	)
	const
{
	assert( itsFontMgr != NULL );
	return itsFontMgr->GetStringWidth(itsID, str);
}

JSize
JFont::GetStrikeThickness()
	const
{
	assert( itsFontMgr != NULL );
	return itsFontMgr->GetStrikeThickness(itsSize);
}

JSize
JFont::GetUnderlineThickness()
	const
{
	assert( itsFontMgr != NULL );
	return itsFontMgr->GetUnderlineThickness(itsSize);
}

/******************************************************************************
 UpdateID (private)

 ******************************************************************************/

void
JFont::UpdateID()
{
	assert( itsFontMgr != NULL );
	itsID = itsFontMgr->GetFontID(itsFontMgr->GetFontName(itsID), itsSize, itsStyle);
}

/******************************************************************************
 Font operators

 ******************************************************************************/

const JFont&
JFont::operator=
	(
	const JFont& source
	)
{
	assert( source.itsFontMgr != NULL );

	if (this == &source)
		{
		return *this;
		}

	if (itsFontMgr == NULL)		// blanks created by JRunArray
		{
		itsFontMgr = source.itsFontMgr;
		itsID      = source.itsID;
		itsSize    = source.itsSize;
		itsStyle   = source.itsStyle;
		}
	else
		{
		itsSize  = source.itsSize;
		itsStyle = source.itsStyle;
		SetName(source.GetName());
		}

	return *this;
}

int
operator==
	(
	const JFont& f1,
	const JFont& f2
	)
{
	return (f1.itsFontMgr == f2.itsFontMgr && f1.itsID == f2.itsID &&
			f1.itsSize == f2.itsSize && f1.itsStyle == f2.itsStyle);
}
