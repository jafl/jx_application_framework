/******************************************************************************
 JXCurrentPathMenu.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXCurrentPathMenu
#define _H_JXCurrentPathMenu

#include "jx-af/jx/JXTextMenu.h"

class JXCurrentPathMenu : public JXTextMenu
{
public:

	JXCurrentPathMenu(const JString& path, JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	JXCurrentPathMenu(const JString& path,
					  JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	~JXCurrentPathMenu();

	JString	GetPath(const Message& message) const;
	JString	GetPath(const JIndex itemIndex) const;
	void	SetPath(const JString& path);

private:

	// not owned

	JXImage*	itsFolderIcon;
	JXImage*	itsReadOnlyFolderIcon;

private:

	void		JXCurrentPathMenuX(const JString& path);
	JXImage*	GetIcon(const JString& path) const;
};

#endif
