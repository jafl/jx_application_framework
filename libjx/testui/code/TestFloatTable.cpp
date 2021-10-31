/******************************************************************************
 TestFloatTable.cpp

	BASE CLASS = JXFloatTable

	Written by John Lindal.

 ******************************************************************************/

#include "TestFloatTable.h"

#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXFontSizeMenu.h>
#include <jx-af/jx/JXStyleTableMenu.h>
#include <jx-af/jx/JXTEBase.h>
#include <jx-af/jx/jXGlobals.h>

#include <jx-af/jcore/JPagePrinter.h>
#include <jx-af/jcore/JFloatTableData.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/JKLRand.h>
#include <jx-af/jcore/jTime.h>
#include <jx-af/jcore/jAssert.h>

const JSize kInitRowCount = 10;
const JSize kInitColCount = 3;

// Table menu information

static const JUtf8Byte* kTableMenuStr =
	"    Insert row %r"
	"  | Duplicate row %r"
	"  | Move row %r"
	"  | Remove row %r"
	"%l| Insert column %r"
	"  | Duplicate column %r"
	"  | Move column %r"
	"  | Remove column %r";

// enum MouseAction is in header file

/******************************************************************************
 Constructor

 ******************************************************************************/

TestFloatTable::TestFloatTable
	(
	JFloatTableData*	data,
	JXMenuBar*			menuBar,
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
	JXFloatTable(data, 4, scrollbarSet, enclosure,
				 hSizing,vSizing, x,y, w,h)
{
JIndex i,j;

	itsMouseAction = kInsertCol;

	GetEditMenuHandler()->AppendEditMenu(menuBar);

	itsTableMenu = menuBar->AppendTextMenu(JGetString("TableMenuTitle::TestFloatTable"));
	itsTableMenu->SetMenuItems(kTableMenuStr);
	itsTableMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsTableMenu);

	itsSizeMenu = jnew JXFontSizeMenu(JFontManager::GetDefaultFontName(), JGetString("SizeMenuTitle::TestFloatTable"),
									  menuBar, kFixedLeft, kFixedTop, 0,0, 10,10);
	assert( itsSizeMenu != nullptr );
	menuBar->AppendMenu(itsSizeMenu);
	ListenTo(itsSizeMenu);

	itsStyleMenu =
		jnew JXStyleTableMenu(this, menuBar,
							 kFixedLeft, kFixedTop, 0,0, 10,10);
	assert( itsStyleMenu != nullptr );
	menuBar->AppendMenu(itsStyleMenu);

	const JFont& font = GetFont();
	itsSizeMenu->SetFontName(font.GetName());
	itsSizeMenu->SetFontSize(font.GetSize());

	data->AppendRows(kInitRowCount);
	data->AppendCols(kInitColCount);

	JKLRand r;
	for (j=1; j<=kInitColCount; j++)
	{
		for (i=1; i<=kInitRowCount; i++)
		{
			data->SetElement(i,j, r.UniformClosedProb());
		}
	}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TestFloatTable::~TestFloatTable()
{
}

/******************************************************************************
 HandleMouseDown

	This interface was designed to test the table classes.
	Please don't design real user iterfaces like this!

 ******************************************************************************/

void
TestFloatTable::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint cell;
	if (button > kJXRightButton)
	{
		ScrollForWheel(button, modifiers);
		return;
	}
	else if (!GetCell(pt, &cell))
	{
		return;
	}

	JFloatTableData* data = GetFloatData();

	if (button == kJXRightButton)	// middle button not easy on MacOS
	{
		BeginEditing(cell);
	}

	// row manipulations

	else if (button == kJXLeftButton && itsMouseAction == kInsertRow)
	{
		data->InsertRows(cell.y, 1);
	}
	else if (button == kJXRightButton && itsMouseAction == kInsertRow)
	{
		data->InsertRows(cell.y+1, 1);
	}

	else if (button == kJXLeftButton && itsMouseAction == kDuplicateRow)
	{
		data->DuplicateRow(cell.y, cell.y);
	}
	else if (button == kJXRightButton && itsMouseAction == kDuplicateRow)
	{
		data->DuplicateRow(cell.y, cell.y+1);
	}

	else if (button == kJXLeftButton && itsMouseAction == kMoveRow &&
			 cell.y > 1)
	{
		data->MoveRow(cell.y, cell.y-1);
	}
	else if (button == kJXRightButton && itsMouseAction == kMoveRow &&
			 ((JIndex) cell.y) < GetRowCount())
	{
		data->MoveRow(cell.y, cell.y+1);
	}

	else if (itsMouseAction == kRemoveRow)
	{
		data->RemoveRow(cell.y);
	}

	// column manipulations

	else if (button == kJXLeftButton && itsMouseAction == kInsertCol)
	{
		data->InsertCols(cell.x, 1);
	}
	else if (button == kJXRightButton && itsMouseAction == kInsertCol)
	{
		data->InsertCols(cell.x+1, 1);
	}

	else if (button == kJXLeftButton && itsMouseAction == kDuplicateCol)
	{
		data->DuplicateCol(cell.x, cell.x);
	}
	else if (button == kJXRightButton && itsMouseAction == kDuplicateCol)
	{
		data->DuplicateCol(cell.x, cell.x+1);
	}

	else if (button == kJXLeftButton && itsMouseAction == kMoveCol &&
			 cell.x > 1)
	{
		data->MoveCol(cell.x, cell.x-1);
	}
	else if (button == kJXRightButton && itsMouseAction == kMoveCol &&
			 ((JIndex) cell.x) < GetColCount())
	{
		data->MoveCol(cell.x, cell.x+1);
	}

	else if (itsMouseAction == kRemoveCol)
	{
		data->RemoveCol(cell.x);
	}
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
TestFloatTable::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsTableMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdateTableMenu();
	}
	else if (sender == itsTableMenu && message.Is(JXMenu::kItemSelected))
	{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleTableMenu(selection->GetIndex());
	}

	else if (sender == itsSizeMenu && message.Is(JXFontSizeMenu::kSizeChanged))
	{
		SetFont(JFontManager::GetDefaultFontName(), itsSizeMenu->GetFontSize());
	}

	else
	{
		JXFloatTable::Receive(sender, message);
	}
}

