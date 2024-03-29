/******************************************************************************
 JXStringHistoryMenu.h

	Interface for the JXStringHistoryMenu class

	Copyright (C) 1997-98 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXStringHistoryMenu
#define _H_JXStringHistoryMenu

#include "JXHistoryMenuBase.h"

class JXStringHistoryMenu : public JXHistoryMenuBase
{
public:

	JXStringHistoryMenu(const JSize historyLength, JXContainer* enclosure,
						const HSizingOption hSizing, const VSizingOption vSizing,
						const JCoordinate x, const JCoordinate y,
						const JCoordinate w, const JCoordinate h);

	JXStringHistoryMenu(const JSize historyLength,
						JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	~JXStringHistoryMenu() override;

	void	AddString(const JString& str);
};

#endif
