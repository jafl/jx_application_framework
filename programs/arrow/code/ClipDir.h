/******************************************************************************
 ClipDir.h

	Interface for the ClipDir class

	Copyright © 1997 by Glenn Bach.
	This code may be freely distributed, used, and modified without restriction.

 ******************************************************************************/

#ifndef _H_ClipDir
#define _H_ClipDir

#include <JXWindowDirector.h>

class JXStaticText;

class ClipDir : public JXWindowDirector
{
public:

	ClipDir(JXDirector* supervisor);

	virtual ~ClipDir();

private:

	// not allowed

	ClipDir(const ClipDir& source);
	const ClipDir& operator=(const ClipDir& source);
};

#endif
