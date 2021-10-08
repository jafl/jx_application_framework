/******************************************************************************
 JXStyleMenu.h

	Interface for the JXStyleMenu class

	Copyright (C) 1996-98 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXStyleMenu
#define _H_JXStyleMenu

#include "jx-af/jx/JXTextMenu.h"

class JXStyleMenuTable;
class JXChooseColorDialog;

class JXStyleMenu : public JXTextMenu
{
	friend class JXStyleMenuTable;

public:

	JXStyleMenu(JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	JXStyleMenu(JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	~JXStyleMenu() override;

protected:

	JXMenuDirector*	CreateMenuWindow(JXWindowDirector* supervisor) override;

	virtual void		UpdateMenu();		// must call inherited
	virtual JFontStyle	GetFontStyleForMenuUpdate() const = 0;
	virtual void		HandleMenuItem(const JIndex menuItem) = 0;
	void				UpdateStyle(const JIndex menuItem, JFontStyle* style);
	JColorID			GetSelectedColor() const;

	JColorID	IndexToColor(const JIndex menuIndex) const;
	JIndex		ColorToIndex(const JColorID color) const;

	void	Receive(JBroadcaster* sender, const Message& message) override;

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

	JColorID	itsColorIndex;
	JColorID	itsColorList[ kColorCount ];

	// used when selecting custom color

	JXChooseColorDialog*	itsChooseColorDialog;	// can be nullptr

private:

	void	JXStyleMenuX();

	void	ChooseColor();
	void	SetCustomColor(const JColorID color);
};


/******************************************************************************
 GetSelectedColor (protected)

 ******************************************************************************/

inline JColorID
JXStyleMenu::GetSelectedColor()
	const
{
	return itsColorIndex;
}

#endif
