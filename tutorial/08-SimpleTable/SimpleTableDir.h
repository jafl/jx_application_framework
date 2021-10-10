/******************************************************************************
 SimpleTableDir.h

	Interface for the SimpleTableDir class

	Written by Glenn Bach - 1998.

 ******************************************************************************/

#ifndef _H_SimpleTableDir
#define _H_SimpleTableDir

#include <JXWindowDirector.h>

class SimpleTableDir : public JXWindowDirector
{
public:

	SimpleTableDir(JXDirector* supervisor);

	~SimpleTableDir() override;

private:

	void BuildWindow();
};

#endif
