/******************************************************************************
 JXTextRadioButton.h

	Interface for the JXTextRadioButton class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXTextRadioButton
#define _H_JXTextRadioButton

#include "JXRadioButton.h"
#include <JString.h>
#include <JFont.h>

class JXTextRadioButton : public JXRadioButton
{
public:

	JXTextRadioButton(const JIndex id, const JString& label,
					  JXRadioGroup* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	virtual ~JXTextRadioButton();

	const JString&	GetLabel() const;
	void			SetLabel(const JString& label);

	const JFont&	GetFont() const;

	void	SetFontName(const JString& fontName);
	void	SetFontSize(const JSize size);
	void	SetFontStyle(const JFontStyle& style);
	void	SetFont(const JFont& f);

	void	SetNormalColor(const JColorID color);
	void	SetPushedColor(const JColorID color);

	virtual void	SetShortcuts(const JString& list) override;

	JCoordinate		GetPreferredWidth() const;

	virtual JString	ToString() const override;

protected:

	virtual void		Draw(JXWindowPainter& p, const JRect& rect) override;
	virtual void		DrawBorder(JXWindowPainter& p, const JRect& frame) override;
	virtual JCoordinate	GetFTCMinContentSize(const bool horizontal) const override;

private:

	JString		itsLabel;
	JString*	itsShortcuts;	// can be nullptr
	JIndex		itsULIndex;
	JFont		itsFont;

	JColorID	itsNormalColor;
	JColorID	itsPushedColor;

private:

	// not allowed

	JXTextRadioButton(const JXTextRadioButton& source);
	const JXTextRadioButton& operator=(const JXTextRadioButton& source);
};


/******************************************************************************
 Label routines

 ******************************************************************************/

inline const JString&
JXTextRadioButton::GetLabel()
	const
{
	return itsLabel;
}


inline const JFont&
JXTextRadioButton::GetFont()
	const
{
	return itsFont;
}

inline void
JXTextRadioButton::SetFontName
	(
	const JString& fontName
	)
{
	itsFont.SetName(fontName);
	Refresh();
}

inline void
JXTextRadioButton::SetFontSize
	(
	const JSize size
	)
{
	itsFont.SetSize(size);
	Refresh();
}

inline void
JXTextRadioButton::SetFontStyle
	(
	const JFontStyle& style
	)
{
	itsFont.SetStyle(style);
	Refresh();
}

inline void
JXTextRadioButton::SetFont
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
JXTextRadioButton::SetNormalColor
	(
	const JColorID color
	)
{
	itsNormalColor = color;
	if (!IsChecked())
		{
		Refresh();
		}
}

inline void
JXTextRadioButton::SetPushedColor
	(
	const JColorID color
	)
{
	itsPushedColor = color;
	if (IsChecked())
		{
		Refresh();
		}
}

#endif
