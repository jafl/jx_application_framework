/******************************************************************************
 TestExprEditor.cpp

	BASE CLASS = JExprEditor

	Written by John Lindal.

 ******************************************************************************/

#include "TestExprEditor.h"
#include <JFunction.h>
#include <JExprRectList.h>
#include <JString.h>
#include <JRect.h>
#include <jMath.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TestExprEditor::TestExprEditor
	(
	JFontManager*			fontManager,
	const JVariableList*	varList
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

TestExprEditor::~TestExprEditor()
{
}

/******************************************************************************
 EIPRefresh (virtual protected)

 ******************************************************************************/

void
TestExprEditor::EIPRefresh()
{
}

/******************************************************************************
 EIPRedraw (virtual protected)

 ******************************************************************************/

void
TestExprEditor::EIPRedraw()
{
}

/******************************************************************************
 EIPBoundsChanged (virtual protected)

 ******************************************************************************/

void
TestExprEditor::EIPBoundsChanged()
{
}

/******************************************************************************
 EIPScrollToRect (virtual protected)

 ******************************************************************************/

JBoolean
TestExprEditor::EIPScrollToRect
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
TestExprEditor::EIPScrollForDrag
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
TestExprEditor::EIPAdjustNeedTab
	(
	const JBoolean needTab
	)
{
}

/******************************************************************************
 HandleMouseDown

 ******************************************************************************/

void
TestExprEditor::HandleMouseDown
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
TestExprEditor::HandleMouseDrag
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
TestExprEditor::HandleMouseUp
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
TestExprEditor::HandleKeyPress
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
TestExprEditor::EIPClipboardChanged()
{
}

/******************************************************************************
 EIPOwnsClipboard (virtual protected)

	We can't know if the current data is ours.

 ******************************************************************************/

JBoolean
TestExprEditor::EIPOwnsClipboard()
{
	return kJTrue;
}

/******************************************************************************
 EIPGetExternalClipboard (virtual protected)

	Returns kJTrue if there is something pasteable on the system clipboard.

 ******************************************************************************/

JBoolean
TestExprEditor::EIPGetExternalClipboard
	(
	JString* text
	)
{
	text->Clear();
	return kJFalse;
}
