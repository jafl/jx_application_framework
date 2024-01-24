/******************************************************************************
 JXPathHistoryMenu.h

	Interface for the JXPathHistoryMenu class

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXPathHistoryMenu
#define _H_JXPathHistoryMenu

#include "JXStringHistoryMenu.h"
#include <jx-af/jcore/JString.h>

class JXPathHistoryMenu : public JXStringHistoryMenu
{
public:

	JXPathHistoryMenu(const JSize historyLength, JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	JXPathHistoryMenu(const JSize historyLength,
					  JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	~JXPathHistoryMenu() override;

	const JString&	GetBasePath() const;
	void			SetBasePath(const JString& path);
	void			ClearBasePath();

protected:

	void	UpdateMenu() override;		// must call inherited

private:

	JString	itsBasePath;

private:

	void	JXPathHistoryMenuX();
	void	RemoveInvalidPaths();
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
