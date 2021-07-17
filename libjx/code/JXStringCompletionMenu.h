/******************************************************************************
 JXStringCompletionMenu.h

	Interface for the JXStringCompletionMenu class

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXStringCompletionMenu
#define _H_JXStringCompletionMenu

#include "JXTextMenu.h"

class JXTEBase;

class JXStringCompletionMenu : public JXTextMenu
{
public:

	JXStringCompletionMenu(JXTEBase* te, const bool allowTabChar);

	virtual ~JXStringCompletionMenu();

	bool	AddString(const JString& str);

	void	CompletionRequested(const JSize prefixCharCount);
	void	ClearRequestCount();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXTEBase*		itsTE;				// not owned
	const bool	itsAllowTabChar;
	JSize			itsRequestCount;
	JSize			itsPrefixCharCount;

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
