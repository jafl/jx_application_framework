/******************************************************************************
 TestStyler.cpp

	Class to test JSTStyler.

	BASE CLASS = JSTStyler

	Written by John Lindal.

 ******************************************************************************/

#include "TestStyler.h"
#include <JColorManager.h>
#include <jStreamUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TestStyler::TestStyler()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TestStyler::~TestStyler()
{
}

/******************************************************************************
 Scan (virtual protected)

 ******************************************************************************/

void
TestStyler::Scan
	(
	std::istream&		input,
	const TokenExtra&	initData
	)
{
	JCharacterRange r;
	r.first = JTellg(input)+1;

	JString token;
	while (1)
		{
		JBoolean foundWS;
		token  = JReadUntilws(input, &foundWS);
		r.last = r.first + token.GetCharacterCount() - 1;

		JFontStyle style;
		if (token == "yes")
			{
			style.color = JColorManager::GetGreenColor();
			}
		else if (token == "no")
			{
			style.bold = kJTrue;
			style.color = JColorManager::GetRedColor();
			}
		else if (token == "zap")
			{
			style.strike = kJTrue;
			}

		if (!SetStyle(r, style) || !foundWS)
			{
			break;
			}

		r.first = r.last + 1;	// whitespace token -- ASSUMES only one!
		r.last  = r.first;
		SetStyle(r, JFontStyle());

		r.first++;
		}
}
