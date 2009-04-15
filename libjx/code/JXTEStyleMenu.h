/******************************************************************************
 JXTEStyleMenu.h

	Interface for the JXTEStyleMenu class

	Copyright © 1996-97 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXTEStyleMenu
#define _H_JXTEStyleMenu

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXStyleMenu.h>

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

protected:

	virtual JFontStyle	GetFontStyleForMenuUpdate() const;
	virtual void		HandleMenuItem(const JIndex menuItem);

private:

	JXTextEditor*	itsTE;		// we don't own this

private:

	// not allowed

	JXTEStyleMenu(const JXTEStyleMenu& source);
	const JXTEStyleMenu& operator=(const JXTEStyleMenu& source);
};

#endif
