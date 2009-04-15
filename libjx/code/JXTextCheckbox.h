/******************************************************************************
 JXTextCheckbox.h

	Interface for the JXTextCheckbox class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXTextCheckbox
#define _H_JXTextCheckbox

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXCheckbox.h>
#include <JString.h>
#include <JFontStyle.h>

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

inline const JString&
JXTextCheckbox::GetFontName()
	const
{
	return itsFontName;
}

inline JSize
JXTextCheckbox::GetFontSize()
	const
{
	return itsFontSize;
}

inline void
JXTextCheckbox::SetFontSize
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
