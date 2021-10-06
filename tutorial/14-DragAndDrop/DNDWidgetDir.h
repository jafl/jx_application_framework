/******************************************************************************
 DNDWidgetDir.h

	Interface for the DNDWidgetDir class

	Written by Glenn Bach - 2000.

 ******************************************************************************/

#ifndef _H_DNDWidgetDir
#define _H_DNDWidgetDir

#include <JXWindowDirector.h>

class DNDWidgetDir : public JXWindowDirector
{
public:

	DNDWidgetDir(JXDirector* supervisor);

	virtual ~DNDWidgetDir();

private:

	void BuildWindow();
};

#endif
