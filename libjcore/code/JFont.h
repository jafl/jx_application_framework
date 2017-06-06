/******************************************************************************
 JFontStyle.h

	Font information for drawing text.

	Copyright (C) 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JFont
#define _H_JFont

#include <JFontStyle.h>
#include <JUtf8Character.h>

class JFontManager;
class JString;

template <class T> class JList;
template <class T> class JArray;
template <class T> class JRunArray;
template <class T> class JRunArrayElement;

typedef unsigned long JFontID;

const JSize kJDefaultFontSize             = 10;
const JSize kJDefaultRowColHeaderFontSize = 9;
const JSize kJDefaultMonoFontSize         = 9;

class JFont
{
	friend int operator==(const JFont& f1, const JFont& f2);

public:

	JFont(const JFont& source)
	{
		itsFontMgr = source.itsFontMgr;
		itsID      = source.itsID;
		itsSize    = source.itsSize;
		itsStyle   = source.itsStyle;
	};

	const JFont&	operator=(const JFont& source);

	const JFontManager*	GetFontManager() const;

	void	Set(const JString& name, const JSize size = kJDefaultFontSize, const JFontStyle style = JFontStyle());
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
	void	SetColor(const JColorIndex color);

	JSize	GetLineHeight() const;
	JSize	GetLineHeight(JCoordinate* ascent, JCoordinate* descent) const;

	JSize	GetCharWidth(const JUtf8Character& c) const;
	JSize	GetStringWidth(const JString& str) const;

	JSize	GetStrikeThickness() const;
	JSize	GetUnderlineThickness() const;

private:

	const JFontManager*	itsFontMgr;
	JFontID				itsID;
	JSize				itsSize;
	JFontStyle			itsStyle;

private:

	void	UpdateID();

private:

	// can only get new JFont instances from JFontManager

	friend class JFontManager;

	JFont(const JFontManager* mgr, const JFontID id,
		  const JSize size, const JFontStyle& style)
		:
		itsFontMgr(mgr), itsID(id), itsSize(size), itsStyle(style)
	{ };

	void	SetID(const JFontID id);

public:		// collections must be able to use new[]  :(

	JFont()
		:
		itsFontMgr(NULL), itsID(0), itsSize(0)
	{ };
};


/******************************************************************************
 GetFontManager

 ******************************************************************************/

inline const JFontManager*
JFont::GetFontManager()
	const
{
	return itsFontMgr;
}

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
 GetFontSize

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
