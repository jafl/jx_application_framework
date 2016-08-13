/******************************************************************************
 JXExprWidget.cpp

	Maintains a pane that displays the result of rendering a JFunction.

	BASE CLASS = JXScrollableWidget, JExprEditor

	Copyright © 2000 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include <JXExprWidget.h>
#include <JXExprEvalDirector.h>
#include <JFunction.h>
#include <JExprRectList.h>
#include <jParserData.h>

#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXWindowPainter.h>
#include <JXSelectionManager.h>
#include <JXTextSelection.h>
#include <JXEPSPrinter.h>

#include <JPagePrinter.h>

#include <jXEventUtil.h>
#include <JXColormap.h>
#include <jXGlobals.h>
#include <jXActionDefs.h>
#include <jXKeysym.h>

#include <JMinMax.h>
#include <jASCIIConstants.h>
#include <JString.h>
#include <JRect.h>
#include <jMath.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXExprWidget::JXExprWidget
	(
	const JVariableList*	varList,
	JXScrollbarSet*			scrollbarSet,
	JXContainer*			enclosure,
	const HSizingOption		hSizing,
	const VSizingOption		vSizing,
	const JCoordinate		x,
	const JCoordinate		y,
	const JCoordinate		w,
	const JCoordinate		h
	)
	:
	JXScrollableWidget(scrollbarSet, enclosure, hSizing, vSizing, x,y, w,h),
	JExprEditor(varList, JXScrollableWidget::GetFontManager(),
				JXScrollableWidget::GetColormap())
{
	JXExprEditorX();
}

// private

void
JXExprWidget::JXExprEditorX()
{
	// required by JXGetCurrColormap
	assert( GetWindow()->GetColormap() == GetDisplay()->GetColormap() );

	// insure that we have a JFunction to display
	// (also calls EIPBoundsChanged and EIPAdjustNeedTab)

	ClearFunction();
	EIPDeactivate();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXExprWidget::~JXExprWidget()
{
}

/******************************************************************************
 GetMultiplicationString (virtual protected)

 ******************************************************************************/

const JCharacter*
JXExprWidget::GetMultiplicationString()
	const
{
	return "\xB7";
}

/******************************************************************************
 EIPRefresh (virtual protected)

 ******************************************************************************/

void
JXExprWidget::EIPRefresh()
{
	Refresh();
}

/******************************************************************************
 EIPRedraw (virtual protected)

 ******************************************************************************/

void
JXExprWidget::EIPRedraw()
{
	Redraw();
}

/******************************************************************************
 EIPBoundsChanged (virtual protected)

	Adjust our bounds to fit the expression.

 ******************************************************************************/

void
JXExprWidget::EIPBoundsChanged()
{
	const JRect newBounds = GetRectList()->GetBoundsRect();
	const JRect apG       = GetApertureGlobal();
	const JCoordinate w   = JMax(newBounds.width(),  apG.width());
	const JCoordinate h   = JMax(newBounds.height(), apG.height());
	SetBounds(w,h);
}

/******************************************************************************
 ApertureResized (virtual protected)

	Adjust our Bounds so the expression remains centered within Aperture.

 ******************************************************************************/

void
JXExprWidget::ApertureResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXScrollableWidget::ApertureResized(dw,dh);
	EIPBoundsChanged();
}

/******************************************************************************
 EIPScrollToRect (virtual protected)

	Scroll the pane to make the given rectangle visible.  Return kJTrue
	if scrolling was necessary.

 ******************************************************************************/

JBoolean
JXExprWidget::EIPScrollToRect
	(
	const JRect& r
	)
{
	JPoint delta;
	GetDrawingOffset(&delta);
	JRect r1 = r;
	r1.Shift(delta);
	return ScrollToRect(r1);
}

/******************************************************************************
 EIPScrollForDrag (virtual protected)

 ******************************************************************************/

JBoolean
JXExprWidget::EIPScrollForDrag
	(
	const JPoint& pt
	)
{
	return ScrollForDrag(RendererToBounds(pt));
}

/******************************************************************************
 EIPAdjustNeedTab (virtual protected)

 ******************************************************************************/

void
JXExprWidget::EIPAdjustNeedTab
	(
	const JBoolean needTab
	)
{
	WantInput(kJTrue, needTab);
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
JXExprWidget::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	PrivateDraw(p, rect);
}

/******************************************************************************
 Print (virtual protected)

 ******************************************************************************/

void
JXExprWidget::Print
	(
	JPagePrinter&	p,
	const JRect&	rect
	)
{
	EIPDraw(p);
}

/******************************************************************************
 PrivateDraw (virtual protected)

 ******************************************************************************/

void
JXExprWidget::PrivateDraw
	(
	JPainter&		p,
	const JRect&	rect
	)
{
	// adjust the origin so the expression is centered in Bounds

	JPoint delta;
	GetDrawingOffset(&delta);
	p.ShiftOrigin(delta);

	// draw the function

	EIPDraw(p);
}

