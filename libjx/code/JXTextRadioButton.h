/******************************************************************************
 JXTextRadioButton.h

	Interface for the JXTextRadioButton class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXTextRadioButton
#define _H_JXTextRadioButton

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXRadioButton.h>
#include <JString.h>
#include <JFontStyle.h>

class JXTextRadioButton : public JXRadioButton
{
public:

	JXTextRadioButton(const JIndex id, const JCharacter* label,
					  JXRadioGroup* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	virtual ~JXTextRadioButton();

	const JString&	GetLabel() const;
	void			SetLabel(const JCharacter* label);

	const JString&	GetFontName() const;
	JSize			GetFontSize() const;
	JFontStyle		GetFontStyle() const;

	void	SetFontName(const JCharacter* fontName);
	void	SetFontSize(const JSize size);
	void	SetFontStyle(const JFontStyle& style);

	void	SetNormalColor(const JColorIndex color);
	void	SetPushedColor(const JColorIndex color);

	virtual void	SetShortcuts(const JCharacter* list);

	JCoordinate		GetPreferredWidth() const;

	virtual void	Activate();
	virtual void	Deactivate();

protected:

	virtual void	Draw(JXWindowPainter& p, const JRect& rect);
	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame);

private:

	JString		itsLabel;
	JString*	itsShortcuts;	// can be NULL
	JIndex		itsULIndex;
	JString		itsFontName;
	JSize		itsFontSize;
	JFontStyle	itsFontStyle;

	JColorIndex	itsNormalColor;
	JColorIndex	itsPushedColor;

	// saves label color while deactivated

	JColorIndex	itsTrueLabelColor;

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

inline const JString&
JXTextRadioButton::GetFontName()
	const
{
	return itsFontName;
}

inline JSize
JXTextRadioButton::GetFontSize()
	const
{
	return itsFontSize;
}

inline void
JXTextRadioButton::SetFontSize
	(
	const JSize size
	)
{
	itsFontSize = size;
	Refresh();
}

/******************************************************************************
 Set button colors

 ******************************************************************************/

inline void
JXTextRadioButton::SetNormalColor
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
JXTextRadioButton::SetPushedColor
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
