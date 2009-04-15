/******************************************************************************
 PrintWidgetDir.h

	Interface for the PrintWidgetDir class

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#ifndef _H_PrintWidgetDir
#define _H_PrintWidgetDir

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWindowDirector.h>

class JXPSPrinter;
class PrintWidget;
class JXTextButton;

class PrintWidgetDir : public JXWindowDirector
{
public:

	PrintWidgetDir(JXDirector* supervisor);

	virtual ~PrintWidgetDir();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JXPSPrinter*	itsPrinter;
	PrintWidget*	itsWidget;
	JXTextButton*	itsPrintButton;

private:

	void BuildWindow();

	// not allowed

	PrintWidgetDir(const PrintWidgetDir& source);
	const PrintWidgetDir& operator=(const PrintWidgetDir& source);
};

#endif
