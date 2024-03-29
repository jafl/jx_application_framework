/******************************************************************************
 DragWidgetDir.h

	Interface for the DragWidgetDir class

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#ifndef _H_DragWidgetDir
#define _H_DragWidgetDir

#include <jx-af/jx/JXWindowDirector.h>

class DragWidgetDir : public JXWindowDirector
{
public:

	DragWidgetDir(JXDirector* supervisor);

private:

	void BuildWindow();
};

#endif
