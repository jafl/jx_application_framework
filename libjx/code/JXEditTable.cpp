/******************************************************************************
 JXEditTable.cpp

	We implement most of the routines required to use JTable's editing
	mechanism for in-place editing.  To use these routines, a derived class
	only needs to override JTable::ExtractInputData() and the following
	routines:

		CreateXInputField
			Create an input field to edit the specified cell and return
			the object as a JXInputField.

		PrepareDeleteXInputField
			Prepare for the active input field to be deleted.

	BASE CLASS = JXTable

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXEditTable.h>
#include <JXWindow.h>
#include <JXInputField.h>
#include <JXDeleteObjectTask.h>
#include <jXEventUtil.h>
#include <jXKeysym.h>
#include <JFontManager.h>
#include <jASCIIConstants.h>
#include <jAssert.h>

const JSize kMin1DVisCharCount = 5;
const JSize kDefault1DVisWidth = 10;

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXEditTable::JXEditTable
	(
	const JCoordinate	defRowHeight,
	const JCoordinate	defColWidth,
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXTable(defRowHeight, defColWidth, scrollbarSet,
			enclosure, hSizing, vSizing, x,y, w,h)
{
	itsInputField      = NULL;
	itsEditMenuHandler = NULL;
	WantInput(kJTrue, kJTrue);		// tab moves to next column
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXEditTable::~JXEditTable()
{
}

/******************************************************************************
 IsEditable (virtual)

	Derived classes should override this if there are some cells that
	cannot be edited.

 ******************************************************************************/

JBoolean
JXEditTable::IsEditable
	(
	const JPoint& cell
	)
	const
{
	return kJTrue;
}

/******************************************************************************
 CreateInputField (virtual protected)

 ******************************************************************************/

JBoolean
JXEditTable::CreateInputField
	(
	const JPoint&	cell,
	const JRect&	cellRect
	)
{
	itsInputField = CreateXInputField(cell, cellRect.left, cellRect.top,
									  cellRect.width(), cellRect.height());
	assert( itsInputField != NULL );
	itsInputField->SetTable(this);
	itsInputField->SetFocusColor(GetFocusColor());

	if (itsEditMenuHandler != NULL && itsEditMenuHandler->HasEditMenu())
		{
		itsInputField->ShareEditMenu(itsEditMenuHandler);
		}

	if (itsInputField->Focus())
		{
		return kJTrue;
		}
	else
		{
		DeleteInputField();
		return kJFalse;
		}
}

/******************************************************************************
 DeleteInputField (virtual protected)

	Deleting the input field during the event processing opens the
	possibility for very subtle bugs.  The one that I had to track down
	involved deactivating a table with an active edit field.
	JXTEBase::Deactivate() first calls JXWidget::Deactivate() which causes
	loss of focus, which deletes the input field.  JXTEBase then tries to
	perform additional processing on the deleted object!

 ******************************************************************************/

void
JXEditTable::DeleteInputField()
{
	if (itsInputField != NULL)
		{
		PrepareDeleteXInputField();
		itsInputField->Hide();
		JXDeleteObjectTask<JBroadcaster>::Delete(itsInputField);
		itsInputField = NULL;
		}
}

/******************************************************************************
 Adjusting the input field (virtual protected)

 ******************************************************************************/

void
JXEditTable::PlaceInputField
	(
	const JCoordinate x,
	const JCoordinate y
	)
{
	assert( itsInputField != NULL );
	itsInputField->Place(x,y);
}

void
JXEditTable::MoveInputField
	(
	const JCoordinate dx,
	const JCoordinate dy
	)
{
	assert( itsInputField != NULL );
	itsInputField->Move(dx,dy);
}

void
JXEditTable::SetInputFieldSize
	(
	const JCoordinate w,
	const JCoordinate h
	)
{
	assert( itsInputField != NULL );
	itsInputField->SetSize(w,h);
}

void
JXEditTable::ResizeInputField
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	assert( itsInputField != NULL );
	itsInputField->AdjustSize(dw,dh);
}

