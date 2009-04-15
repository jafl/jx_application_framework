/******************************************************************************
 JXTextButton.h

	Interface for the JXTextButton class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXTextButton
#define _H_JXTextButton

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXButton.h>
#include <JString.h>
#include <JFontStyle.h>

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

	const JString&	GetFontName() const;
	JSize			GetFontSize() const;
	JFontStyle		GetFontStyle() const;

	void	SetFontName(const JCharacter* fontName);
	void	SetFontSize(const JSize size);
	void	SetFontStyle(const JFontStyle& style);

	void	SetNormalColor(const JColorIndex color);
	void	SetPushedColor(const JColorIndex color);

	virtual void	SetShortcuts(const JCharacter* list);

	virtual void	Activate();
	virtual void	Deactivate();

protected:

	virtual void	Draw(JXWindowPainter& p, const JRect& rect);
	virtual void	DrawBackground(JXWindowPainter& p, const JRect& frame);

private:

	JString		itsLabel;
	JString*	itsShortcuts;	// can be NULL
	JIndex		itsULIndex;
	JString		itsFontName;
	JSize		itsFontSize;
	JFontStyle	itsFontStyle;
	JColorIndex	itsPushedColor;

	// saves label color while deactivated

	JColorIndex	itsTrueLabelColor;

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

inline const JString&
JXTextButton::GetFontName()
	const
{
	return itsFontName;
}

inline JSize
JXTextButton::GetFontSize()
	const
{
	return itsFontSize;
}

inline void
JXTextButton::SetFontSize
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

#endif
