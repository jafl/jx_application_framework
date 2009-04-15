/******************************************************************************
 JXStringHistoryMenu.h

	Interface for the JXStringHistoryMenu class

	Copyright © 1997-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXStringHistoryMenu
#define _H_JXStringHistoryMenu

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXHistoryMenuBase.h>

class JXStringHistoryMenu : public JXHistoryMenuBase
{
public:

	JXStringHistoryMenu(const JSize historyLength,
						const JCharacter* title, JXContainer* enclosure,
						const HSizingOption hSizing, const VSizingOption vSizing,
						const JCoordinate x, const JCoordinate y,
						const JCoordinate w, const JCoordinate h);

	JXStringHistoryMenu(const JSize historyLength,
						JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	virtual ~JXStringHistoryMenu();

	void	AddString(const JCharacter* str);

private:

	// not allowed

	JXStringHistoryMenu(const JXStringHistoryMenu& source);
	const JXStringHistoryMenu& operator=(const JXStringHistoryMenu& source);
};

#endif
