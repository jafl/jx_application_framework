/******************************************************************************
 JXTEStyleMenu.h

	Interface for the JXTEStyleMenu class

	Copyright (C) 1996-97 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXTEStyleMenu
#define _H_JXTEStyleMenu

#include "jx-af/jx/JXStyleMenu.h"

class JXTextEditor;

class JXTEStyleMenu : public JXStyleMenu
{
public:

	JXTEStyleMenu(JXTextEditor* te, JXContainer* enclosure,
				  const HSizingOption hSizing, const VSizingOption vSizing,
				  const JCoordinate x, const JCoordinate y,
				  const JCoordinate w, const JCoordinate h);

	JXTEStyleMenu(JXTextEditor* te,
				  JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	virtual ~JXTEStyleMenu();

	void	SetTE(JXTextEditor* te);

protected:

	virtual JFontStyle	GetFontStyleForMenuUpdate() const;
	virtual void		HandleMenuItem(const JIndex menuItem);

private:

	JXTextEditor*	itsTE;		// we don't own this

};


/******************************************************************************
 SetTE

	Allows menu to be shared by multiple widgets.

 ******************************************************************************/

inline void
JXTEStyleMenu::SetTE
	(
	JXTextEditor* te
	)
{
	itsTE = te;
}

#endif
