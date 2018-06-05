/******************************************************************************
 JFontStyle.h

	Font information for drawing text.

	Copyright (C) 2016-18 by John Lindal.

 ******************************************************************************/

#ifndef _H_JFont
#define _H_JFont

#include <JFontStyle.h>
#include <JUtf8Character.h>
#include <JRunArray.h>

class JString;
class JFontManager;

typedef unsigned long JFontID;

class JFont
{
	friend int operator==(const JFont& f1, const JFont& f2);

public:

	// default copy ctor and operator=

	void	Set(const JString& name, const JSize size = 0, const JFontStyle style = JFontStyle());
	void	Set(const JFont& f);

	JFontID	GetID() const;

	const JString&	GetName() const;
	void			SetName(const JString& name);

	JSize	GetSize() const;
	void	SetSize(const JSize size);

	const JFontStyle&	GetStyle() const;
	void				SetStyle(const JFontStyle& style);
	void				ClearStyle();

	void	SetBold(const JBoolean bold);
	void	SetItalic(const JBoolean italic);
	void	SetUnderlineCount(const JSize count);
	void	SetStrike(const JBoolean strike);
	void	SetColor(const JColorID color);

	JSize	GetLineHeight(JFontManager* fontManager) const;
	JSize	GetLineHeight(JFontManager* fontManager,
						  JCoordinate* ascent, JCoordinate* descent) const;

	JSize	GetCharWidth(JFontManager* fontManager, const JUtf8Character& c) const;
	JSize	GetStringWidth(JFontManager* fontManager, const JString& str) const;

	JSize	GetStrikeThickness() const;
	JSize	GetUnderlineThickness() const;

	JBoolean	IsExact(JFontManager* fontManager) const;
	JBoolean	HasGlyphForCharacter(JFontManager* fontManager, const JUtf8Character& c) const;
	JBoolean	SubstituteToDisplayGlyph(JFontManager* fontManager, const JUtf8Character& c);

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
		itsID(0), itsSize(0)
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
