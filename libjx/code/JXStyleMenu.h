/******************************************************************************
 JXStyleMenu.h

	Interface for the JXStyleMenu class

	Copyright © 1996-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXStyleMenu
#define _H_JXStyleMenu

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXTextMenu.h>

class JXStyleMenuTable;
class JXChooseColorDialog;

class JXStyleMenu : public JXTextMenu
{
	friend class JXStyleMenuTable;

public:

	JXStyleMenu(const JBoolean allowChooseCustomColors,
				JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	JXStyleMenu(const JBoolean allowChooseCustomColors,
				JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	virtual ~JXStyleMenu();

protected:

	virtual JXMenuDirector*	CreateMenuWindow(JXWindowDirector* supervisor);

	virtual void		UpdateMenu();		// must call inherited
	virtual JFontStyle	GetFontStyleForMenuUpdate() const = 0;
	virtual void		HandleMenuItem(const JIndex menuItem) = 0;
	virtual void		HandleCustomColor(const JColorIndex color);
	void				UpdateStyle(const JIndex menuItem, JFontStyle* style);
	JColorIndex			GetSelectedColor() const;

	JColorIndex	IndexToColor(const JIndex menuIndex) const;
	JIndex		ColorToIndex(const JColorIndex color) const;

	virtual void	Receive(JBroadcaster* sender, const Message& message);

protected:

	enum
	{
		kPlainStyleCmd = 1,
		kBoldStyleCmd,
		kItalicStyleCmd,
		kUnderlineStyleCmd,
		kDblUnderlineStyleCmd,
		kStrikeStyleCmd,
		kFirstColorCmd,

		kColorCount     = 11,		// last one reserved for "other"
		kCustomColorCmd = kFirstColorCmd + kColorCount - 1
	};

private:

	const JBoolean	itsChooseCustomColorFlag;

	JColorIndex	itsColorIndex;
	JColorIndex	itsColorList[ kColorCount ];

	// used when selecting custom color

	JXChooseColorDialog*	itsChooseColorDialog;	// can be NULL

private:

	void	JXStyleMenuX();

	void	ChooseColor();
	void	SetCustomColor(const JColorIndex color);

	// not allowed

	JXStyleMenu(const JXStyleMenu& source);
	const JXStyleMenu& operator=(const JXStyleMenu& source);
};


/******************************************************************************
 GetSelectedColor (protected)

 ******************************************************************************/

inline JColorIndex
JXStyleMenu::GetSelectedColor()
	const
{
	return itsColorIndex;
}

#endif
