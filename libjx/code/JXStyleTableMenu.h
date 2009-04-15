/******************************************************************************
 JXStyleTableMenu.h

	Interface for the JXStyleTableMenu class

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXStyleTableMenu
#define _H_JXStyleTableMenu

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXStyleMenu.h>

class JXStyleTable;

class JXStyleTableMenu : public JXStyleMenu
{
public:

	JXStyleTableMenu(JXStyleTable* table, JXContainer* enclosure,
					 const HSizingOption hSizing, const VSizingOption vSizing,
					 const JCoordinate x, const JCoordinate y,
					 const JCoordinate w, const JCoordinate h);

	JXStyleTableMenu(JXStyleTable* table,
					 JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	virtual ~JXStyleTableMenu();

protected:

	virtual void		UpdateMenu();
	virtual JFontStyle	GetFontStyleForMenuUpdate() const;
	virtual void		HandleMenuItem(const JIndex menuItem);

private:

	JXStyleTable*	itsTable;		// we don't own this

private:

	// not allowed

	JXStyleTableMenu(const JXStyleTableMenu& source);
	const JXStyleTableMenu& operator=(const JXStyleTableMenu& source);
};

#endif
