/******************************************************************************
 TestStyler.h

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestStyler
#define _H_TestStyler

#include <JSTStyler.h>

class TestStyler : public JSTStyler
{
public:

	TestStyler();

	virtual ~TestStyler();

protected:

	virtual void	Scan(const JStyledText::TextIndex& startIndex,
						 std::istream& input, const TokenExtra& initData) override;

private:

	// not allowed

	TestStyler(const TestStyler& source);
	const TestStyler& operator=(const TestStyler& source);
};

#endif
