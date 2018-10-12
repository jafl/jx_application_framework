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
#include <JStringIterator.h>
#include <jGlobals.h>
#include <jAssert.h>

const JUtf8Character JUserInputFunction::kSwitchFontCharacter('`');

static const JString kEmptyString("?", kJFalse);

const JCoordinate kHMarginWidth = 2;
const JCoordinate kVMarginWidth = 1;

/******************************************************************************
 Constructor

 ******************************************************************************/

JUserInputFunction::JUserInputFunction
	(
	JExprEditor*	editor,
	const JString&	text
	)
	:
	JTextEditor(kFullEditor, editor->BuildStyledText(), kJTrue,
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

	RecalcAll();
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
	JTextEditor(source, source.itsEditor->BuildStyledText()),
	itsEditor(source.itsEditor),
	itsWidth(source.itsWidth),
	itsHeight(source.itsHeight),
	itsGreekFlag(kJFalse),
	itsNeedRedrawFlag(kJTrue),
	itsNeedRenderFlag(kJTrue)
{
	GetText()->SetText(source.GetText().GetText());

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
	kEmptyString.Print(output);
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
	const JUtf8Character&	c,
	JBoolean*				needParse,
	JBoolean*				needRender
	)
{
	*needRender = kJFalse;
	*needParse  = kJFalse;

	if (c == kSwitchFontCharacter)
		{
		itsGreekFlag = ! itsGreekFlag;
		return kJTrue;
		}

	const JString& text = GetText()->GetText();

	const JBoolean isEmpty = IsEmpty();
	if (isEmpty && (c == '(' || c == '['))
		{
		return kJFalse;
		}
	else if (isEmpty)
		{
		SelectAll();
		}
	else if (c == '(' || c == '[' ||
			 (c.ToLower() == 'e' && !IsEmpty() && !text.IsHexValue() && text.IsFloat()))
		{
		*needParse = kJTrue;
		return kJFalse;
		}

	TEHandleKeyPress(itsGreekFlag ? ConvertToGreek(c) : c,
					 kJFalse, kMoveByCharacter, kJFalse);

	if (GetText()->GetText().IsEmpty())
		{
		GetText()->SetText(kEmptyString);
		}

	*needRender = itsNeedRenderFlag;
	return itsNeedRedrawFlag;
}

/******************************************************************************
 ConvertToGreek (static)

 ******************************************************************************/

JString
JUserInputFunction::ConvertToGreek
	(
	const JString& s
	)
{
	if (!s.Contains(JUserInputFunction::kSwitchFontCharacter))
		{
		return s;
		}

	JString g = s;

	JStringIterator iter(&g);
	JUtf8Character c;
	while (iter.Next(&c))
		{
		if (c == JUserInputFunction::kSwitchFontCharacter)
			{
			iter.RemovePrev();
			if (iter.Next(&c))
				{
				iter.SetPrev(JUserInputFunction::ConvertToGreek(c));
				}
			}
		}

	return g;
}

/******************************************************************************
 ConvertToGreek (static)

 ******************************************************************************/

struct GreekCharacterMapping
{
	JUtf8Byte			ascii;
	const JUtf8Byte*	lower;
	const JUtf8Byte*	upper;
};

static const GreekCharacterMapping kGreekData[] =
{
	{ 'A', "\xCE\xB1", "\xCE\x91" },	// alpha
	{ 'B', "\xCE\xB2", "\xCE\x92" },	// beta
	{ 'D', "\xCE\xB4", "\xCE\x94" },	// delta
	{ 'E', "\xCE\xB5", "\xCE\x95" },	// epsilon
	{ 'F', "\xCF\x86", "\xCE\xA6" },	// phi
	{ 'G', "\xCE\xB3", "\xCE\x93" },	// gamma
	{ 'H', "\xCE\xB7", "\xCE\x97" },	// eta
	{ 'I', "\xCE\xB9", "\xCE\x99" },	// iota
	{ 'J', "\xCE\xB8", "\xCE\x98" },	// theta
	{ 'K', "\xCE\xBA", "\xCE\x9A" },	// kappa
	{ 'L', "\xCE\xBB", "\xCE\x9B" },	// lambda
	{ 'M', "\xCE\xBC", "\xCE\x9C" },	// mu
	{ 'N', "\xCE\xBD", "\xCE\x9D" },	// nu
	{ 'O', "\xCE\xBF", "\xCE\x9F" },	// omicron
	{ 'P', "\xCF\x80", "\xCE\xA0" },	// pi
	{ 'Q', "\xCE\xBE", "\xCE\x9E" },	// xi
	{ 'R', "\xCF\x81", "\xCE\xA1" },	// rho
	{ 'S', "\xCF\x83", "\xCE\xA3" },	// sigma
	{ 'T', "\xCF\x84", "\xCE\xA4" },	// tau
	{ 'U', "\xCF\x85", "\xCE\xA5" },	// upsilon
	{ 'W', "\xCF\x89", "\xCE\xA9" },	// omega
	{ 'X', "\xCF\x87", "\xCE\xA7" },	// chi
	{ 'Y', "\xCF\x88", "\xCE\xA8" },	// psi
	{ 'Z', "\xCE\xB6", "\xCE\x96" }		// zeta
};

const JSize kGreekCount = sizeof(kGreekData) / sizeof(GreekCharacterMapping);

static JBoolean theInitGreekMappingFlag = kJFalse;
static GreekCharacterMapping kGreekMapping[1+int('Z')];

JUtf8Character
JUserInputFunction::ConvertToGreek
	(
	const JUtf8Character& c
	)
{
	if (!theInitGreekMappingFlag)
		{
		bzero(kGreekMapping, sizeof(kGreekMapping));

		for (int i=0; i<kGreekCount; i++)
			{
			kGreekMapping[ int(kGreekData[i].ascii) ] = kGreekData[i];
			}

		theInitGreekMappingFlag = kJTrue;
		}

	JUtf8Character g   = c;
	const JUtf8Byte b1 = g.GetBytes()[0];
	if (isupper(b1) && kGreekMapping[int(b1)].ascii != 0)
	{
		g.Set(kGreekMapping[int(b1)].upper);
	}
	else if (islower(b1) && kGreekMapping[toupper(b1)].ascii != 0)
	{
		g.Set(kGreekMapping[toupper(b1)].lower);
	}

	return g;
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

		JExprParser p(itsEditor->GetVariableList());
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
		JExprParser p(itsEditor->GetVariableList());
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
 AdjustStylesBeforeBroadcast (static)

	Draw the empty string using gray.

 ******************************************************************************/

void
JUserInputFunction::AdjustStylesBeforeBroadcast
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
