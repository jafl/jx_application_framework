/******************************************************************************
 test_HTMLStyler.cpp

	Test html/php/jsp/hbs styler.

	Written by John Lindal.

 *****************************************************************************/

#include <JTestManager.h>
#include "CBHTMLStyler.h"
#include "cbTestUtil.h"
#include <JColorManager.h>
#include <jAssert.h>

class TestHTMLStyler : public CBHTMLStyler
{
public:

	TestHTMLStyler() {};

	void TestSetWordStyle(const JString& word, const JFontStyle& style);
};

void
TestHTMLStyler::TestSetWordStyle
	(
	const JString&		word,
	const JFontStyle&	style
	)
{
	SetWordStyle(word, style);
}

int main()
{
	return JTestManager::Execute();
}

JTEST(html)
{
	JStyledText* st = jnew JStyledText(kJFalse, kJFalse);
	assert( st != nullptr );

	TestHTMLStyler styler;

	// empty
	UpdateStyles(st, &styler);

	RunTest(st, &styler, "./data/styler/html/test.html", "./data/styler/html/styled-html.out");
}

JTEST(hbs)
{
	JStyledText* st = jnew JStyledText(kJFalse, kJFalse);
	assert( st != nullptr );

	TestHTMLStyler styler;
	RunTest(st, &styler, "./data/styler/html/test.hbs", "./data/styler/html/styled-hbs.out");
}

JTEST(php)
{
	JStyledText* st = jnew JStyledText(kJFalse, kJFalse);
	assert( st != nullptr );

	TestHTMLStyler styler;
	RunTest(st, &styler, "./data/styler/html/test.php", "./data/styler/html/styled-php.out");
}

JTEST(jsp)
{
	JStyledText* st = jnew JStyledText(kJFalse, kJFalse);
	assert( st != nullptr );

	TestHTMLStyler styler;
	styler.TestSetWordStyle(JString("/form", kJFalse), JFontStyle(kJTrue, kJFalse, 0, kJFalse, JColorManager::GetBlueColor()));
	styler.TestSetWordStyle(JString("set", kJFalse), JFontStyle(JColorManager::GetOrangeColor()));
	styler.TestSetWordStyle(JString("x:choose", kJFalse), JFontStyle(kJTrue, kJFalse, 0, kJFalse));

	RunTest(st, &styler, "./data/styler/html/test.jsp", "./data/styler/html/styled-jsp.out");
}
