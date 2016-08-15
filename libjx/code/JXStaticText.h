/******************************************************************************
 JXStaticText.h

	Interface for the JXStaticText class

	Copyright � 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXStaticText
#define _H_JXStaticText

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXTEBase.h>

class JXStaticText : public JXTEBase
{
public:

	JXStaticText(const JCharacter* text, JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	JXStaticText(const JCharacter* text,
				 const JBoolean wordWrap, const JBoolean selectable,
				 JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	virtual ~JXStaticText();

	void	SetFontName(const JCharacter* name);
	void	SetFontSize(const JSize size);
	void	SetFontStyle(const JFontStyle& style);
	void	SetFont(const JFont& f);

	void	SetBackgroundColor(const JColorIndex color);

	void	SetToLabel();

protected:

	virtual void	BoundsResized(const JCoordinate dw, const JCoordinate dh);

private:

	JBoolean	itsIsLabelFlag;

private:

	void	JXStaticTextX(const JCharacter* text,
						  const JCoordinate w, const JCoordinate h);

	void	CenterVertically();

	// not allowed

	JXStaticText(const JXStaticText& source);
	const JXStaticText& operator=(const JXStaticText& source);
};


/******************************************************************************
 Change font

 ******************************************************************************/

inline void
JXStaticText::SetFontName
	(
	const JCharacter* name
	)
{
	SelectAll();
	SetCurrentFontName(name);
	SetDefaultFontName(name);
}

inline void
JXStaticText::SetFontSize
	(
	const JSize size
	)
{
	SelectAll();
	SetCurrentFontSize(size);
	SetDefaultFontSize(size);
}

inline void
JXStaticText::SetFontStyle
	(
	const JFontStyle& style
	)
{
	SelectAll();
	SetCurrentFontStyle(style);
	SetDefaultFontStyle(style);
}

inline void
JXStaticText::SetFont
	(
	const JFont& f
	)
{
	SelectAll();
	SetCurrentFont(f);
	SetDefaultFont(f);
}

/******************************************************************************
 SetBackgroundColor

 ******************************************************************************/

inline void
JXStaticText::SetBackgroundColor
	(
	const JColorIndex color
	)
{
	SetBackColor(color);
	SetFocusColor(color);
}

#endif
