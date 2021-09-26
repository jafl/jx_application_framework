/******************************************************************************
 JXHintManager.h

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXHintManager
#define _H_JXHintManager

#include "jx-af/jx/JXDirector.h"
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/JPoint.h>
#include <X11/X.h>

class JXContainer;
class JXDirector;
class JXHintDirector;

class JXHintManager
{
public:

	JXHintManager(JXContainer* widget, const JString& text);

	~JXHintManager();

	bool	IsActive() const;
	void	Activate();
	void	Deactivate();

	void	HandleMouseEnter();
	void	HandleMouseHere(const JPoint& pt);
	void	HandleMouseHere(const JPoint& pt, const JRect& rect);
	void	HandleMouseLeave();

	const JString&	GetText() const;
	void			SetText(const JString& text);

	Time	GetStartTime() const;
	void	SetStartTime(const Time t);

	void	CreateHintWindow();
	void	CreateHintWindow(const JRect& rect);
	void	DestroyWindow();

private:

	JXContainer*	itsWidget;		// not owned
	JString			itsText;
	bool			itsActiveFlag;	// true if should show hint
	Time			itsStartTime;
	JPoint			itsPrevPt;
	JXDirector*		itsDirOwner;	// can be nullptr
	JXHintDirector*	itsDirector;	// can be nullptr

private:

	// not allowed

	JXHintManager(const JXHintManager&) = delete;
	JXHintManager& operator=(const JXHintManager&) = delete;
};


/******************************************************************************
 Text

 ******************************************************************************/

inline const JString&
JXHintManager::GetText()
	const
{
	return itsText;
}

inline void
JXHintManager::SetText
	(
	const JString& text
	)
{
	itsText = text;
}

/******************************************************************************
 Start time

 ******************************************************************************/

inline Time
JXHintManager::GetStartTime()
	const
{
	return itsStartTime;
}

inline void
JXHintManager::SetStartTime
	(
	const Time t
	)
{
	itsStartTime = t;
}

/******************************************************************************
 Activate

 ******************************************************************************/

inline bool
JXHintManager::IsActive()
	const
{
	return itsActiveFlag;
}

inline void
JXHintManager::Activate()
{
	itsActiveFlag = true;
	HandleMouseEnter();
}

inline void
JXHintManager::Deactivate()
{
	itsActiveFlag = false;
	DestroyWindow();
}

#endif
