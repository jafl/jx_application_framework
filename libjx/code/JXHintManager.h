/******************************************************************************
 JXHintManager.h

	Copyright © 1999 by John Lindal.  All rights reserved.

 ******************************************************************************/

#ifndef _H_JXHintManager
#define _H_JXHintManager

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDirector.h>
#include <JString.h>
#include <JPoint.h>
#include <X11/X.h>

class JXContainer;
class JXDirector;
class JXHintDirector;

class JXHintManager
{
public:

	JXHintManager(JXContainer* widget, const JCharacter* text);

	~JXHintManager();

	JBoolean	IsActive() const;
	void		Activate();
	void		Deactivate();

	void	HandleMouseEnter();
	void	HandleMouseHere(const JPoint& pt);
	void	HandleMouseHere(const JPoint& pt, const JRect& rect);
	void	HandleMouseLeave();

	const JString&	GetText() const;
	void			SetText(const JCharacter* text);

	Time	GetStartTime() const;
	void	SetStartTime(const Time t);

	void	CreateHintWindow();
	void	CreateHintWindow(const JRect& rect);
	void	DestroyWindow();

private:

	JXContainer*	itsWidget;		// not owned
	JString			itsText;
	JBoolean		itsActiveFlag;	// kJTrue if should show hint
	Time			itsStartTime;
	JPoint			itsPrevPt;
	JXDirector*		itsDirOwner;	// can be NULL
	JXHintDirector*	itsDirector;	// can be NULL

private:

	// not allowed

	JXHintManager(const JXHintManager& source);
	const JXHintManager& operator=(const JXHintManager& source);
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
	const JCharacter* text
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

inline JBoolean
JXHintManager::IsActive()
	const
{
	return itsActiveFlag;
}

inline void
JXHintManager::Activate()
{
	itsActiveFlag = kJTrue;
	HandleMouseEnter();
}

inline void
JXHintManager::Deactivate()
{
	itsActiveFlag = kJFalse;
	DestroyWindow();
}

#endif
