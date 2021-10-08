/******************************************************************************
 JXTextButton.h

	Interface for the JXTextButton class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXTextButton
#define _H_JXTextButton

#include "jx-af/jx/JXButton.h"
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/JFont.h>

class JXTextButton : public JXButton
{
public:

	JXTextButton(const JString& label, JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	~JXTextButton();

	const JString&	GetLabel() const;
	void			SetLabel(const JString& label);

	const JFont&	GetFont() const;

	void	SetFontName(const JString& fontName);
	void	SetFontSize(const JSize size);
	void	SetFontStyle(const JFontStyle& style);
	void	SetFont(const JFont& f);

	void	SetNormalColor(const JColorID color);
	void	SetPushedColor(const JColorID color);

	const JPoint&	GetPadding() const;
	void			SetPaddingBeforeFTC(const JPoint& p);

	void	SetShortcuts(const JString& list) override;

	JString	ToString() const override;

protected:

	void		Draw(JXWindowPainter& p, const JRect& rect) override;
	void		DrawBackground(JXWindowPainter& p, const JRect& frame) override;
	JCoordinate	GetFTCMinContentSize(const bool horizontal) const override;

private:

	JString		itsLabel;
	JString*	itsShortcuts;	// can be nullptr
	JIndex		itsULIndex;
	JFont		itsFont;
	JColorID	itsPushedColor;
	JPoint		itsPadding;

private:

	void	CalcULIndex();
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
	const JString& fontName
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
	const JColorID color
	)
{
	SetBackColor(color);
}

inline void
JXTextButton::SetPushedColor
	(
	const JColorID color
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
