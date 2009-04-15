/******************************************************************************
 JXStringCompletionMenu.h

	Interface for the JXStringCompletionMenu class

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXStringCompletionMenu
#define _H_JXStringCompletionMenu

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXTextMenu.h>

class JXTEBase;

class JXStringCompletionMenu : public JXTextMenu
{
public:

	JXStringCompletionMenu(JXTEBase* te, const JBoolean allowTabChar);

	virtual ~JXStringCompletionMenu();

	JBoolean	AddString(const JCharacter* str);

	void	CompletionRequested(const JSize prefixLength);
	void	ClearRequestCount();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JXTEBase*		itsTE;				// not owned
	const JBoolean	itsAllowTabChar;
	JSize			itsRequestCount;
	JSize			itsPrefixLength;

private:

	void	HandleSelection(const JIndex index);

	// not allowed

	JXStringCompletionMenu(const JXStringCompletionMenu& source);
	const JXStringCompletionMenu& operator=(const JXStringCompletionMenu& source);
};


/******************************************************************************
 ClearRequestCount

 ******************************************************************************/

inline void
JXStringCompletionMenu::ClearRequestCount()
{
	itsRequestCount = 0;
}

#endif
