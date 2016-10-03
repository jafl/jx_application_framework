/******************************************************************************
 SMTPDebugDir.h

	Interface for the SMTPDebugDir class

	Copyright (C) 1997 by Glenn Bach.
	This code may be freely distributed, used, and modified without restriction.

 ******************************************************************************/

#ifndef _H_SMTPDebugDir
#define _H_SMTPDebugDir

#include <GMManagedDirector.h>

class JXStaticText;
class JXTextMenu;
class JXImage;

class SMTPDebugDir : public GMManagedDirector
{
public:

	SMTPDebugDir(JXDirector* supervisor);

	virtual ~SMTPDebugDir();

	void	AddText(const JCharacter* text);

	virtual JXImage*	GetMenuIcon() const;

protected:

	virtual void		Receive(JBroadcaster* sender,
								const JBroadcaster::Message& message);
private:

	JXStaticText*	itsText;
	JXTextMenu*		itsFileMenu;
	JXImage*		itsMenuIcon;

private:

	void UpdateFileMenu();
	void HandleFileMenu(const JIndex index);

	// not allowed

	SMTPDebugDir(const SMTPDebugDir& source);
	const SMTPDebugDir& operator=(const SMTPDebugDir& source);
};

/******************************************************************************
 GetMenuIcon (public)

 ******************************************************************************/

inline JXImage*
SMTPDebugDir::GetMenuIcon()
	const
{
	return itsMenuIcon;
}



#endif