/******************************************************************************
 UpdateTableMenu (private)

 ******************************************************************************/

void
TestFloatTable::UpdateTableMenu()
{
	itsTableMenu->CheckItem(itsMouseAction);
}

/******************************************************************************
 HandleTableMenu (private)

 ******************************************************************************/

void
TestFloatTable::HandleTableMenu
	(
	const JIndex index
	)
{
	itsMouseAction = static_cast<MouseAction>(index);
}

/******************************************************************************
 Print header and footer (virtual protected)

 ******************************************************************************/

JCoordinate
TestFloatTable::GetPrintHeaderHeight
	(
	JPagePrinter& p
	)
	const
{
	return p.GetLineHeight();
}

JCoordinate
TestFloatTable::GetPrintFooterHeight
	(
	JPagePrinter& p
	)
	const
{
	return JRound(1.5 * p.GetLineHeight());
}

void
TestFloatTable::DrawPrintHeader
	(
	JPagePrinter&		p,
	const JCoordinate	headerHeight
	)
{
	JRect pageRect = p.GetPageRect();
	p.String(pageRect.left, pageRect.top, JGetString("PageHeader::TestFloatTable"));
	const JString dateStr = JGetTimeStamp();
	p.String(pageRect.left, pageRect.top, dateStr,
			 pageRect.width(), JPainter::kHAlignRight);
}

void
TestFloatTable::DrawPrintFooter
	(
	JPagePrinter&		p,
	const JCoordinate	footerHeight
	)
{
	JRect pageRect = p.GetPageRect();
	const JString pageNumberStr = JString(p.GetPageIndex(), 0);

	const JUtf8Byte* map[] =
	{
		"page", pageNumberStr.GetBytes()
	};
	p.String(pageRect.left, pageRect.bottom - footerHeight,
			 JGetString("PageFooter::TestFloatTable", map, sizeof(map)),
			 pageRect.width(), JPainter::kHAlignCenter,
			 footerHeight, JPainter::kVAlignBottom);
}
