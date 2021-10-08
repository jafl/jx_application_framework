/******************************************************************************
 JFontStyle.h

	Font information for drawing text.

	Copyright (C) 2016-18 by John Lindal.

 ******************************************************************************/

#ifndef _H_JFont
#define _H_JFont

#include "jx-af/jcore/JFontStyle.h"
#include "jx-af/jcore/JUtf8Character.h"
#include "jx-af/jcore/JRunArray.h"

class JString;
class JFontManager;

using JFontID                = unsigned long;
const JFontID kInvalidFontID = 0;

class JFont
{
	friend int operator==(const JFont& f1, const JFont& f2);

public:

	// default copy ctor and operator=

	void	Set(const JString& name, const JSize size = 0, const JFontStyle& style = JFontStyle());
	void	Set(const JFont& f);

	JFontID	GetID() const;

	const JString&	GetName() const;
	void			SetName(const JString& name);

	JSize	GetSize() const;
	void	SetSize(const JSize size);

	const JFontStyle&	GetStyle() const;
	void				SetStyle(const JFontStyle& style);
	void				ClearStyle();

	void	SetBold(const bool bold);
	void	SetItalic(const bool italic);
	void	SetUnderlineCount(const JSize count);
	void	SetStrike(const bool strike);
	void	SetColor(const JColorID color);

	JSize	GetLineHeight(JFontManager* fontManager) const;
	JSize	GetLineHeight(JFontManager* fontManager,
						  JCoordinate* ascent, JCoordinate* descent) const;

	JSize	GetCharWidth(JFontManager* fontManager, const JUtf8Character& c) const;
	JSize	GetStringWidth(JFontManager* fontManager, const JString& str) const;

	JSize	GetStrikeThickness() const;
	JSize	GetUnderlineThickness() const;

	bool	IsExact(JFontManager* fontManager) const;
	bool	HasGlyphsForString(JFontManager* fontManager, const JString& s) const;
	bool	HasGlyphForCharacter(JFontManager* fontManager, const JUtf8Character& c) const;
	bool	SubstituteToDisplayGlyph(JFontManager* fontManager, const JUtf8Character& c);

private:

	JFontID		itsID;
	JSize		itsSize;
	JFontStyle	itsStyle;

private:

	void	UpdateID();

private:

	// can only get new JFont instances from JFontManager

	friend class JFontManager;

	JFont(const JFontID id, const JSize size, const JFontStyle& style)
		:
		itsID(id), itsSize(size), itsStyle(style)
	{ };

	void	SetID(const JFontID id);

public:		// collections must be able to use new[]  :(

	JFont()
		:
		itsID(kInvalidFontID), itsSize(0)
	{ };
};


/******************************************************************************
 GetID

 ******************************************************************************/

inline JFontID
JFont::GetID()
	const
{
	return itsID;
}

/******************************************************************************
 GetSize

 ******************************************************************************/

inline JSize
JFont::GetSize()
	const
{
	return itsSize;
}

/******************************************************************************
 FontStyle

 ******************************************************************************/

inline const JFontStyle&
JFont::GetStyle()
	const
{
	return itsStyle;
}

inline void
JFont::ClearStyle()
{
	SetStyle(JFontStyle());
}

/******************************************************************************
 Font operators

 ******************************************************************************/

inline int
operator!=
	(
	const JFont& f1,
	const JFont& f2
	)
{
	return !(f1 == f2);
}

#endif
