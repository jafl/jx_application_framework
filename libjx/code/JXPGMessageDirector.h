/******************************************************************************
 JXPGMessageDirector.h

	Copyright (C) 1996 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_JXPGMessageDirector
#define _H_JXPGMessageDirector

#include "jx-af/jx/JXWindowDirector.h"

class JXTextButton;
class JXStaticText;
class JXPTPrinter;

class JXPGMessageDirector : public JXWindowDirector
{
public:

	JXPGMessageDirector(JXDirector* supervisor);

	virtual ~JXPGMessageDirector();

	void	AddMessageLine(const JString& text);
	void	AddMessageString(const JString& text);
	void	ProcessFinished();

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXStaticText*	itsMessageText;
	JXPTPrinter*	itsPrinter;			// can be nullptr

// begin JXLayout

	JXTextButton* itsSaveButton;
	JXTextButton* itsCloseButton;
	JXTextButton* itsPrintButton;

// end JXLayout

private:

	void	BuildWindow();
	void	SaveMessages();
};

#endif
