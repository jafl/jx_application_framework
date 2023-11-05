/******************************************************************************
 TestStringTable.cpp

	BASE CLASS = JXStringTable

	Written by John Lindal.

 ******************************************************************************/

#include "TestStringTable.h"

#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXFontNameMenu.h>
#include <jx-af/jx/JXFontSizeMenu.h>
#include <jx-af/jx/JXStyleTableMenu.h>
#include <jx-af/jx/JXRowHeaderWidget.h>
#include <jx-af/jx/JXTEBase.h>
#include <jx-af/jx/jXGlobals.h>

#include <jx-af/jcore/JPagePrinter.h>
#include <jx-af/jcore/JStringTableData.h>
#include <jx-af/jcore/JTableSelection.h>
#include <jx-af/jcore/JString.h>
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
	"  | Row border"
	"%l| Insert column %r"
	"  | Duplicate column %r"
	"  | Move column %r"
	"  |Remove column %r"
	"  | Column border"
	"%l| Select cells %r"
	"  | Select rows %r"
	"  | Select columns %r"
	"%l| Test selection iterator by row"
	"  | Test selection iterator by col"
	"%l| Add 4000 rows";

// enum MouseAction is in header file

// Border width menu information

static const JUtf8Byte* kBorderWidthMenuStr =
	"0%r|1%r|2%r|3%r|4%r|5%r";

/******************************************************************************
 Constructor

 ******************************************************************************/

