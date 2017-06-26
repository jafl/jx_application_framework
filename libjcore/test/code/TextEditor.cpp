/******************************************************************************
 TextEditor.cpp

	Class to test JTextEditor.

	BASE CLASS = JTextEditor

	Written by John Lindal.

 ******************************************************************************/

#include "TextEditor.h"
#include "TEFontManager.h"
#include "TEColormap.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TextEditor::TextEditor()
	:
	JTextEditor(kFullEditor, kJTrue, kJFalse,
				jnew TEFontManager,
				jnew TEColormap,
				1,1,1,1,1, 500)
{
	assert( TEGetFontManager() != NULL );
	assert( TEGetColormap() != NULL );

	RecalcAll(kJTrue);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TextEditor::~TextEditor()
{
}

/******************************************************************************
 TERefresh (virtual protected)

 ******************************************************************************/

void
TextEditor::TERefresh()
{
}

/******************************************************************************
 TERefreshRect (virtual protected)

 ******************************************************************************/

void
TextEditor::TERefreshRect
	(
	const JRect& rect
	)
{
}

/******************************************************************************
 TERedraw (virtual protected)

 ******************************************************************************/

void
TextEditor::TERedraw()
{
}

/******************************************************************************
 TESetGUIBounds (virtual protected)

 ******************************************************************************/

void
TextEditor::TESetGUIBounds
	(
	const JCoordinate w,
	const JCoordinate h,
	const JCoordinate changeY
	)
{
}

/******************************************************************************
 TEWidthIsBeyondDisplayCapacity (virtual protected)

 ******************************************************************************/

JBoolean
TextEditor::TEWidthIsBeyondDisplayCapacity
	(
	const JSize width
	)
	const
{
	return kJFalse;
}

/******************************************************************************
 TEScrollToRect (virtual protected)

 ******************************************************************************/

JBoolean
TextEditor::TEScrollToRect
	(
	const JRect&	rect,
	const JBoolean	centerInDisplay
	)
{
	return kJTrue;
}

/******************************************************************************
 TEScrollForDrag (virtual protected)

 ******************************************************************************/

JBoolean
TextEditor::TEScrollForDrag
	(
	const JPoint& pt
	)
{
	return kJTrue;
}

/******************************************************************************
 TEScrollForDND (virtual protected)

 ******************************************************************************/

JBoolean
TextEditor::TEScrollForDND
	(
	const JPoint& pt
	)
{
	return kJTrue;
}

/******************************************************************************
 TESetVertScrollStep (virtual protected)

 ******************************************************************************/

void
TextEditor::TESetVertScrollStep
	(
	const JCoordinate vStep
	)
{
}

/******************************************************************************
 TEClipboardChanged (virtual protected)

 ******************************************************************************/

void
TextEditor::TEClipboardChanged()
{
}

/******************************************************************************
 TEGetExternalClipboard (virtual protected)

 ******************************************************************************/

JBoolean
TextEditor::TEGetExternalClipboard
	(
	JString*			text,
	JRunArray<JFont>*	style
	)
	const
{
	return kJFalse;
}

/******************************************************************************
 TEDisplayBusyCursor (virtual protected)

 ******************************************************************************/

void
TextEditor::TEDisplayBusyCursor()
	const
{
}

/******************************************************************************
 TEBeginDND (virtual protected)

 ******************************************************************************/

JBoolean
TextEditor::TEBeginDND()
{
	return kJFalse;
}

/******************************************************************************
 TEPasteDropData (virtual protected)

 ******************************************************************************/

void
TextEditor::TEPasteDropData()
{
}

/******************************************************************************
 TECaretShouldBlink (virtual protected)

 ******************************************************************************/

void
TextEditor::TECaretShouldBlink
	(
	const JBoolean blink
	)
{
}

/******************************************************************************
 TEHasSearchText (virtual)

 ******************************************************************************/

JBoolean
TextEditor::TEHasSearchText()
	const
{
	return kJFalse;
}
