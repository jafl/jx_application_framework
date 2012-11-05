/******************************************************************************
 CMArray2DTable.cpp

	BASE CLASS = JXStringTable

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cmStdInc.h>
#include "CMArray2DTable.h"
#include "CMArray2DDir.h"
#include "CMCommandDirector.h"
#include "cmGlobals.h"
#include <JXInputField.h>
#include <JXTextMenu.h>
#include <JXSelectionManager.h>
#include <JXTextSelection.h>
#include <JStringTableData.h>
#include <JTableSelection.h>
#include <JPagePrinter.h>
#include <jASCIIConstants.h>
#include <jTime.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CMArray2DTable::CMArray2DTable
	(
	CMCommandDirector*	cmdDir,
	CMArray2DDir*		arrayDir,
	JXMenuBar*			menuBar,
	JStringTableData*	data,
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
	JXStringTable(data, scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h),
	itsCmdDir(cmdDir),
	itsArrayDir(arrayDir)
{
	SetSelectionBehavior(kJFalse, kJFalse);

	JXTEBase* te = GetEditMenuHandler();
	itsEditMenu  = te->AppendEditMenu(menuBar);
	ListenTo(itsEditMenu);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMArray2DTable::~CMArray2DTable()
{
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
CMArray2DTable::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JTableSelection& s = GetTableSelection();

	JPoint cell;
	if (ScrollForWheel(button, modifiers))
		{
		// work has been done
		}
	else if (!GetCell(pt, &cell))
		{
		s.ClearSelection();
		}
	else if (button == kJXLeftButton && modifiers.meta())
		{
		if (!s.IsSelected(cell))
			{
			SelectSingleCell(cell, kJFalse);
			}

		const JString expr = itsArrayDir->GetExpression(cell);
		itsCmdDir->DisplayExpression(expr);
		}
	else if (button == kJXLeftButton && clickCount == 2)
		{
		BeginEditing(cell);
		}
	else if (button == kJXLeftButton && clickCount == 1)
		{
		SelectSingleCell(cell, kJFalse);
		}
}

/******************************************************************************
 HandleKeyPress (virtual protected)

 ******************************************************************************/

void
CMArray2DTable::HandleKeyPress
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	if (key == kJReturnKey)
		{
		JPoint cell;
		if (!IsEditing() && (GetTableSelection()).GetSingleSelectedCell(&cell))
			{
			BeginEditing(cell);
			}
		else
			{
			EndEditing();
			}
		}

	else if (!IsEditing() && HandleSelectionKeyPress(key, modifiers))
		{
		// work has been done
		}

	else
		{
		JXStringTable::HandleKeyPress(key, modifiers);
		}
}

/******************************************************************************
 CreateXInputField (virtual protected)

 ******************************************************************************/

JXInputField*
CMArray2DTable::CreateXInputField
	(
	const JPoint&		cell,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	JXInputField* input = JXStringTable::CreateXInputField(cell, x,y, w,h);
	input->SetIsRequired();

	itsOrigEditValue = input->GetText();

	return input;
}

/******************************************************************************
 ExtractInputData

 ******************************************************************************/

JBoolean
CMArray2DTable::ExtractInputData
	(
	const JPoint& cell
	)
{
	JXInputField* input = NULL;
	const JBoolean ok = GetXInputField(&input);
	assert( ok );
	const JString& text = input->GetText();

	if (!text.IsEmpty())
		{
		if (text != itsOrigEditValue)
			{
			const JString name = itsArrayDir->GetExpression(cell);
			(CMGetLink())->SetValue(name, text);
			}
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 Print header (virtual protected)

 ******************************************************************************/

JCoordinate
CMArray2DTable::GetPrintHeaderHeight
	(
	JPagePrinter& p
	)
	const
{
	return p.GetLineHeight();
}

void
CMArray2DTable::DrawPrintHeader
	(
	JPagePrinter&		p,
	const JCoordinate	headerHeight
	)
{
	JRect pageRect = p.GetPageRect();
	p.String(pageRect.left, pageRect.top, itsArrayDir->GetExpression());

	const JString dateStr = JGetTimeStamp();
	p.String(pageRect.left, pageRect.top, dateStr,
			 pageRect.width(), JPainter::kHAlignRight);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CMArray2DTable::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsEditMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		if (HasFocus())
			{
			UpdateEditMenu();
			}
		}
	else if (sender == itsEditMenu && message.Is(JXMenu::kItemSelected))
		{
		if (HasFocus())
			{
			const JXMenu::ItemSelected* selection =
				dynamic_cast<const JXMenu::ItemSelected*>(&message);
			assert( selection != NULL );
			HandleEditMenu(selection->GetIndex());
			}
		}

	else
		{
		JXStringTable::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateEditMenu (private)

 ******************************************************************************/

void
CMArray2DTable::UpdateEditMenu()
{
	JXTEBase* te = GetEditMenuHandler();

	JIndex index;
	if ((GetTableSelection()).HasSelection() &&
		te->EditMenuCmdToIndex(JTextEditor::kCopyCmd, &index))
		{
		itsEditMenu->EnableItem(index);
		}
}

/******************************************************************************
 HandleEditMenu (private)

 ******************************************************************************/

void
CMArray2DTable::HandleEditMenu
	(
	const JIndex index
	)
{
	JTextEditor::CmdIndex cmd;
	JPoint cell;
	if ((GetEditMenuHandler())->EditMenuIndexToCmd(index, &cmd) &&
		cmd == JTextEditor::kCopyCmd &&
		(GetTableSelection()).GetSingleSelectedCell(&cell))
		{
		JXTextSelection* data =
			new JXTextSelection(GetDisplay(), (GetStringData())->GetString(cell));
		assert( data != NULL );

		(GetSelectionManager())->SetData(kJXClipboardName, data);
		}
}