/******************************************************************************
 GetDrawingOffset (private)

 ******************************************************************************/

void
JXExprWidget::GetDrawingOffset
	(
	JPoint* delta
	)
	const
{
	const JRect bounds = GetBounds();

	const JExprRectList* rectList = GetRectList();
	const JRect exprBounds = rectList->GetBoundsRect();

	delta->x = bounds.xcenter() - exprBounds.xcenter();
	delta->y = bounds.ycenter() - exprBounds.ycenter();
}

/******************************************************************************
 Bounds coords <-> Renderer coords (private)

 ******************************************************************************/

JPoint
JXExprWidget::BoundsToRenderer
	(
	const JPoint& pt
	)
	const
{
	JPoint origin;
	GetDrawingOffset(&origin);
	return (pt-origin);
}

JPoint
JXExprWidget::RendererToBounds
	(
	const JPoint& pt
	)
	const
{
	JPoint origin;
	GetDrawingOffset(&origin);
	return (pt+origin);
}

/******************************************************************************
 HandleFocusEvent (virtual protected)

 ******************************************************************************/

void
JXExprWidget::HandleFocusEvent()
{
	JXScrollableWidget::HandleFocusEvent();
	EIPActivate();
}

/******************************************************************************
 OKToUnfocus (virtual protected)

 ******************************************************************************/

JBoolean
JXExprWidget::OKToUnfocus()
{
	return JConvertToBoolean( JXScrollableWidget::OKToUnfocus() && EndEditing() );
}

/******************************************************************************
 HandleUnfocusEvent (virtual protected)

 ******************************************************************************/

void
JXExprWidget::HandleUnfocusEvent()
{
	JXScrollableWidget::HandleUnfocusEvent();
	EIPDeactivate();
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

void
JXExprWidget::HandleKeyPress
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	if (0 < key && key <= 255 &&
		!modifiers.meta() && !modifiers.control())
		{
		EIPHandleKeyPress(key);
		}
	else
		{
		JXScrollableWidget::HandleKeyPress(key, modifiers);
		}
}

/******************************************************************************
 EIPClipboardChanged (virtual protected)

 ******************************************************************************/

void
JXExprWidget::EIPClipboardChanged()
{
	const JFunction* f;
	if (GetClipboard(&f))
		{
		const JString text = f->Print();

		JXTextSelection* data = new JXTextSelection(GetDisplay(), text);
		assert( data != NULL );

		GetSelectionManager()->SetData(kJXClipboardName, data);
		}
}

/******************************************************************************
 EIPOwnsClipboard (virtual protected)

	We can't know if the current data is ours.

 ******************************************************************************/

JBoolean
JXExprWidget::EIPOwnsClipboard()
{
	return kJFalse;
}

/******************************************************************************
 EIPGetExternalClipboard (virtual protected)

	Returns kJTrue if there is something pasteable on the system clipboard.

 ******************************************************************************/

JBoolean
JXExprWidget::EIPGetExternalClipboard
	(
	JString* text
	)
{
	text->Clear();

	JBoolean gotData = kJFalse;
	JXSelectionManager* selManager = GetSelectionManager();

	JArray<Atom> typeList;
	if (selManager->GetAvailableTypes(kJXClipboardName, CurrentTime, &typeList))
		{
		JBoolean canGetText = kJFalse;
		Atom textType;

		const JSize typeCount = typeList.GetElementCount();
		for (JIndex i=1; i<=typeCount; i++)
			{
			Atom type = typeList.GetElement(i);
			if (type == XA_STRING ||
				(!canGetText && type == selManager->GetTextXAtom()))
				{
				canGetText = kJTrue;
				textType   = type;
				break;
				}
			}

		Atom returnType;
		unsigned char* data = NULL;
		JSize dataLength;
		JXSelectionManager::DeleteMethod delMethod;
		if (canGetText &&
			selManager->GetData(kJXClipboardName, CurrentTime, textType,
								&returnType, &data, &dataLength, &delMethod))
			{
			if (returnType == XA_STRING)
				{
				*text = JString(reinterpret_cast<JCharacter*>(data), dataLength);
				gotData = kJTrue;
				}
			selManager->DeleteData(&data, delMethod);
			}
		else
			{
			(JGetUserNotification())->ReportError(
				"Unable to paste the current contents of the X Clipboard.");
			}
		}
	else
		{
		(JGetUserNotification())->ReportError("The X Clipboard is empty.");
		}

	return gotData;
}

/******************************************************************************
 Receive (protected)

	Listen for menu update requests and menu selections.

 ******************************************************************************/

void
JXExprWidget::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	JXScrollableWidget::Receive(sender, message);
	JExprEditor::Receive(sender, message);
}
