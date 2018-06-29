/******************************************************************************
 JXStyledText.cpp

	Implements optional font substitution.  It is only appropriate for text
	displayed on a single Display, because otherwise, there are multiple
	JFontManager's involved.

	BASE CLASS = JStyledText

	Copyright (C) 2018 by John Lindal.

 ******************************************************************************/

#include "JXStyledText.h"
#include <JStringIterator.h>
#include <JFontManager.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

	Provide a fontManager to enable font substitution.

 ******************************************************************************/

JXStyledText::JXStyledText
	(
	const JBoolean	useMultipleUndo,
	const JBoolean	pasteStyledText,
	JFontManager*	fontManager
	)
	:
	JStyledText(useMultipleUndo, pasteStyledText),
	itsFontManager(fontManager)
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JXStyledText::JXStyledText
	(
	const JXStyledText& source
	)
	:
	JStyledText(source),
	itsFontManager(source.itsFontManager)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXStyledText::~JXStyledText()
{
}

/******************************************************************************
 NeedsToAdjustFontToDisplayGlyphs(virtual protected)

 ******************************************************************************/

JBoolean
JXStyledText::NeedsToAdjustFontToDisplayGlyphs
	(
	const JString&			text,
	const JRunArray<JFont>&	style
	)
	const
{
	if (itsFontManager == nullptr)
		{
		return kJFalse;
		}

	JStringIterator siter(text);
	JRunArrayIterator<JFont> fiter(style);

	JUtf8Character c;
	JFont f;
	while (siter.Next(&c))
		{
		const JBoolean ok = fiter.Next(&f);
		assert( ok );

		if (!f.HasGlyphForCharacter(itsFontManager, c))
			{
			return kJTrue;
			}
		}

	return kJFalse;
}

/******************************************************************************
 AdjustFontToDisplayGlyphs (virtual protected)

	Font substitution to hopefully make all characters render.

 ******************************************************************************/

JBoolean
JXStyledText::AdjustFontToDisplayGlyphs
	(
	const TextRange&	range,
	const JString&		text,
	JRunArray<JFont>*	style
	)
{
	if (itsFontManager == nullptr)
		{
		return kJFalse;
		}

	JBoolean changed = kJFalse;

	JStringIterator siter(text);
	JRunArrayIterator<JFont> fiter(style);

	JUtf8Character c;
	JFont f;
	while (siter.Next(&c))
		{
		const JBoolean ok = fiter.Next(&f);
		assert( ok );

		if (f.SubstituteToDisplayGlyph(itsFontManager, c))
			{
			fiter.SetPrev(f);
			changed = kJTrue;
			}
		}

	return changed;
}
