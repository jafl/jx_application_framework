/******************************************************************************
 TestDNDTextDirector.h

	Interface for the TestDNDTextDirector class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestDNDTextDirector
#define _H_TestDNDTextDirector

#include <JXWindowDirector.h>

class JXHorizPartition;

class TestDNDTextDirector : public JXWindowDirector
{
public:

	TestDNDTextDirector(JXDirector* supervisor);

	virtual ~TestDNDTextDirector();

private:

	void	BuildWindow();

	// not allowed

	TestDNDTextDirector(const TestDNDTextDirector& source);
	const TestDNDTextDirector& operator=(const TestDNDTextDirector& source);
};

#endif
