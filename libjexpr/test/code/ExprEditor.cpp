/******************************************************************************
 ExprEditor.cpp

	BASE CLASS = JExprEditor

	Written by John Lindal.

 ******************************************************************************/

#include "ExprEditor.h"
#include <JFunction.h>
#include <JExprRectList.h>
#include <JTestManager.h>
#include <JRect.h>
#include <jMath.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

ExprEditor::ExprEditor
	(
	const JVariableList*	varList,
	JFontManager*			fontManager
	)
	:
	JExprEditor(varList, fontManager)
{
	ClearFunction();
	EIPActivate();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

ExprEditor::~ExprEditor()
{
}

/******************************************************************************
 EIPRefresh (virtual protected)

 ******************************************************************************/

void
ExprEditor::EIPRefresh()
{
}

/******************************************************************************
 EIPRedraw (virtual protected)

 ******************************************************************************/

void
ExprEditor::EIPRedraw()
{
}

/******************************************************************************
 EIPBoundsChanged (virtual protected)

 ******************************************************************************/

void
ExprEditor::EIPBoundsChanged()
{
}

/******************************************************************************
 EIPScrollToRect (virtual protected)

 ******************************************************************************/

JBoolean
ExprEditor::EIPScrollToRect
	(
	const JRect& r
	)
{
	return kJFalse;
}

/******************************************************************************
 EIPScrollForDrag (virtual protected)

 ******************************************************************************/

JBoolean
ExprEditor::EIPScrollForDrag
	(
	const JPoint& pt
	)
{
	return kJFalse;
}

/******************************************************************************
 EIPAdjustNeedTab (virtual protected)

 ******************************************************************************/

void
ExprEditor::EIPAdjustNeedTab
	(
	const JBoolean needTab
	)
{
}

/******************************************************************************
 HandleMouseDown

 ******************************************************************************/

void
ExprEditor::HandleMouseDown
	(
	const JPoint&	pt,
	const JBoolean	extend
	)
{
	EIPHandleMouseDown(pt, extend);
}

/******************************************************************************
 HandleMouseDrag

 ******************************************************************************/

void
ExprEditor::HandleMouseDrag
	(
	const JPoint& pt
	)
{
	EIPHandleMouseDrag(pt);
}

/******************************************************************************
 HandleMouseUp

 ******************************************************************************/

void
ExprEditor::HandleMouseUp
	(
	const JPoint& pt
	)
{
	EIPHandleMouseUp();
}

/******************************************************************************
 HandleKeyPress

 ******************************************************************************/

void
ExprEditor::HandleKeyPress
	(
	const JUtf8Character& c
	)
{
	EIPHandleKeyPress(c);
}

/******************************************************************************
 EIPClipboardChanged (virtual protected)

 ******************************************************************************/

void
ExprEditor::EIPClipboardChanged()
{
}

/******************************************************************************
 EIPOwnsClipboard (virtual protected)

	We can't know if the current data is ours.

 ******************************************************************************/

JBoolean
ExprEditor::EIPOwnsClipboard()
{
	return kJTrue;
}

/******************************************************************************
 EIPGetExternalClipboard (virtual protected)

	Returns kJTrue if there is something pasteable on the system clipboard.

 ******************************************************************************/

JBoolean
ExprEditor::EIPGetExternalClipboard
	(
	JString* text
	)
{
	text->Clear();
	return kJFalse;
}

/******************************************************************************
 Activate

 ******************************************************************************/

void
ExprEditor::Activate()
{
	EIPActivate();
}

/******************************************************************************
 CheckCmdStatus

 ******************************************************************************/

void
ExprEditor::CheckCmdStatus
	(
	const JRunArray<JBoolean>& expected
	)
	const
{
	const JArray<JBoolean> status = GetCmdStatus(nullptr);

	const JSize count = status.GetElementCount();
	JAssertEqual(expected.GetElementCount(), count);

	for (JIndex i=1; i<=count; i++)
		{
		JString s((JUInt64) i);
		JAssertEqualWithMessage(expected.GetElement(i), status.GetElement(i), s.GetBytes());
		}
}
