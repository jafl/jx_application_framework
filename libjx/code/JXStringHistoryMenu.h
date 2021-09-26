/******************************************************************************
 JXStringHistoryMenu.h

	Interface for the JXStringHistoryMenu class

	Copyright (C) 1997-98 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXStringHistoryMenu
#define _H_JXStringHistoryMenu

#include "jx-af/jx/JXHistoryMenuBase.h"

class JXStringHistoryMenu : public JXHistoryMenuBase
{
public:

	JXStringHistoryMenu(const JSize historyLength,
						const JString& title, JXContainer* enclosure,
						const HSizingOption hSizing, const VSizingOption vSizing,
						const JCoordinate x, const JCoordinate y,
						const JCoordinate w, const JCoordinate h);

	JXStringHistoryMenu(const JSize historyLength,
						JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	virtual ~JXStringHistoryMenu();

	void	AddString(const JString& str);
};

#endif
