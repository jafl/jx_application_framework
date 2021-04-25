/******************************************************************************
 CBSearchFilterHistoryMenu.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBSearchFilterHistoryMenu
#define _H_CBSearchFilterHistoryMenu

#include <JXHistoryMenuBase.h>

class CBSearchFilterHistoryMenu : public JXHistoryMenuBase
{
public:

	CBSearchFilterHistoryMenu(const JSize historyLength,
							  const JString& title, JXContainer* enclosure,
							  const HSizingOption hSizing, const VSizingOption vSizing,
							  const JCoordinate x, const JCoordinate y,
							  const JCoordinate w, const JCoordinate h);

	CBSearchFilterHistoryMenu(const JSize historyLength,
							  JXMenu* owner, const JIndex itemIndex,
							  JXContainer* enclosure);

	virtual ~CBSearchFilterHistoryMenu();

	void	AddFilter(const JString& filter, const JBoolean invert);

	const JString&	GetFilter(const Message& message, JBoolean* invert) const;
	const JString&	GetFilter(const JIndex index, JBoolean* invert) const;

private:

	// not allowed

	CBSearchFilterHistoryMenu(const CBSearchFilterHistoryMenu& source);
	const CBSearchFilterHistoryMenu& operator=(const CBSearchFilterHistoryMenu& source);
};

#endif
