/******************************************************************************
 DNDWidgetDir.h

	Interface for the DNDWidgetDir class

	Written by Glenn Bach - 2000.

 ******************************************************************************/

#ifndef _H_DNDWidgetDir
#define _H_DNDWidgetDir

#include <jx-af/jx/JXWindowDirector.h>

class DNDWidgetDir : public JXWindowDirector
{
public:

	DNDWidgetDir(JXDirector* supervisor);

private:

	void BuildWindow();
};

#endif
