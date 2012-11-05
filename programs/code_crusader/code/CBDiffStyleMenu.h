/******************************************************************************
 CBDiffStyleMenu.h

	Interface for the CBDiffStyleMenu class

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBDiffStyleMenu
#define _H_CBDiffStyleMenu

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXStyleMenu.h>

class CBDiffStyleMenu : public JXStyleMenu
{
public:

	CBDiffStyleMenu(const JCharacter* title, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	virtual ~CBDiffStyleMenu();

	const JFontStyle&	GetStyle() const;
	void				SetStyle(const JFontStyle& style);

	void	ReadStyle(istream& input);
	void	WriteStyle(ostream& output);

protected:

	virtual JFontStyle	GetFontStyleForMenuUpdate() const;
	virtual void		HandleMenuItem(const JIndex menuItem);

private:

	JFontStyle	itsStyle;

private:

	// not allowed

	CBDiffStyleMenu(const CBDiffStyleMenu& source);
	const CBDiffStyleMenu& operator=(const CBDiffStyleMenu& source);
};


/******************************************************************************
 GetStyle

 ******************************************************************************/

inline const JFontStyle&
CBDiffStyleMenu::GetStyle()
	const
{
	return itsStyle;
}

/******************************************************************************
 SetStyle

 ******************************************************************************/

inline void
CBDiffStyleMenu::SetStyle
	(
	const JFontStyle& style
	)
{
	itsStyle = style;
}

#endif
