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

private:

	// not allowed

	HelloWorldDir(const HelloWorldDir& source);
	const HelloWorldDir& operator=(const HelloWorldDir& source);
};

#endif
