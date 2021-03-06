/******************************************************************************
 JXTextButton.h

	Interface for the JXTextButton class

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXTextButton
#define _H_JXTextButton

#include <JXButton.h>
#include <JString.h>
#include <JFont.h>

class JXTextButton : public JXButton
{
public:

	JXTextButton(const JCharacter* label, JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	virtual ~JXTextButton();

	const JString&	GetLabel() const;
	void			SetLabel(const JCharacter* label);

	const JFont&	GetFont() const;

	void	SetFontName(const JCharacter* fontName);
	void	SetFontSize(const JSize size);
	void	SetFontStyle(const JFontStyle& style);
	void	SetFont(const JFont& f);

	void	SetNormalColor(const JColorIndex color);
	void	SetPushedColor(const JColorIndex color);

	const JPoint&	GetPadding() const;
	void			SetPaddingBeforeFTC(const JPoint& p);

	virtual void	SetShortcuts(const JCharacter* list);

	virtual JString	ToString() const;

protected:

	virtual void		Draw(JXWindowPainter& p, const JRect& rect);
	virtual void		DrawBackground(JXWindowPainter& p, const JRect& frame);
	virtual JCoordinate	GetFTCMinContentSize(const JBoolean horizontal) const;

private:

	JString		itsLabel;
	JString*	itsShortcuts;	// can be NULL
	JIndex		itsULIndex;
	JFont		itsFont;
	JColorIndex	itsPushedColor;
	JPoint		itsPadding;

private:

	void	CalcULIndex();

	// not allowed

	JXTextButton(const JXTextButton& source);
	const JXTextButton& operator=(const JXTextButton& source);
};


/******************************************************************************
 Label routines

 ******************************************************************************/

inline const JString&
JXTextButton::GetLabel()
	const
{
	return itsLabel;
}

inline const JFont&
JXTextButton::GetFont()
	const
{
	return itsFont;
}

inline void
JXTextButton::SetFontName
	(
	const JCharacter* fontName
	)
{
	itsFont.SetName(fontName);
	Refresh();
}

inline void
JXTextButton::SetFontSize
	(
	const JSize size
	)
{
	itsFont.SetSize(size);
	Refresh();
}

inline void
JXTextButton::SetFontStyle
	(
	const JFontStyle& style
	)
{
	itsFont.SetStyle(style);
	Refresh();
}

inline void
JXTextButton::SetFont
	(
	const JFont& font
	)
{
	itsFont = font;
	Refresh();
}

/******************************************************************************
 Set button colors

 ******************************************************************************/

inline void
JXTextButton::SetNormalColor
	(
	const JColorIndex color
	)
{
	SetBackColor(color);
}

inline void
JXTextButton::SetPushedColor
	(
	const JColorIndex color
	)
{
	itsPushedColor = color;
	if (IsPushed())
		{
		Refresh();
		}
}

/******************************************************************************
 Padding

 ******************************************************************************/

inline const JPoint&
JXTextButton::GetPadding()
	const
{
	return itsPadding;
}

inline void
JXTextButton::SetPaddingBeforeFTC
	(
	const JPoint& p
	)
{
	itsPadding = p;
}

#endif
