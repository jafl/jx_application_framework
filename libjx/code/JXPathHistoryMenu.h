/******************************************************************************
 JXPathHistoryMenu.h

	Interface for the JXPathHistoryMenu class

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXPathHistoryMenu
#define _H_JXPathHistoryMenu

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXStringHistoryMenu.h>
#include <JString.h>

class JXPathHistoryMenu : public JXStringHistoryMenu
{
public:

	JXPathHistoryMenu(const JSize historyLength,
					  const JCharacter* title, JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	JXPathHistoryMenu(const JSize historyLength,
					  JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	virtual ~JXPathHistoryMenu();

	const JString&	GetBasePath() const;
	void			SetBasePath(const JCharacter* path);
	void			ClearBasePath();

protected:

	virtual void	UpdateMenu();		// must call inherited

private:

	JString	itsBasePath;

private:

	void	JXPathHistoryMenuX();
	void	RemoveInvalidPaths();

	// not allowed

	JXPathHistoryMenu(const JXPathHistoryMenu& source);
	const JXPathHistoryMenu& operator=(const JXPathHistoryMenu& source);
};


/******************************************************************************
 GetBasePath

 ******************************************************************************/

inline const JString&
JXPathHistoryMenu::GetBasePath()
	const
{
	return itsBasePath;
}

/******************************************************************************
 ClearBasePath

 ******************************************************************************/

inline void
JXPathHistoryMenu::ClearBasePath()
{
	itsBasePath.Clear();
}

#endif
