/******************************************************************************
 TextEditor.cpp

	Class to test JTextEditor.

	BASE CLASS = JTextEditor

	Written by John Lindal.

 ******************************************************************************/

#include "TextEditor.h"
#include "TestFontManager.h"
#include "TestPainter.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TextEditor::TextEditor
	(
	JStyledText*		text,
	const JBoolean		breakCROnly,
	const JCoordinate	width
	)
	:
	JTextEditor(kFullEditor, text, kJFalse,
				jnew TestFontManager, breakCROnly,
				1,1,1,1, width),
	itsWidth(0),
	itsHeight(0),
	itsClipText(NULL),
	itsClipStyle(NULL)
{
	RecalcAll();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TextEditor::~TextEditor()
{
	jdelete itsClipText;
	jdelete itsClipStyle;
}

/******************************************************************************
 SetBoundsWidth

 ******************************************************************************/

void
TextEditor::SetBoundsWidth
	(
	const JCoordinate width
	)
{
	TESetBoundsWidth(width);
}

/******************************************************************************
 Draw

 ******************************************************************************/

void
TextEditor::Draw()
{
	TestPainter p;
	TEDraw(p, JRect(0,0,1000000,1000000));
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
	itsWidth  = w;
	itsHeight = h;
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
TextEditor::TEUpdateClipboard
	(
	const JString&			text,
	const JRunArray<JFont>&	style
	)
	const
{
	if (itsClipText == NULL)
		{
		itsClipText = jnew JString(text);
		assert( itsClipText != NULL );

		itsClipStyle = jnew JRunArray<JFont>(style);
		assert( itsClipStyle != NULL );
		}
	else
		{
		*itsClipText  = text;
		*itsClipStyle = style;
		}
}

/******************************************************************************
 TEGetExternalClipboard (virtual protected)

 ******************************************************************************/

JBoolean
TextEditor::TEGetClipboard
	(
	JString*			text,
	JRunArray<JFont>*	style
	)
	const
{
	if (itsClipText == NULL)
		{
		return kJFalse;
		}

	text->Set(*itsClipText);
	*style = *itsClipStyle;
	return kJTrue;
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
