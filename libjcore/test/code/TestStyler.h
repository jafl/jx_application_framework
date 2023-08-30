/******************************************************************************
 TestStyler.h

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestStyler
#define _H_TestStyler

#include "JSTStyler.h"

class TestStyler : public JSTStyler
{
public:

	TestStyler();

	~TestStyler() override;

protected:

	void	Scan(const JStyledText::TextIndex& startIndex,
				 std::istream& input, const TokenExtra& initData) override;
};

#endif
