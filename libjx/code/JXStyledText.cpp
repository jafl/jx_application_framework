/******************************************************************************
 JXStyledText.cpp

	Implements optional font substitution.  It is only appropriate for
	text displayed on a single Display.

	BASE CLASS = JStyledText

	Copyright (C) 2018 by John Lindal.

 ******************************************************************************/

#include <JXStyledText.h>
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
 NeedsToFilterText (virtual protected)

 ******************************************************************************/

JBoolean
JXStyledText::NeedsToFilterText
	(
	const JString&			text,
	const JRunArray<JFont>&	style
	)
	const
{
	if (itsFontManager != NULL)
		{
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
		}

	return kJFalse;
}

/******************************************************************************
 FilterText (virtual protected)

	Font substitution to hopefully make all characters render.

 ******************************************************************************/

#include <JStopWatch.h>

JBoolean
JXStyledText::FilterText
	(
	JString*			text,
	JRunArray<JFont>*	style
	)
{
	if (itsFontManager != NULL)
		{
JStopWatch w;
w.StartTimer();

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
				}
			}

w.StopTimer();
std::cout << w.PrintTimeInterval() << std::endl;
		}

	return kJTrue;
}
