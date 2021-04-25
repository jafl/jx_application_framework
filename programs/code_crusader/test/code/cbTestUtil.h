/******************************************************************************
 cbTestUtil.h

	Test bash styler.

	Written by John Lindal.

 *****************************************************************************/

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
		&recalcRange, &redrawRange, kJFalse, &tokenStartList);
}
