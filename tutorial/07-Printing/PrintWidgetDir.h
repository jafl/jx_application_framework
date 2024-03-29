/******************************************************************************
 PrintWidgetDir.h

	Interface for the PrintWidgetDir class

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#ifndef _H_PrintWidgetDir
#define _H_PrintWidgetDir

#include <jx-af/jx/JXWindowDirector.h>

class JXPSPrinter;
class PrintWidget;
class JXTextButton;

class PrintWidgetDir : public JXWindowDirector
{
public:

	PrintWidgetDir(JXDirector* supervisor);

	~PrintWidgetDir() override;

private:

	JXPSPrinter*	itsPrinter;
	PrintWidget*	itsWidget;
	JXTextButton*	itsPrintButton;

private:

	void BuildWindow();
};

#endif
