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

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXEditTable.h"
#include "JXWindow.h"
#include "JXInputField.h"
#include "JXDeleteObjectTask.h"
#include "jXEventUtil.h"
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
	itsInputField      = nullptr;
	itsEditMenuHandler = nullptr;
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
	assert( itsInputField != nullptr );
	itsInputField->SetTable(this);
	itsInputField->SetFocusColor(GetFocusColor());

	if (itsEditMenuHandler != nullptr && itsEditMenuHandler->HasEditMenu())
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
	if (itsInputField != nullptr)
		{
		PrepareDeleteXInputField();
		itsInputField->Hide();
		JXDeleteObjectTask<JBroadcaster>::Delete(itsInputField);
		itsInputField = nullptr;
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
	assert( itsInputField != nullptr );
	itsInputField->Place(x,y);
}

void
JXEditTable::MoveInputField
	(
	const JCoordinate dx,
	const JCoordinate dy
	)
{
	assert( itsInputField != nullptr );
	itsInputField->Move(dx,dy);
}

void
JXEditTable::SetInputFieldSize
	(
	const JCoordinate w,
	const JCoordinate h
	)
{
	assert( itsInputField != nullptr );
	itsInputField->SetSize(w,h);
}

void
JXEditTable::ResizeInputField
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	assert( itsInputField != nullptr );
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
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	const JBoolean shiftOn = modifiers.shift();
	const JBoolean metaOn  = modifiers.meta();

	if (c == kJReturnKey && metaOn)
		{
		EndEditing();
		}
	else if (c == kJReturnKey && shiftOn)
		{
		ShiftEditing(0,-1);
		}
	else if (c == kJReturnKey)
		{
		ShiftEditing(0,1);
		}

	else if (c == kJTabKey && shiftOn)
		{
		ShiftEditing(-1,0);
		}
	else if (c == kJTabKey)
		{
		ShiftEditing(1,0);
		}

	else if (metaOn && (c == kJUpArrow || c == '8'))
		{
		ShiftEditing(0,-1);
		}
	else if (metaOn && (c == kJDownArrow || c == '2'))
		{
		ShiftEditing(0,1);
		}
	else if (metaOn && (c == kJLeftArrow || c == '4'))
		{
		ShiftEditing(-1,0);
		}
	else if (metaOn && (c == kJRightArrow || c == '6'))
		{
		ShiftEditing(1,0);
		}

	else if (c == kJEscapeKey)
		{
		CancelEditing();
		}

	else
		{
		JXTable::HandleKeyPress(c, keySym, modifiers);
		}
}

/******************************************************************************
 WantsInputFieldKey (virtual)

	Returns kJTrue if it wants to preempt the input field from getting the key.

 ******************************************************************************/

JBoolean
JXEditTable::WantsInputFieldKey
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
	const
{
	return JI2B(
		c == kJReturnKey || c == kJTabKey || c == kJEscapeKey ||
		(modifiers.meta() &&
		 (c == kJUpArrow || c == '8' ||
		  c == kJDownArrow || c == '2' ||
		  c == kJLeftArrow || c == '4' ||
		  c == kJRightArrow || c == '6')) ||

		// JXScrollableWidget

		(!JXTEBase::WillUseWindowsHomeEnd() &&
		 (keySym == XK_Home || keySym == XK_KP_Home ||
		  keySym == XK_End || keySym == XK_KP_End)) ||
		keySym == XK_Page_Up || keySym == XK_KP_Page_Up ||
		keySym == XK_Page_Down || keySym == XK_KP_Page_Down);
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
	if (itsEditMenuHandler == nullptr)
		{
		JXEditTable* me = const_cast<JXEditTable*>(this);
		me->itsEditMenuHandler =
			jnew JXInputField(me, JXWidget::kFixedLeft, JXWidget::kFixedTop,
							 0,0, 10,10);
		assert( itsEditMenuHandler != nullptr );
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
		assert( itsInputField != nullptr );

		JIndex i;
		if (itsInputField->GetCaretLocation(&i))
			{
			return itsInputField->GetCharLeft(i);
			}
		else
			{
			return kMin1DVisCharCount *
				(itsInputField->GetText()->GetDefaultFont()).GetCharWidth(GetFontManager(), JUtf8Character('W'));
			}
		}

	return kDefault1DVisWidth;
}
