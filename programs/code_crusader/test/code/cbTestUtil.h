/******************************************************************************
 cbTestUtil.h

	Utility test code.

	Written by John Lindal.

 *****************************************************************************/

#include <jFileUtil.h>
#include <sstream>

inline void
UpdateStyles
	(
	JStyledText*	st,
	CBStylerBase*	styler
	)
{
	JStyledText::TextRange recalcRange(JStyledText::TextIndex(1,1), st->GetBeyondEnd());
	JStyledText::TextRange redrawRange = recalcRange;

	JArray<JSTStyler::TokenData> tokenStartList;

	styler->UpdateStyles(
		st, st->GetText(), const_cast<JRunArray<JFont>*>(&st->GetStyles()),
		&recalcRange, &redrawRange, false, &tokenStartList);
}

inline void
RunTest
	(
	JStyledText*		st,
	CBStylerBase*		styler,
	const JUtf8Byte*	inputFilename,
	const JUtf8Byte*	outputFilename
	)
{
	JString text;
	JReadFile(JString(inputFilename), &text);

	st->SetText(text);
	UpdateStyles(st, styler);

	std::ostringstream result;
	st->WritePrivateFormat(result);

	JReadFile(JString(outputFilename), &text);
	JAssertStringsEqual(text, JString(result.str().c_str(), false));
}
