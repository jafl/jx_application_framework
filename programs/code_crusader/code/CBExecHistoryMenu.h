/******************************************************************************
 CBExecHistoryMenu.h

	Interface for the CBExecHistoryMenu class

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBExecHistoryMenu
#define _H_CBExecHistoryMenu

#include <JXStringHistoryMenu.h>

class CBExecHistoryMenu : public JXStringHistoryMenu
{
public:

	CBExecHistoryMenu(const JSize historyLength,
					  const JString& title, JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	CBExecHistoryMenu(const JSize historyLength,
					  JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	virtual ~CBExecHistoryMenu();

private:

	void	CBExecHistoryMenuX();

	// not allowed

	CBExecHistoryMenu(const CBExecHistoryMenu& source);
	const CBExecHistoryMenu& operator=(const CBExecHistoryMenu& source);
};

#endif
