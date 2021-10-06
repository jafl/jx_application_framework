/******************************************************************************
 WidgetDir.h

	Interface for the WidgetDir class

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#ifndef _H_WidgetDir
#define _H_WidgetDir

#include <JXWindowDirector.h>

class WidgetDir : public JXWindowDirector
{
public:

	WidgetDir(JXDirector* supervisor);

	virtual ~WidgetDir();

private:

	void BuildWindow();
};

#endif
