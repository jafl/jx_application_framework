/******************************************************************************
 CBSearchPathHistoryMenu.h

	Copyright © 2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBSearchPathHistoryMenu
#define _H_CBSearchPathHistoryMenu

#include <JXHistoryMenuBase.h>

class CBSearchPathHistoryMenu : public JXHistoryMenuBase
{
public:

	CBSearchPathHistoryMenu(const JSize historyLength,
							const JString& title, JXContainer* enclosure,
							const HSizingOption hSizing, const VSizingOption vSizing,
							const JCoordinate x, const JCoordinate y,
							const JCoordinate w, const JCoordinate h);

	CBSearchPathHistoryMenu(const JSize historyLength,
							JXMenu* owner, const JIndex itemIndex,
							JXContainer* enclosure);

	virtual ~CBSearchPathHistoryMenu();

	void	AddPath(const JString& fullName, const bool recurse);

	const JString&	GetPath(const Message& message, bool* recurse) const;
	const JString&	GetPath(const JIndex index, bool* recurse) const;

protected:

	virtual void	UpdateMenu();		// must call inherited

private:

	void	CBSearchPathHistoryMenuX();
	void	RemoveInvalidPaths();

	// not allowed

	CBSearchPathHistoryMenu(const CBSearchPathHistoryMenu& source);
	const CBSearchPathHistoryMenu& operator=(const CBSearchPathHistoryMenu& source);
};

#endif
