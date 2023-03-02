/******************************************************************************
 JXStyledText.cpp

	Implements optional font substitution.  It is only appropriate for text
	displayed on a single Display, because otherwise, there are multiple
	JFontManager's involved.

	BASE CLASS = JStyledText

	Copyright (C) 2018 by John Lindal.

 ******************************************************************************/

#include "JXStyledText.h"
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

	Provide a fontManager to enable font substitution.

 ******************************************************************************/

JXStyledText::JXStyledText
	(
	const bool	useMultipleUndo,
	const bool	pasteStyledText,
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
 NeedsToAdjustFontToDisplayGlyphs (virtual protected)

 ******************************************************************************/

bool
JXStyledText::NeedsToAdjustFontToDisplayGlyphs
	(
	const JString&			text,
	const JRunArray<JFont>&	style
	)
	const
{
	if (itsFontManager == nullptr)
	{
		return false;
	}

	JStringIterator siter(text);
	JRunArrayIterator fiter(style);

	JUtf8Character c;
	JFont f;
	while (siter.Next(&c))
	{
		const bool ok = fiter.Next(&f);
		assert( ok );

		// on ubuntu, reports false for newline!

		if (!c.IsSpace() && !f.HasGlyphForCharacter(itsFontManager, c))
		{
			return true;
		}
	}

	return false;
}

/******************************************************************************
 AdjustFontToDisplayGlyphs (virtual protected)

	Font substitution to hopefully make all characters render.

 ******************************************************************************/

bool
JXStyledText::AdjustFontToDisplayGlyphs
	(
	const TextRange&	range,
	const JString&		text,
	JRunArray<JFont>*	style
	)
{
	if (itsFontManager == nullptr)
	{
		return false;
	}

	if (range.charRange.GetCount() > 1e4)
	{
		Broadcast(WillBeBusy());
	}

	bool changed = false;

	JStringIterator siter(text);
	JRunArrayIterator<JFont> fiter(style);

	JUtf8Character c;
	JFont f;
	while (siter.Next(&c))
	{
		const bool ok = fiter.Next(&f);
		assert( ok );

		if (f.SubstituteToDisplayGlyph(itsFontManager, c))
		{
			fiter.SetPrev(f, kJIteratorStay);
			changed = true;
		}
	}

	return changed;
}
