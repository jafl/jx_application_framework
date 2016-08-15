/******************************************************************************
 JUserInputFunction.cpp

	Class to accept input from the user while interactively building
	an expression.

	*** We can only be used by JExprEditor, not JExprRenderer.

	BASE CLASS = JFunction, JTextEditor

	Copyright © 1996-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JUserInputFunction.h>
#include <jExprUIUtil.h>
#include <JExprEditor.h>
#include <JExprRectList.h>

#include <jParseFunction.h>
#include <jParserData.h>
#include <JVariableList.h>
#include <JFunctionWithVar.h>
#include <JProduct.h>
#include <JExponent.h>
#include <JConstantValue.h>

#include <JPainter.h>
#include <JColormap.h>
#include <JString.h>
#include <string.h>
#include <ctype.h>
#include <jGlobals.h>
#include <jAssert.h>

static const JCharacter* kEmptyString = "?";

const JCoordinate kHMarginWidth = 2;
const JCoordinate kVMarginWidth = 1;

/******************************************************************************
 Constructor

	text can be NULL

 ******************************************************************************/

JUserInputFunction::JUserInputFunction
	(
	const JVariableList*	varList,
	const JFontManager*		fontManager,
	JColormap*				colormap,
	const JCharacter*		text
	)
	:
	JFunction(kJUserInputType),
	JTextEditor(kFullEditor, kJTrue, kJFalse, kJTrue, fontManager, colormap,
				colormap->GetBlackColor(),				// caret
				colormap->GetDefaultSelectionColor(),	// selection
				colormap->GetBlueColor(),				// outline
				colormap->GetBlackColor(),				// drag (not used)
				colormap->GetBlackColor(),				// whitespace (not used)
				1)
{
	itsVarList = varList;

	// width and height will be set by JTextEditor

	itsWidth  = 0;
	itsHeight = 0;

	itsNeedRedrawFlag = kJFalse;
	itsNeedRenderFlag = kJFalse;

	TECaretShouldBlink(kJTrue);
	TEActivateSelection();

	RecalcAll(kJTrue);
	TESetLeftMarginWidth(kMinLeftMarginWidth);

	if (text != NULL)
		{
		SetParseableText(this, text);
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
	JTextEditor(source)
{
	itsVarList = source.itsVarList;

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
	JUserInputFunction* newFunction = new JUserInputFunction(*this);
	assert( newFunction != NULL );
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
	ostream& output
	)
	const
{
	output << kEmptyString;
}

/******************************************************************************
 SameAs

	Returns kJTrue if the given function is identical to us.
	We only compare the text.

 ******************************************************************************/

JBoolean
JUserInputFunction::SameAs
	(
	const JFunction& theFunction
	)
	const
{
	if (!JFunction::SameAs(theFunction))
		{
		return kJFalse;
		}

	const JUserInputFunction& theUIF = (const JUserInputFunction&) theFunction;
	return JConvertToBoolean( GetText() == theUIF.GetText() );
}

/******************************************************************************
 PrepareToRender

 ******************************************************************************/

JIndex
JUserInputFunction::PrepareToRender
	(
	const JExprRenderer&	renderer,
	const JPoint&			upperLeft,
	const JSize				fontSize,
	JExprRectList*			rectList
	)
{
	itsNeedRenderFlag = kJFalse;

	if (fontSize != GetDefaultFont().GetSize())
		{
		JIndex selStart, selEnd, caretLoc;
		const JBoolean hasSelection = GetSelection(&selStart, &selEnd);
		const JBoolean hasCaret     = GetCaretLocation(&caretLoc);

		SelectAll();
		SetCurrentFontSize(fontSize);	// changes itsWidth and itsHeight
		SetDefaultFontSize(fontSize);

		if (hasSelection)
			{
			SetSelection(selStart, selEnd);
			}
		else
			{
			SetCaretLocation(caretLoc);
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
	assert( me != NULL );

	itsNeedRedrawFlag = kJFalse;

	// find ourselves in the list

	JIndex ourIndex;
	const JBoolean found = rectList.FindFunction(this, &ourIndex);
	assert( found );

	const JRect ourRect = rectList.GetRect(ourIndex);

	// JTextEditor draws text

	JExprEditor* exprEditor =
		dynamic_cast<JExprEditor*>(const_cast<JExprRenderer*>(&renderer));
	assert( exprEditor != NULL );

	JPainter* p = exprEditor->GetPainter();
	assert( p != NULL );

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
 BuildNodeList

	We are a terminal node.

 ******************************************************************************/

void
JUserInputFunction::BuildNodeList
	(
	JExprNodeList*	nodeList,
	const JIndex	myNode
	)
{
}

/******************************************************************************
 IsEmpty

 ******************************************************************************/

JBoolean
JUserInputFunction::IsEmpty()
	const
{
	return JConvertToBoolean( GetText() == kEmptyString );
}

/******************************************************************************
 Clear

 ******************************************************************************/

void
JUserInputFunction::Clear()
{
	SetText(kEmptyString);
}

/******************************************************************************
 GetEmptyString (static)

 ******************************************************************************/

const JCharacter*
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
	TEHandleMouseUp(kJFalse);
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
	const JCharacter	key,
	JBoolean*			needParse,
	JBoolean*			needRender
	)
{
	*needRender = kJFalse;
	*needParse  = kJFalse;

	if (key == JPGetGreekCharPrefixChar())
		{
		const JFont currFont = GetCurrentFont();
		if (strcmp(currFont.GetName(), JGetDefaultFontName()) == 0)
			{
			SetCurrentFontName(JGetGreekFontName());
			}
		else
			{
			SetCurrentFontName(JGetDefaultFontName());
			}
		return kJTrue;
		}

	const JBoolean isEmpty = IsEmpty();
	if (isEmpty && (key == '(' || key == '['))
		{
		return kJFalse;
		}
	else if (isEmpty)
		{
		const JFont font = GetCurrentFont();
		SelectAll();
		SetCurrentFontName(font.GetName());
		}
	else if (key == '(' || key == '[')
		{
		SetCaretLocation(GetTextLength()+1);
		}

	TEHandleKeyPress(key, kJFalse, kMoveByCharacter, kJFalse);

	const JString& text = GetText();
	const JSize textLen = text.GetLength();
	JCharacter c = '\0';
	if (textLen > 0)
		{
		c = text.GetLastCharacter();
		}

	if (textLen == 0)
		{
		SetText(kEmptyString);
		}
	else if (textLen > 1 && (c == '(' || c == '['))
		{
		*needParse = kJTrue;
		}
	else if (textLen > 1 && tolower(c) == 'e')
		{
		*needParse = JI2B((text.GetSubstring(1, textLen-1)).IsFloat() &&
						  !text.IsHexValue());
		}

	*needRender = itsNeedRenderFlag;
	return itsNeedRedrawFlag;
}

/******************************************************************************
 Parse

	If the last character is '(', parse the characters in the buffer as a function.
	If the last character is '[', parse the characters in the buffer as an array variable.
	If the last character is 'e', parse the characters in the buffer as a number
								  and then append 10^?.
	Otherwise, parse the characters in the buffer as a normal variable.

 ******************************************************************************/

extern const JCharacter* kArgSeparatorString;

extern const JSize stdFunctionCount;
extern const JStdFunctionInfo stdFunction[];

JBoolean
JUserInputFunction::Parse
	(
	JFunction**				f,
	JUserInputFunction**	newUIF,
	JBoolean*				needRender
	)
{
	*f          = NULL;
	*newUIF     = NULL;
	*needRender = kJFalse;

	JString buffer = GetParseableText(*this);
	if (buffer == kEmptyString)
		{
		(JGetUserNotification())->ReportError("This function is empty.");
		return kJFalse;
		}

	buffer.TrimWhitespace();
	const JSize length = buffer.GetLength();
	const JCharacter lastChar = buffer.GetLastCharacter();

	JFloat x;
	if (lastChar == '(')
		{
		JFunction* newArg;
		JUserInputFunction* extraUIF;
		JUserInputFunction* tempUIF =
			new JUserInputFunction(itsVarList, TEGetFontManager(), TEGetColormap());
		assert( tempUIF != NULL );
		const JBoolean ok =
			JApplyFunction(buffer, itsVarList, *tempUIF, TEGetFontManager(), TEGetColormap(),
						   f, &newArg, &extraUIF);
		delete tempUIF;
		if (ok)
			{
			*newUIF = dynamic_cast<JUserInputFunction*>(newArg);
			assert( *newUIF != NULL );
			}
		else
			{
			SetSelection(length, GetTextLength());
			DeleteSelection();
			}
		return ok;
		}
	else if (lastChar == '[')
		{
		JIndex varIndex;
		const JString varNamePrefix = buffer.GetSubstring(1,length-1);
		JString maxPrefix;
		const JVariableList::MatchResult match =
			itsVarList->FindUniqueVarName(varNamePrefix, &varIndex, &maxPrefix);
		if (match == JVariableList::kSingleMatch)
			{
			buffer = maxPrefix + "[";
			}
		else if (match == JVariableList::kMultipleMatch)
			{
			SetParseableText(this, maxPrefix);
			SetCaretLocation(GetTextLength()+1);
			*needRender = kJTrue;
			return kJFalse;
			}
		// JParseFunction reports kNoMatch as an error

		buffer += "1]";
		const JBoolean ok = JParseFunction(buffer, itsVarList, f);
		if (ok)
			{
			JFunctionWithVar* fwv = (**f).CastToJFunctionWithVar();
			assert( fwv != NULL );
			*newUIF = new JUserInputFunction(itsVarList, TEGetFontManager(), TEGetColormap());
			assert( *newUIF != NULL );
			fwv->SetArrayIndex(*newUIF);
			}
		else
			{
			SetSelection(length, length);
			DeleteSelection();
			}
		return ok;
		}
	else if (tolower(lastChar) == 'e' && length>1 && !buffer.IsHexValue() &&
			 (buffer.GetSubstring(1,length-1)).ConvertToFloat(&x))
		{
		JConstantValue* expBase = new JConstantValue(10.0);
		assert( expBase != NULL );
		*newUIF = new JUserInputFunction(itsVarList, TEGetFontManager(), TEGetColormap());
		assert( *newUIF != NULL );
		JExponent* exponent = new JExponent(expBase, *newUIF);
		assert( exponent != NULL );
		if (x == 1.0)
			{
			*f = exponent;
			}
		else
			{
			JConstantValue* mantissa = new JConstantValue(x);
			assert( mantissa != NULL );
			JProduct* product = new JProduct;
			assert( product != NULL );
			product->SetArg(1, mantissa);
			product->SetArg(2, exponent);
			*f = product;
			}
		return kJTrue;
		}
	else
		{
		JIndex varIndex;
		JString maxPrefix;
		const JVariableList::MatchResult match =
			itsVarList->FindUniqueVarName(buffer, &varIndex, &maxPrefix);
		if (match == JVariableList::kSingleMatch)
			{
			buffer = maxPrefix;
			}
		else if (match == JVariableList::kMultipleMatch)
			{
			SetParseableText(this, maxPrefix);
			SetCaretLocation(GetTextLength()+1);
			*needRender = kJTrue;
			return kJFalse;
			}
		// JParseFunction reports kNoMatch as an error

		return JParseFunction(buffer, itsVarList, f);
		}
}

/******************************************************************************
 GetParseableText (static)

 ******************************************************************************/

JString
JUserInputFunction::GetParseableText
	(
	const JTextEditor& te
	)
{
	const JCharacter* greekFontName     = JGetGreekFontName();
	const JCharacter* greekPrefixString = JPGetGreekCharPrefixString();

	JString text = te.GetText();
	const JSize length = text.GetLength();
	for (JIndex i=length; i>=1; i--)
		{
		if (strcmp(te.GetFont(i).GetName(), greekFontName) == 0)
			{
			text.InsertSubstring(greekPrefixString, i);
			}
		}

	return text;
}

/******************************************************************************
 SetParseableText (static)

 ******************************************************************************/

void
JUserInputFunction::SetParseableText
	(
	JTextEditor*		te,
	const JCharacter*	text
	)
{
	JArray<JIndex> greekList;

	JString newText = text;
	{
	const JCharacter greekPrefix = JPGetGreekCharPrefixChar();

	JSize length = newText.GetLength();
	for (JIndex i=1; i<=length; i++)
		{
		if (newText.GetCharacter(i) == greekPrefix)
			{
			greekList.AppendElement(i);
			newText.RemoveSubstring(i,i);
			length--;
			}
		}
	}

	te->SetText(newText);
	{
	const JSize greekCount = greekList.GetElementCount();
	for (JIndex i=1; i<=greekCount; i++)
		{
		const JIndex j = greekList.GetElement(i);
		te->SetFontName(j,j, JGetGreekFontName(), kJTrue);
		}
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
JUserInputFunction::TEClipboardChanged()
{
}

/******************************************************************************
 TEOwnsClipboard (virtual protected)

 ******************************************************************************/

JBoolean
JUserInputFunction::TEOwnsClipboard()
	const
{
	return kJTrue;
}

/******************************************************************************
 TEGetExternalClipboard (virtual protected)

	We don't deal with the system clipboard.

 ******************************************************************************/

JBoolean
JUserInputFunction::TEGetExternalClipboard
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

	This is never needed.

 ******************************************************************************/

void
JUserInputFunction::TEDisplayBusyCursor()
	const
{
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
