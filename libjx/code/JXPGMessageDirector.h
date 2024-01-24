/******************************************************************************
 JXPGMessageDirector.h

	Copyright (C) 1996 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_JXPGMessageDirector
#define _H_JXPGMessageDirector

#include "JXWindowDirector.h"

class JXTextButton;
class JXStaticText;
class JXPTPrinter;

class JXPGMessageDirector : public JXWindowDirector
{
public:

	JXPGMessageDirector(JXDirector* supervisor);

	~JXPGMessageDirector() override;

	void	AddMessageLine(const JString& text);
	void	AddMessageString(const JString& text);
	void	ProcessFinished();

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXStaticText*	itsMessageText;
	JXPTPrinter*	itsPrinter;			// can be nullptr

// begin JXLayout

	JXTextButton* itsPrintButton;
	JXTextButton* itsSaveButton;
	JXTextButton* itsCloseButton;

// end JXLayout

private:

	void	BuildWindow();
	void	SaveMessages();
};

#endif
