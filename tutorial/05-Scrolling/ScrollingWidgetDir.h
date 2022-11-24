/******************************************************************************
 ScrollingWidgetDir.h

	Interface for the ScrollingWidgetDir class

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#ifndef _H_ScrollingWidgetDir
#define _H_ScrollingWidgetDir

#include <jx-af/jx/JXWindowDirector.h>

class ScrollingWidgetDir : public JXWindowDirector
{
public:

	ScrollingWidgetDir(JXDirector* supervisor);

private:

	void BuildWindow();
};

#endif
