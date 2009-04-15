/******************************************************************************
 JXPGMessageDirector.h

	Interface for the JXPGMessageDirector class

	Copyright © 1996 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXPGMessageDirector
#define _H_JXPGMessageDirector

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWindowDirector.h>

class JXTextButton;
class JXStaticText;
class JXPTPrinter;

class JXPGMessageDirector : public JXWindowDirector
{
public:

	JXPGMessageDirector(JXDirector* supervisor);

	virtual ~JXPGMessageDirector();

	void	AddMessageLine(const JCharacter* text);
	void	AddMessageString(const JCharacter* text);
	void	ProcessFinished();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JXStaticText*	itsMessageText;
	JXPTPrinter*	itsPrinter;			// can be NULL

// begin JXLayout

    JXTextButton* itsSaveButton;
    JXTextButton* itsCloseButton;
    JXTextButton* itsPrintButton;

// end JXLayout

private:

	void	BuildWindow();
	void	SaveMessages();

	// not allowed

	JXPGMessageDirector(const JXPGMessageDirector& source);
	const JXPGMessageDirector& operator=(const JXPGMessageDirector& source);
};

#endif
