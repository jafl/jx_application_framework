/******************************************************************************
 SimpleTableDir.h

	Interface for the SimpleTableDir class

	Written by Glenn Bach - 1998.

 ******************************************************************************/

#ifndef _H_SimpleTableDir
#define _H_SimpleTableDir

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWindowDirector.h>

class SimpleTableDir : public JXWindowDirector
{
public:

	SimpleTableDir(JXDirector* supervisor);

	virtual ~SimpleTableDir();

private:

	void BuildWindow();

	// not allowed

	SimpleTableDir(const SimpleTableDir& source);
	const SimpleTableDir& operator=(const SimpleTableDir& source);
};

#endif
