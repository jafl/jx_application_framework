/******************************************************************************
 Test3DDirector.h

	Interface for the Test3DDirector class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_Test3DDirector
#define _H_Test3DDirector

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWindowDirector.h>

class Test3DWidget;
class J3DUniverse;

class Test3DDirector : public JXWindowDirector
{
public:

	Test3DDirector(JXDirector* supervisor);

	virtual ~Test3DDirector();

private:

	J3DUniverse*	itsUniverse;
	Test3DWidget*	its3DWidget1;
	Test3DWidget*	its3DWidget2;

private:

	void	BuildWindow();

	// not allowed

	Test3DDirector(const Test3DDirector& source);
	const Test3DDirector& operator=(const Test3DDirector& source);
};

#endif
