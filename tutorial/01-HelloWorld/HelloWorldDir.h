/******************************************************************************
 HelloWorldDir.h

	Interface for the HelloWorldDir class

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#ifndef _H_HelloWorldDir
#define _H_HelloWorldDir

#include <JXWindowDirector.h>

class HelloWorldDir : public JXWindowDirector
{
public:

	HelloWorldDir(JXDirector* supervisor);

	virtual ~HelloWorldDir();
};

#endif