TestStringTable::TestStringTable
	(
	JStringTableData*	data,
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
	JXStringTable(data, scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h)
{
JIndex i,j;

	itsMouseAction = kInsertCol;
	itsRowHeader   = nullptr;

	GetEditMenuHandler()->AppendEditMenu(menuBar);

	itsTableMenu = menuBar->AppendTextMenu(JGetString("TableMenuTitle::TestStringTable"));
	itsTableMenu->SetMenuItems(kTableMenuStr);
	itsTableMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsTableMenu->AttachHandlers(this,
		&TestStringTable::UpdateTableMenu,
		&TestStringTable::HandleTableMenu);

	itsRowBorderMenu = jnew JXTextMenu(itsTableMenu, kChangeRowBorderWidthCmd, menuBar);
	itsRowBorderMenu->SetMenuItems(kBorderWidthMenuStr);
	itsRowBorderMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsRowBorderMenu->AttachHandlers(this,
		&TestStringTable::UpdateRowBorderMenu,
		&TestStringTable::HandleRowBorderMenu);

	itsColBorderMenu = jnew JXTextMenu(itsTableMenu, kChangeColBorderWidthCmd, menuBar);
	itsColBorderMenu->SetMenuItems(kBorderWidthMenuStr);
	itsColBorderMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsColBorderMenu->AttachHandlers(this,
		&TestStringTable::UpdateColBorderMenu,
		&TestStringTable::HandleColBorderMenu);

	std::function f = [this]()
	{
		SetFont(itsFontMenu->GetFontName(), itsSizeMenu->GetFontSize());
		if (itsRowHeader != nullptr)
		{
			itsRowHeader->TurnOnRowResizing(GetDefaultRowHeight());
		}
	};

	itsFontMenu = jnew JXFontNameMenu(JGetString("FontMenuTitle::TestStringTable"), true,
									  menuBar, kFixedLeft, kFixedTop, 0,0, 10,10);
	assert( itsFontMenu != nullptr );
	menuBar->AppendMenu(itsFontMenu);
	ListenTo(itsFontMenu, std::function([f](const JXFontNameMenu::NameChanged&)
	{
		f();
	}));

	itsSizeMenu = jnew JXFontSizeMenu(itsFontMenu, JGetString("SizeMenuTitle::TestStringTable"), 
									  menuBar, kFixedLeft, kFixedTop, 0,0, 10,10);
	assert( itsSizeMenu != nullptr );
	menuBar->AppendMenu(itsSizeMenu);
	ListenTo(itsSizeMenu, std::function([f](const JXFontSizeMenu::SizeChanged&)
	{
		f();
	}));

	itsStyleMenu =
		jnew JXStyleTableMenu(this, menuBar,
							 kFixedLeft, kFixedTop, 0,0, 10,10);
	menuBar->AppendMenu(itsStyleMenu);

	const JFont& font = GetFont();
	itsFontMenu->SetFontName(font.GetName());
	itsSizeMenu->SetFontSize(font.GetSize());

	data->AppendRows(kInitRowCount);
	data->AppendCols(kInitColCount);

	for (j=1; j<=kInitColCount; j++)
	{
		for (i=1; i<=kInitRowCount; i++)
		{
			const JString str = "(" + JString(i, 0) + "," + JString(j, 0) + ")";
			data->SetString(i,j, str);
		}
	}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TestStringTable::~TestStringTable()
{
}

/******************************************************************************
 TurnOnRowResizing

	This can't be done in the constructor because the JXRowHeaderWidget
	must be constructed after the table that it depends on.

 ******************************************************************************/

void
TestStringTable::TurnOnRowResizing
	(
	JXRowHeaderWidget* rowHeader
	)
{
	itsRowHeader = rowHeader;
	if (itsRowHeader != nullptr)
	{
		itsRowHeader->TurnOnRowResizing(GetDefaultRowHeight());
	}
}

/******************************************************************************
 HandleMouseDown

	This interface was designed to test the table classes.
	Please don't design real user iterfaces like this!

 ******************************************************************************/

void
TestStringTable::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	itsIsDraggingFlag = false;
	if (button > kJXRightButton)
	{
		ScrollForWheel(button, modifiers);
		return;
	}

	JPoint cell;
	if (!GetCell(pt, &cell))
	{
		return;
	}

	JStringTableData* data = GetStringData();
	JTableSelection& s     = GetTableSelection();

	if (button == kJXRightButton)	// middle button not easy on MacOS
	{
		BeginEditing(cell);
	}

	else if (itsMouseAction == kSelectCells)
	{
		if (button == kJXLeftButton && !modifiers.shift())
		{
			s.ClearSelection();
			s.SetBoat(cell);
			s.SetAnchor(cell);
			s.SelectCell(cell);
			itsIsDraggingFlag = true;
		}
		else
		{
			itsIsDraggingFlag = s.ExtendSelection(cell);
		}
	}

	else if (itsMouseAction == kSelectRows)
	{
		if (button == kJXLeftButton && !modifiers.shift())
		{
			s.ClearSelection();
		}
		s.SetBoat(JPoint(1, cell.y));
		s.SetAnchor(JPoint(GetColCount(), cell.y));
		s.SelectRow(cell.y);
	}

	else if (itsMouseAction == kSelectCols)
	{
		if (button == kJXLeftButton && !modifiers.shift())
		{
			s.ClearSelection();
		}
		s.SetBoat(JPoint(cell.x, 1));
		s.SetAnchor(JPoint(cell.x, GetRowCount()));
		s.SelectCol(cell.x);
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
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
TestStringTable::HandleMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsIsDraggingFlag)
	{
		ScrollForDrag(pt);

		JPoint cell;
		const bool ok = GetCell(JPinInRect(pt, GetBounds()), &cell);
		assert( ok );
		(GetTableSelection()).ExtendSelection(cell);
	}
}

/******************************************************************************
 UpdateTableMenu (private)

 ******************************************************************************/

void
TestStringTable::UpdateTableMenu()
{
	itsTableMenu->CheckItem(itsMouseAction);
}

/******************************************************************************
 HandleTableMenu (private)

 ******************************************************************************/

void
TestStringTable::HandleTableMenu
	(
	const JIndex index
	)
{
	if (index < kTestSelectionIteratorByRow)
	{
		itsMouseAction = static_cast<MouseAction>(index);
	}

	else if (index == kTestSelectionIteratorByRow ||
			 index == kTestSelectionIteratorByCol)
	{
		JTableSelectionIterator::Direction d = JTableSelectionIterator::kIterateByRow;
		if (index == kTestSelectionIteratorByCol)
		{
			d = JTableSelectionIterator::kIterateByCol;
		}

		JTableSelection& s = GetTableSelection();
		JTableSelectionIterator iter(&s, d);

		JPoint cell;

		std::cout << "Forward:" << std::endl;
		while (iter.Next(&cell))
		{
			std::cout << '(' << cell.y << ", " << cell.x << ')' << std::endl;
		}

		std::cout << std::endl;

		std::cout << "Backward:" << std::endl;
		while (iter.Prev(&cell))
		{
			std::cout << '(' << cell.y << ", " << cell.x << ')' << std::endl;
		}
	}

	else if (index == kAdd4000Rows)
	{
		// This is the most inefficient way to add a large amount of data to
		// a table because it calls Broadcast() for every single cell.
		// SetCol() would be the fastest in this case.  But it's a good stress test.

		JProgressDisplay* pg = JNewPG();
		pg->FixedLengthProcessBeginning(40, JGetString("Progress::TestStringTable"), true, true);

		JStringTableData* data   = GetStringData();
		const JSize origRowCount = GetRowCount();
		const JSize colCount     = GetColCount();

		data->AppendRows(4000);

		for (JIndex i=1; i<=4000; i++)
		{
			for (JIndex j=1; j<=colCount; j++)
			{
				const JString str = "(" + JString(i, 0) + "," + JString(j, 0) + ")";
				data->SetString(origRowCount+i, j, str);
			}

			if (i%100 == 0 && !pg->IncrementProgress())
			{
				break;
			}
		}

		pg->ProcessFinished();
		jdelete pg;
	}
}

/******************************************************************************
 UpdateRowBorderMenu (private)

 ******************************************************************************/

void
TestStringTable::UpdateRowBorderMenu()
{
	JCoordinate width;
	JColorID color;
	GetRowBorderInfo(&width, &color);
	itsRowBorderMenu->CheckItem(width+1);
}

/******************************************************************************
 HandleRowBorderMenu (private)

 ******************************************************************************/

void
TestStringTable::HandleRowBorderMenu
	(
	const JIndex index
	)
{
	JCoordinate width;
	JColorID color;
	GetRowBorderInfo(&width, &color);
	SetRowBorderInfo(index-1, color);
}

/******************************************************************************
 UpdateColBorderMenu (private)

 ******************************************************************************/

void
TestStringTable::UpdateColBorderMenu()
{
	JCoordinate width;
	JColorID color;
	GetColBorderInfo(&width, &color);
	itsColBorderMenu->CheckItem(width+1);
}

/******************************************************************************
 HandleColBorderMenu (private)

 ******************************************************************************/

void
TestStringTable::HandleColBorderMenu
	(
	const JIndex index
	)
{
	JCoordinate width;
	JColorID color;
	GetColBorderInfo(&width, &color);
	SetColBorderInfo(index-1, color);
}

/******************************************************************************
 Print header and footer (virtual protected)

 ******************************************************************************/

JCoordinate
TestStringTable::GetPrintHeaderHeight
	(
	JPagePrinter& p
	)
	const
{
	return p.GetLineHeight();
}

JCoordinate
TestStringTable::GetPrintFooterHeight
	(
	JPagePrinter& p
	)
	const
{
	return JRound(1.5 * p.GetLineHeight());
}

void
TestStringTable::DrawPrintHeader
	(
	JPagePrinter&		p,
	const JCoordinate	headerHeight
	)
{
	JRect pageRect = p.GetPageRect();
	p.String(pageRect.left, pageRect.top, JGetString("PageHeader::TestStringTable"));
	const JString dateStr = JGetTimeStamp();
	p.String(pageRect.left, pageRect.top, dateStr,
			 pageRect.width(), JPainter::HAlign::kRight);
}

void
TestStringTable::DrawPrintFooter
	(
	JPagePrinter&		p,
	const JCoordinate	footerHeight
	)
{
	JRect pageRect = p.GetPageRect();
	const JString pageNumberStr(p.GetPageIndex(), 0);

	const JUtf8Byte* map[] =
	{
		"page", pageNumberStr.GetBytes()
	};
	p.String(pageRect.left, pageRect.bottom - footerHeight,
			 JGetString("PageFooter::TestStringTable", map, sizeof(map)),
			 pageRect.width(), JPainter::HAlign::kCenter,
			 footerHeight, JPainter::VAlign::kBottom);
}
