/******************************************************************************
 JXCurrentPathMenu.h

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXCurrentPathMenu
#define _H_JXCurrentPathMenu

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXTextMenu.h>

class JXCurrentPathMenu : public JXTextMenu
{
public:

	JXCurrentPathMenu(const JCharacter* path, JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	JXCurrentPathMenu(const JCharacter* path,
					  JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	virtual ~JXCurrentPathMenu();

	JString	GetPath(const Message& message) const;
	JString	GetPath(const JIndex itemIndex) const;
	void	SetPath(const JCharacter* path);

private:

	JXImage*	itsFolderIcon;
	JXImage*	itsReadOnlyFolderIcon;

private:

	void		JXCurrentPathMenuX(const JCharacter* path);
	JXImage*	GetIcon(const JCharacter* path) const;

	// not allowed

	JXCurrentPathMenu(const JXCurrentPathMenu& source);
	const JXCurrentPathMenu& operator=(const JXCurrentPathMenu& source);
};

#endif
