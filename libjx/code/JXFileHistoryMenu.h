/******************************************************************************
 JXFileHistoryMenu.h

	Interface for the JXFileHistoryMenu class

	Copyright (C) 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXFileHistoryMenu
#define _H_JXFileHistoryMenu

#include <JXHistoryMenuBase.h>

class JXFileHistoryMenu : public JXHistoryMenuBase
{
public:

	JXFileHistoryMenu(const JSize historyLength,
					  const JCharacter* title, JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	JXFileHistoryMenu(const JSize historyLength,
					  JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	virtual ~JXFileHistoryMenu();

	void	AddFile(const JCharacter* fullName);
	void	AddFile(const JCharacter* path, const JCharacter* name);

	JString	GetFile(const Message& message) const;
	JString	GetFile(const JIndex index) const;

protected:

	virtual void	UpdateMenu();		// must call inherited

private:

	void	JXFileHistoryMenuX();
	void	RemoveNonexistentFiles();

	// not allowed

	JXFileHistoryMenu(const JXFileHistoryMenu& source);
	const JXFileHistoryMenu& operator=(const JXFileHistoryMenu& source);
};

#endif
