/******************************************************************************
 JUserInputFunction.cpp

	Class to accept input from the user while interactively building
	an expression.

	*** We can only be used by JExprEditor, not JExprRenderer.

	BASE CLASS = JFunction, JTextEditor

	Copyright (C) 1996-98 by John Lindal.

 ******************************************************************************/

#include "JUserInputFunction.h"
#include "JExprEditor.h"
#include "JExprRectList.h"
#include "JExprParser.h"

#include "JVariableList.h"
#include "JFunctionWithVar.h"
#include "JProduct.h"
#include "JExponent.h"
#include "JConstantValue.h"

#include <JColorManager.h>
#include <JPainter.h>
#include <jGlobals.h>
#include <jAssert.h>

static const JString kEmptyString("?", kJFalse);

const JCoordinate kHMarginWidth = 2;
const JCoordinate kVMarginWidth = 1;

/******************************************************************************
 Constructor

	text can be nullptr

 ******************************************************************************/

JUserInputFunction::JUserInputFunction
	(
	JExprEditor*	editor,
	const JString&	text
	)
	:
	JTextEditor(kFullEditor, jnew StyledText(editor->GetFontManager()), kJTrue,
				editor->GetFontManager(), kJTrue,
				JColorManager::GetBlackColor(),				// caret
				JColorManager::GetDefaultSelectionColor(),	// selection
				JColorManager::GetBlueColor(),				// outline
				JColorManager::GetBlackColor(),				// whitespace (not used)
				1),
	itsEditor(editor),
	itsGreekFlag(kJFalse)
{
	assert( itsEditor != nullptr );

	// width and height will be set by JTextEditor

	itsWidth  = 0;
	itsHeight = 0;

	itsNeedRedrawFlag = kJFalse;
	itsNeedRenderFlag = kJFalse;

	TECaretShouldBlink(kJTrue);
	TEActivateSelection();

	RecalcAll(kJTrue);
	TESetLeftMarginWidth(kMinLeftMarginWidth);

	if (!text.IsEmpty())
		{
		GetText()->SetText(text);
		}
	else
		{
		Clear();
		}
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JUserInputFunction::JUserInputFunction
	(
	const JUserInputFunction& source
	)
	:
	JFunction(source),
	JTextEditor(source),
	itsEditor(source.itsEditor)
{
	itsWidth  = source.itsWidth;
	itsHeight = source.itsHeight;

	itsNeedRedrawFlag = kJTrue;
	itsNeedRenderFlag = kJTrue;

	TECaretShouldBlink(kJTrue);
	TEActivateSelection();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JUserInputFunction::~JUserInputFunction()
{
}

/******************************************************************************
 Copy

	This provides a way to copy an object without knowing its exact class.

 ******************************************************************************/

JFunction*
JUserInputFunction::Copy()
	const
{
	JUserInputFunction* newFunction = jnew JUserInputFunction(*this);
	assert( newFunction != nullptr );
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

JBoolean
JUserInputFunction::Evaluate
	(
	JFloat* result
	)
	const
{
	*result = 0.0;
	return kJTrue;
}

JBoolean
JUserInputFunction::Evaluate
	(
	JComplex* result
	)
	const
{
	*result = 0.0;
	return kJTrue;
}

/******************************************************************************
 Print

	Our text buffer is unlikely to be parsable, so we write out something
	that is.

 ******************************************************************************/

void
JUserInputFunction::Print
	(
	std::ostream& output
	)
	const
{
	output << kEmptyString;
}

/******************************************************************************
 Layout

 ******************************************************************************/

JIndex
JUserInputFunction::Layout
	(
	const JExprRenderer&	renderer,
	const JPoint&			upperLeft,
	const JSize				fontSize,
	JExprRectList*			rectList
	)
{
	itsNeedRenderFlag = kJFalse;

	if (fontSize != GetText()->GetDefaultFont().GetSize())
		{
		JCharacterRange sel;
		const JBoolean hasSelection = GetSelection(&sel);

		JIndex caret;
		GetCaretLocation(&caret);

		SelectAll();
		SetCurrentFontSize(fontSize);	// changes itsWidth and itsHeight
		GetText()->SetDefaultFontSize(fontSize);

		if (hasSelection)
			{
			SetSelection(sel);
			}
		else
			{
			SetCaretLocation(caret);
			}
		}

	JRect ourRect;
	ourRect.top    = upperLeft.y;
	ourRect.left   = upperLeft.x;
	ourRect.bottom = upperLeft.y + itsHeight;
	ourRect.right  = upperLeft.x + itsWidth;

	const JCoordinate ourMidline = ourRect.ycenter();
	return rectList->AddRect(ourRect, ourMidline, fontSize, this);
}

/******************************************************************************
 Render

 ******************************************************************************/

void
JUserInputFunction::Render
	(
	const JExprRenderer& renderer,
	const JExprRectList& rectList
	)
	const
{
	JUserInputFunction* me = const_cast<JUserInputFunction*>(this);
	assert( me != nullptr );

	itsNeedRedrawFlag = kJFalse;

	// find ourselves in the list

	JIndex ourIndex;
	const JBoolean found = rectList.FindFunction(this, &ourIndex);
	assert( found );

	const JRect ourRect = rectList.GetRect(ourIndex);

	// JTextEditor draws text

	JExprEditor* exprEditor =
		dynamic_cast<JExprEditor*>(const_cast<JExprRenderer*>(&renderer));
	assert( exprEditor != nullptr );

	JPainter* p = exprEditor->GetPainter();
	assert( p != nullptr );

	const JPoint delta = ourRect.topLeft() + JPoint(kHMarginWidth, kVMarginWidth);
	p->ShiftOrigin(delta);
	JRect teRect = ourRect;
	teRect.Shift(-teRect.left, -teRect.top);
	me->TEDraw(*p, teRect);
	p->ShiftOrigin(-delta);

	JRect frame = ourRect;
	frame.Shrink(kHMarginWidth-1, kVMarginWidth-1);
	p->Rect(frame);
}

/******************************************************************************
 IsEmpty

 ******************************************************************************/

JBoolean
JUserInputFunction::IsEmpty()
	const
{
	return JI2B( GetText().GetText() == kEmptyString );
}

/******************************************************************************
 Clear

 ******************************************************************************/

void
JUserInputFunction::Clear()
{
	GetText()->SetText(kEmptyString);
}

/******************************************************************************
 GetEmptyString (static)

 ******************************************************************************/

const JString&
JUserInputFunction::GetEmptyString()
{
	return kEmptyString;
}

/******************************************************************************
 HandleMouseDown

	Returns kJTrue if the object needs to be redrawn.

 ******************************************************************************/

JBoolean
JUserInputFunction::HandleMouseDown
	(
	const JPoint&			pt,
	const JBoolean			extendSelection,
	const JExprRectList&	rectList,
	const JExprRenderer&	renderer
	)
{
	// find ourselves in the list

	JIndex ourIndex;
	const JBoolean found = rectList.FindFunction(this, &ourIndex);
	assert( found );

	const JRect ourRect = rectList.GetRect(ourIndex);

	// JTextEditor does the work

	TEHandleMouseDown(pt - ourRect.topLeft(), 1, extendSelection, kJFalse);
	return itsNeedRedrawFlag;
}

/******************************************************************************
 HandleMouseDrag

	Returns kJTrue if the object needs to be redrawn.

 ******************************************************************************/

JBoolean
JUserInputFunction::HandleMouseDrag
	(
	const JPoint&			pt,
	const JExprRectList&	rectList,
	const JExprRenderer&	renderer
	)
{
	// find ourselves in the list

	JIndex ourIndex;
	const JBoolean found = rectList.FindFunction(this, &ourIndex);
	assert( found );

	const JRect ourRect = rectList.GetRect(ourIndex);

	// JTextEditor does the work

	TEHandleMouseDrag(pt - ourRect.topLeft());
	return itsNeedRedrawFlag;
}

/******************************************************************************
 HandleMouseUp

	Returns kJTrue if the object needs to be redrawn.

 ******************************************************************************/

JBoolean
JUserInputFunction::HandleMouseUp()
{
	TEHandleMouseUp();
	return itsNeedRedrawFlag;
}

/******************************************************************************
 HandleKeyPress

	Deal with the key that was pressed.  We need to be parsed if (,[,e,E
	was pressed.  We always put the (,[ at the end, regardless of where the
	cursor is, both because we can't parse it otherwise, and because it is
	more convenient for the user after editing what was typed in.

	Returns kJTrue if the object needs to be redrawn.

 ******************************************************************************/

JBoolean
JUserInputFunction::HandleKeyPress
	(
	const JUtf8Character&	key,
	JBoolean*				needParse,
	JBoolean*				needRender
	)
{
	*needRender = kJFalse;
	*needParse  = kJFalse;

	if (key == '\'')
		{
		itsGreekFlag = ! itsGreekFlag;
		return kJTrue;
		}

	const JString& text = GetText()->GetText();

	const JBoolean isEmpty = IsEmpty();
	if (isEmpty && (key == '(' || key == '['))
		{
		return kJFalse;
		}
	else if (isEmpty)
		{
		SelectAll();
		}
	else if (key == '(' || key == '[' ||
			 (key.ToLower() == 'e' && !IsEmpty() && !text.IsHexValue() && text.IsFloat()))
		{
		*needParse = kJTrue;
		return kJFalse;
		}

	JUtf8Character c   = key;
	const JUtf8Byte b1 = c.GetBytes()[0];
	if (itsGreekFlag && isupper(b1))		// only translate ascii
	{
		JUtf8Byte b[] = { '\xCE', JUtf8Byte('\x91' + (b1 - 'A')), 0 };
		c.Set(b);
	}
	else if (itsGreekFlag && islower(b1) && b1 < 'p')
	{
		JUtf8Byte b[] = { '\xCE', JUtf8Byte('\xB1' + (b1 - 'a')), 0 };
		c.Set(b);
	}
	else if (itsGreekFlag && islower(b1))
	{
		JUtf8Byte b[] = { '\xCF', JUtf8Byte('\x80' + (b1 - 'p')), 0 };
		c.Set(b);
	}

	TEHandleKeyPress(c, kJFalse, kMoveByCharacter, kJFalse);

	if (GetText()->GetText().IsEmpty())
		{
		GetText()->SetText(kEmptyString);
		}

	*needRender = itsNeedRenderFlag;
	return itsNeedRedrawFlag;
}

/******************************************************************************
 Parse

	If c is '(', parse the characters in the buffer as a function.
	If c is '[', parse the characters in the buffer as an array variable.
	If c is 'e', parse the characters in the buffer as a number and then append 10^?.
	Otherwise, parse the characters in the buffer as a normal variable.

 ******************************************************************************/

JBoolean
JUserInputFunction::Parse
	(
	const JUtf8Character&	c,
	JFunction**				f,
	JUserInputFunction**	newUIF,
	JBoolean*				needRender
	)
{
	*f          = nullptr;
	*newUIF     = nullptr;
	*needRender = kJFalse;

	JString buffer = GetText()->GetText();
	if (buffer == kEmptyString)
		{
		(JGetUserNotification())->ReportError(JGetString("EmptyFunction::JExprEditor"));
		return kJFalse;
		}

	buffer.TrimWhitespace();

	JFloat x;
	if (c == '(')
		{
		JFunction* newArg;
		JUserInputFunction* extraUIF;
		JUserInputFunction* tempUIF = jnew JUserInputFunction(itsEditor);
		assert( tempUIF != nullptr );
		const JBoolean ok = itsEditor->ApplyFunction(buffer, *tempUIF, f, &newArg, &extraUIF);
		jdelete tempUIF;
		if (ok)
			{
			*newUIF = dynamic_cast<JUserInputFunction*>(newArg);
			assert( *newUIF != nullptr );
			}
		return ok;
		}
	else if (c == '[')
		{
		buffer += "1]";

		JExprParser p(itsEditor->GetVariableList(), itsEditor->GetFontManager());
		if (p.Parse(buffer, f))
			{
			JFunctionWithVar* fwv = dynamic_cast<JFunctionWithVar*>(*f);
			assert( fwv != nullptr );
			*newUIF = jnew JUserInputFunction(itsEditor);
			assert( *newUIF != nullptr );
			fwv->SetArrayIndex(*newUIF);
			return kJTrue;
			}
		else
			{
			return kJFalse;
			}
		}
	else if (c.ToLower() == 'e' && !IsEmpty() && !buffer.IsHexValue() &&
			 buffer.ConvertToFloat(&x))
		{
		JConstantValue* expBase = jnew JConstantValue(10.0);
		assert( expBase != nullptr );
		*newUIF = jnew JUserInputFunction(itsEditor);
		assert( *newUIF != nullptr );
		JExponent* exponent = jnew JExponent(expBase, *newUIF);
		assert( exponent != nullptr );
		if (x == 1.0)
			{
			*f = exponent;
			}
		else
			{
			JConstantValue* mantissa = jnew JConstantValue(x);
			assert( mantissa != nullptr );
			JProduct* product = jnew JProduct;
			assert( product != nullptr );
			product->SetArg(1, mantissa);
			product->SetArg(2, exponent);
			*f = product;
			}
		return kJTrue;
		}
	else
		{
		JExprParser p(itsEditor->GetVariableList(), itsEditor->GetFontManager());
		return p.Parse(buffer, f);
		}
}

/******************************************************************************
 TERefresh (virtual protected)

 ******************************************************************************/

void
JUserInputFunction::TERefresh()
{
	itsNeedRedrawFlag = kJTrue;
}

/******************************************************************************
 TERefreshRect (virtual protected)

 ******************************************************************************/

void
JUserInputFunction::TERefreshRect
	(
	const JRect& rect
	)
{
	itsNeedRedrawFlag = kJTrue;
}

/******************************************************************************
 TERedraw (virtual protected)

 ******************************************************************************/

void
JUserInputFunction::TERedraw()
{
	itsNeedRedrawFlag = kJTrue;
}

/******************************************************************************
 TESetGUIBounds (virtual protected)

 ******************************************************************************/

void
JUserInputFunction::TESetGUIBounds
	(
	const JCoordinate w,
	const JCoordinate h,
	const JCoordinate changeY
	)
{
	itsWidth  = w + 2*kHMarginWidth;
	itsHeight = h + 2*kVMarginWidth;

	itsNeedRenderFlag = kJTrue;
}

/******************************************************************************
 TEWidthIsBeyondDisplayCapacity (virtual protected)

 ******************************************************************************/

JBoolean
JUserInputFunction::TEWidthIsBeyondDisplayCapacity
	(
	const JSize width
	)
	const
{
	return JI2B( width > 16000 );	// X uses 2 bytes for coordinate value
}

/******************************************************************************
 TEScrollToRect (virtual protected)

	We don't scroll.

 ******************************************************************************/

JBoolean
JUserInputFunction::TEScrollToRect
	(
	const JRect&	rect,
	const JBoolean	centerInDisplay
	)
{
	return kJFalse;
}

/******************************************************************************
 TEScrollForDrag (virtual protected)

	We don't scroll.

 ******************************************************************************/

JBoolean
JUserInputFunction::TEScrollForDrag
	(
	const JPoint& pt
	)
{
	return kJFalse;
}

/******************************************************************************
 TEScrollForDND (virtual protected)

	We don't scroll.

 ******************************************************************************/

JBoolean
JUserInputFunction::TEScrollForDND
	(
	const JPoint& pt
	)
{
	return kJFalse;
}

/******************************************************************************
 TESetVertScrollStep (virtual protected)

	We don't scroll.

 ******************************************************************************/

void
JUserInputFunction::TESetVertScrollStep
	(
	const JCoordinate vStep
	)
{
}

/******************************************************************************
 TEClipboardChanged (virtual protected)

 ******************************************************************************/

void
JUserInputFunction::TEUpdateClipboard
	(
	const JString&			text,
	const JRunArray<JFont>&	style
	)
	const
{
}

/******************************************************************************
 TEGetExternalClipboard (virtual protected)

	We don't deal with the system clipboard.

 ******************************************************************************/

JBoolean
JUserInputFunction::TEGetClipboard
	(
	JString*			text,
	JRunArray<JFont>*	style
	)
	const
{
	return kJFalse;
}

/******************************************************************************
 TEBeginDND (virtual protected)

	We don't support Drag-And-Drop.

 ******************************************************************************/

JBoolean
JUserInputFunction::TEBeginDND()
{
	return kJFalse;
}

/******************************************************************************
 TEPasteDropData (virtual protected)

	There is nothing to do since we don't support Drag-And-Drop.

 ******************************************************************************/

void
JUserInputFunction::TEPasteDropData()
{
}

/******************************************************************************
 TECaretShouldBlink (virtual protected)

	We don't blink our caret because it requires system dependent code.

 ******************************************************************************/

void
JUserInputFunction::TECaretShouldBlink
	(
	const JBoolean blink
	)
{
	if (blink)
		{
		TEShowCaret();
		}
	else
		{
		TEHideCaret();
		}
}

/******************************************************************************
 TEHasSearchText (virtual)

 ******************************************************************************/

JBoolean
JUserInputFunction::TEHasSearchText()
	const
{
	return kJFalse;
}

/******************************************************************************
 AdjustStylesBeforeBroadcast (virtual protected)

	Draw the empty string using gray.

 ******************************************************************************/

void
JUserInputFunction::StyledText::AdjustStylesBeforeBroadcast
	(
	const JString&			text,
	JRunArray<JFont>*		styles,
	JStyledText::TextRange*	recalcRange,
	JStyledText::TextRange*	redrawRange,
	const JBoolean			deletion
	)
{
	const JSize totalLength = text.GetCharacterCount();

	JFont f = styles->GetFirstElement();
	styles->RemoveAll();

	f.SetColor(text == kEmptyString ? JColorManager::GetInactiveLabelColor() :
									  JColorManager::GetBlackColor());

	styles->AppendElements(f, totalLength);

	*recalcRange = *redrawRange = JStyledText::TextRange(
		JCharacterRange(1, totalLength),
		JUtf8ByteRange(1, text.GetByteCount()));
}
