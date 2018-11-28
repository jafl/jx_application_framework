/******************************************************************************
 TextEditor.cpp

	Class to test JTextEditor.

	BASE CLASS = JTextEditor

	Written by John Lindal.

 ******************************************************************************/

#include "TextEditor.h"
#include "TestFontManager.h"
#include "TestPainter.h"
#include <JTestManager.h>
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
	itsClipText(nullptr),
	itsClipStyle(nullptr),
	itsHasSearchTextFlag(kJFalse)
{
	ListenTo(this);
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
 TestReplaceSelection

 ******************************************************************************/

void
TextEditor::TestReplaceSelection
	(
	const JStringMatch&	match,
	const JString&		replaceStr,
	JInterpolate*		interpolator,
	const JBoolean		preserveCase
	)
{
	ReplaceSelection(match, replaceStr, interpolator, preserveCase);
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
	if (itsClipText == nullptr)
		{
		itsClipText = jnew JString(text);
		assert( itsClipText != nullptr );

		itsClipStyle = jnew JRunArray<JFont>(style);
		assert( itsClipStyle != nullptr );
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
	if (itsClipText == nullptr)
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
 SetHasSearchText (virtual)

 ******************************************************************************/

void
TextEditor::SetHasSearchText
	(
	const JBoolean has
	)
{
	itsHasSearchTextFlag = has;
}

/******************************************************************************
 TEHasSearchText (virtual)

 ******************************************************************************/

JBoolean
TextEditor::TEHasSearchText()
	const
{
	return itsHasSearchTextFlag;
}

/******************************************************************************
 Activate

 ******************************************************************************/

void
TextEditor::Activate()
{
	TEActivate();
}

/******************************************************************************
 CheckCmdStatus

 ******************************************************************************/

void
TextEditor::CheckCmdStatus
	(
	const JArray<JBoolean>& expected
	)
	const
{
	JString crmActionText, crm2ActionText;
	JBoolean isReadOnly;
	const JArray<JBoolean> status = GetCmdStatus(&crmActionText, &crm2ActionText, &isReadOnly);

	const JSize count = status.GetElementCount();
	JAssertEqual(expected.GetElementCount(), count);

	for (JIndex i=1; i<=count; i++)
		{
		JString s((JUInt64) i);
		JAssertEqualWithMessage(expected.GetElement(i), status.GetElement(i), s.GetBytes());
		}
}

/******************************************************************************
 GetDoubleClickSelection

 ******************************************************************************/

void
TextEditor::GetDoubleClickSelection
	(
	const JStyledText::TextIndex&	i,
	const JBoolean					partialWord,
	const JBoolean					dragging,
	JStyledText::TextRange*			range
	)
{
	TEGetDoubleClickSelection(i, partialWord, dragging, range);
}

/******************************************************************************
 HandleKeyPress

	Returns kJTrue if the key was processed.

 ******************************************************************************/

JBoolean
TextEditor::HandleKeyPress
	(
	const JUtf8Character&	c,
	const JBoolean			selectText,
	const CaretMotion		motion,
	const JBoolean			deleteToTabStop
	)
{
	return TEHandleKeyPress(c, selectText, motion, deleteToTabStop);
}

/******************************************************************************
 Receive (virtual protected)

	Validate messages

 ******************************************************************************/

void
TextEditor::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == this && message.Is(JTextEditor::kCaretLocationChanged))
		{
		const JTextEditor::CaretLocationChanged* info =
			dynamic_cast<const JTextEditor::CaretLocationChanged*>(&message);
		assert( info != nullptr );

		const JIndex i = info->GetCharacterIndex();

		JAssertTrue(0 < i && i <= GetText()->GetText().GetCharacterCount()+1);
		JAssertTrue(info->GetLineIndex() <= GetLineCount()+1);
		}

	JTextEditor::Receive(sender, message);
}