/******************************************************************************
 HandleKeyPress (virtual)

	Return - move down, TAB - move right, shift - opposite direction

	Meta: arrows or keypad numbers move in obvious directions

	Meta-Return: EndEditing()

 ******************************************************************************/

void
JXEditTable::HandleKeyPress
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	const JBoolean shiftOn = modifiers.shift();
	const JBoolean metaOn  = modifiers.meta();

	JXWindow* window = GetWindow();

	if (key == kJReturnKey && metaOn)
		{
		EndEditing();
		}
	else if (key == kJReturnKey && shiftOn)
		{
		ShiftEditing(0,-1);
		}
	else if (key == kJReturnKey)
		{
		ShiftEditing(0,1);
		}

	else if (key == kJTabKey && shiftOn)
		{
		ShiftEditing(-1,0);
		}
	else if (key == kJTabKey)
		{
		ShiftEditing(1,0);
		}

	else if (metaOn && (key == kJUpArrow || key == '8'))
		{
		ShiftEditing(0,-1);
		}
	else if (metaOn && (key == kJDownArrow || key == '2'))
		{
		ShiftEditing(0,1);
		}
	else if (metaOn && (key == kJLeftArrow || key == '4'))
		{
		ShiftEditing(-1,0);
		}
	else if (metaOn && (key == kJRightArrow || key == '6'))
		{
		ShiftEditing(1,0);
		}

	else if (key == kJEscapeKey)
		{
		CancelEditing();
		}

	else
		{
		JXTable::HandleKeyPress(key, modifiers);
		}
}

/******************************************************************************
 WantsInputFieldKey (virtual)

	Returns kJTrue if it wants to preempt the input field from getting the key.

 ******************************************************************************/

JBoolean
JXEditTable::WantsInputFieldKey
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
	const
{
	return JI2B(
		key == kJReturnKey || key == kJTabKey || key == kJEscapeKey ||
		(modifiers.meta() &&
		 (key == kJUpArrow || key == '8' ||
		  key == kJDownArrow || key == '2' ||
		  key == kJLeftArrow || key == '4' ||
		  key == kJRightArrow || key == '6')) ||

		// JXScrollableWidget

		(!JXTEBase::WillUseWindowsHomeEnd() &&
		 (key == XK_Home || key == XK_KP_Home ||
		  key == XK_End || key == XK_KP_End)) ||
		key == XK_Page_Up || key == XK_KP_Page_Up ||
		key == XK_Page_Down || key == XK_KP_Page_Down);
}

/******************************************************************************
 GetEditMenuHandler

	Returns JXTEBase* that can then be used for AppendEditMenu() or
	ShareEditMenu().

 ******************************************************************************/

JXTEBase*
JXEditTable::GetEditMenuHandler()
	const
{
	if (itsEditMenuHandler == NULL)
		{
		JXEditTable* me = const_cast<JXEditTable*>(this);
		me->itsEditMenuHandler =
			jnew JXInputField(me, JXWidget::kFixedLeft, JXWidget::kFixedTop,
							 0,0, 10,10);
		assert( itsEditMenuHandler != NULL );
		itsEditMenuHandler->Hide();
		}

	return itsEditMenuHandler;
}

/******************************************************************************
 GetMin1DVisibleWidth (virtual protected)

	We want the caret to be visible, if possible.

 ******************************************************************************/

JCoordinate
JXEditTable::GetMin1DVisibleWidth
	(
	const JPoint& cell
	)
	const
{
	JPoint editCell;
	if (GetEditedCell(&editCell) && editCell == cell)
		{
		assert( itsInputField != NULL );

		JIndex i;
		if (itsInputField->GetCaretLocation(&i))
			{
			return itsInputField->GetCharLeft(i);
			}
		else
			{
			return kMin1DVisCharCount *
				(itsInputField->GetDefaultFont()).GetCharWidth('W');
			}
		}

	return kDefault1DVisWidth;
}
