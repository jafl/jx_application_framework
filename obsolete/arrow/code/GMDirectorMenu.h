/******************************************************************************
 GMDirectorMenu.h

	Interface for the GMDirectorMenu class

	Copyright (C) 1999 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#ifndef _H_GMDirectorMenu
#define _H_GMDirectorMenu


#include <JXTextMenu.h>



class GMDirectorMenu : public JXTextMenu
{
public:

	GMDirectorMenu(const JCharacter* title, JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);
	virtual ~GMDirectorMenu();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

private:

	// not allowed

	GMDirectorMenu(const GMDirectorMenu& source);
	const GMDirectorMenu& operator=(const GMDirectorMenu& source);

public:
};

#endif
