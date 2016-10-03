/******************************************************************************
 GNetDebugDir.h

	Interface for the GNetDebugDir class

	Copyright (C) 1997 by Glenn Bach.
	This code may be freely distributed, used, and modified without restriction.

 ******************************************************************************/

#ifndef _H_GNetDebugDir
#define _H_GNetDebugDir

#include <GMManagedDirector.h>

class JXStaticText;
class JXTextMenu;
class JXImage;

class GNetDebugDir : public GMManagedDirector
{
public:

	GNetDebugDir(JXDirector* supervisor, const JCharacter* title);

	virtual ~GNetDebugDir();

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

	GNetDebugDir(const GNetDebugDir& source);
	const GNetDebugDir& operator=(const GNetDebugDir& source);
};

/******************************************************************************
 GetMenuIcon (public)

 ******************************************************************************/

inline JXImage*
GNetDebugDir::GetMenuIcon()
	const
{
	return itsMenuIcon;
}



#endif
