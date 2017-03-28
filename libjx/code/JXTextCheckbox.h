/******************************************************************************
 JXTextCheckbox.h

	Interface for the JXTextCheckbox class

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXTextCheckbox
#define _H_JXTextCheckbox

#include <JXCheckbox.h>
#include <JString.h>
#include <JFont.h>

class JXTextCheckbox : public JXCheckbox
{
public:

	JXTextCheckbox(const JCharacter* label, JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	virtual ~JXTextCheckbox();

	const JString&	GetLabel() const;
	void			SetLabel(const JCharacter* label);

	const JFont&	GetFont() const;

	void	SetFontName(const JCharacter* fontName);
	void	SetFontSize(const JSize size);
	void	SetFontStyle(const JFontStyle& style);
	void	SetFont(const JFont& f);

	void	SetNormalColor(const JColorIndex color);
	void	SetPushedColor(const JColorIndex color);

	virtual void	SetShortcuts(const JCharacter* list);

	JCoordinate		GetPreferredWidth() const;

protected:

	virtual void	Draw(JXWindowPainter& p, const JRect& rect);
	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame);

private:

	JString		itsLabel;
	JString*	itsShortcuts;	// can be NULL
	JIndex		itsULIndex;
	JFont		itsFont;

	JColorIndex	itsNormalColor;
	JColorIndex	itsPushedColor;

private:

	// not allowed

	JXTextCheckbox(const JXTextCheckbox& source);
	const JXTextCheckbox& operator=(const JXTextCheckbox& source);
};


/******************************************************************************
 Label routines

 ******************************************************************************/

inline const JString&
JXTextCheckbox::GetLabel()
	const
{
	return itsLabel;
}

inline const JFont&
JXTextCheckbox::GetFont()
	const
{
	return itsFont;
}

inline void
JXTextCheckbox::SetFontName
	(
	const JCharacter* fontName
	)
{
	itsFont.SetName(fontName);
	Refresh();
}

inline void
JXTextCheckbox::SetFontSize
	(
	const JSize size
	)
{
	itsFont.SetSize(size);
	Refresh();
}

inline void
JXTextCheckbox::SetFontStyle
	(
	const JFontStyle& style
	)
{
	itsFont.SetStyle(style);
	Refresh();
}

inline void
JXTextCheckbox::SetFont
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
JXTextCheckbox::SetNormalColor
	(
	const JColorIndex color
	)
{
	itsNormalColor = color;
	if (!IsChecked())
		{
		Refresh();
		}
}

inline void
JXTextCheckbox::SetPushedColor
	(
	const JColorIndex color
	)
{
	itsPushedColor = color;
	if (IsChecked())
		{
		Refresh();
		}
}

#endif
