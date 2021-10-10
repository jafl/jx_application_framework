/******************************************************************************
 ScrollingWidgetDir.h

	Interface for the ScrollingWidgetDir class

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#ifndef _H_ScrollingWidgetDir
#define _H_ScrollingWidgetDir

#include <JXWindowDirector.h>

class ScrollingWidgetDir : public JXWindowDirector
{
public:

	ScrollingWidgetDir(JXDirector* supervisor);

private:

	void BuildWindow();
};

#endif
