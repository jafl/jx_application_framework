/******************************************************************************
 TestDNDTextDirector.h

	Interface for the TestDNDTextDirector class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestDNDTextDirector
#define _H_TestDNDTextDirector

#include <jx-af/jx/JXWindowDirector.h>

class JXHorizPartition;

class TestDNDTextDirector : public JXWindowDirector
{
public:

	TestDNDTextDirector(JXDirector* supervisor);

	~TestDNDTextDirector() override;

private:

	void	BuildWindow();
};

#endif
