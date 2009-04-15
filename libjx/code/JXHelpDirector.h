/******************************************************************************
 JXHelpDirector.h

	Interface for the JXHelpDirector class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXHelpDirector
#define _H_JXHelpDirector

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWindowDirector.h>
#include <JXHelpManager.h>		// need definition of Format

class JXTextButton;
class JXImageButton;
class JXTextMenu;
class JXHelpText;
class JXPSPrinter;

class JXHelpDirector : public JXWindowDirector
{
public:

	JXHelpDirector(const JCharacter* text, JXPSPrinter* printer);

	virtual ~JXHelpDirector();

	void		ShowSubsection(const JCharacter* name);
	JBoolean	Search() const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JXHelpText*	itsText;
	JXTextMenu*	itsFileMenu;
	JIndex		itsSearchAllCmdIndex;	// index of "Search all sections" on Search menu
	JXTextMenu*	itsPrefsMenu;

// begin JXLayout

    JXTextButton*  itsCloseButton;
    JXImageButton* itsTOCButton;

// end JXLayout

private:

	void	BuildWindow(const JCharacter* text, JXPSPrinter* printer);

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	UpdateCustomSearchMenuItems();
	void	HandleCustomSearchMenuItems(const JIndex index);

	void	UpdatePrefsMenu();
	void	HandlePrefsMenu(const JIndex index);

	// not allowed

	JXHelpDirector(const JXHelpDirector& source);
	const JXHelpDirector& operator=(const JXHelpDirector& source);
};

#endif
