/******************************************************************************
 test_jTime.cpp

	Program to test jTime.

	Written by John Lindal.

 *****************************************************************************/

#include "JTestManager.h"
#include "TestTable.h"
#include "JFloatTableData.h"
#include "JTableSelection.h"
#include "jAssert.h"

int main()
{
	return JTestManager::Execute();
}

JTEST(Table)
{
	TestTable t(10, 50);
	JTableSelection& s = t.GetTableSelection();
	JTableSelectionIterator ir(&s, JTableSelectionIterator::kIterateByRow);
	JTableSelectionIterator ic(&s);

	JAssertFalse(s.HasSelection());

	t.TestAppendRows(3);
	t.TestAppendCols(3);
	s.SelectAll();

	JAssertTrue(s.HasSelection());

	ir.MoveTo(JTableSelectionIterator::kStartBefore, 2,2);
	ic.MoveTo(JTableSelectionIterator::kStartBefore, 2,2);

	t.TestInsertRows(3, 1, 20);
	t.TestMoveRow(3, 2);

	t.TestInsertCols(3, 1, 100);
	t.TestMoveCol(3, 2);

	t.TestPrependRows(1);
	t.TestPrependCols(1);

	JAssertEqual(5, t.GetRowCount());
	JAssertEqual(10, t.GetRowHeight(1));
	JAssertEqual(10, t.GetRowHeight(2));
	JAssertEqual(20, t.GetRowHeight(3));
	JAssertEqual(10, t.GetRowHeight(4));
	JAssertEqual(10, t.GetRowHeight(5));

	JAssertTrue(t.RowIndexValid(5));
	JAssertFalse(t.RowIndexValid(6));

	JAssertEqual(5, t.GetColCount());
	JAssertEqual(50, t.GetColWidth(1));
	JAssertEqual(50, t.GetColWidth(2));
	JAssertEqual(100, t.GetColWidth(3));
	JAssertEqual(50, t.GetColWidth(4));
	JAssertEqual(50, t.GetColWidth(5));

	JAssertTrue(t.ColIndexValid(5));
	JAssertFalse(t.ColIndexValid(6));

	JAssertTrue(t.CellValid(JPoint(5,5)));
	JAssertFalse(t.CellValid(JPoint(5,6)));
	JAssertFalse(t.CellValid(JPoint(6,5)));

	t.SetRowHeight(4, 30);
	t.SetColWidth(4, 80);

	JAssertEqual(5, t.GetRowCount());
	JAssertEqual(10, t.GetRowHeight(1));
	JAssertEqual(10, t.GetRowHeight(2));
	JAssertEqual(20, t.GetRowHeight(3));
	JAssertEqual(30, t.GetRowHeight(4));
	JAssertEqual(10, t.GetRowHeight(5));

	JAssertEqual(5, t.GetColCount());
	JAssertEqual(50, t.GetColWidth(1));
	JAssertEqual(50, t.GetColWidth(2));
	JAssertEqual(100, t.GetColWidth(3));
	JAssertEqual(80, t.GetColWidth(4));
	JAssertEqual(50, t.GetColWidth(5));

	JPoint cell;
	JAssertTrue(t.GetCell(JPoint(150, 30), &cell));
	JAssertEqual(3, cell.x);
	JAssertEqual(3, cell.y);

	JRect r = t.GetCellRect(JPoint(4, 3));
	JAssertEqual(22, r.top);
	JAssertEqual(42, r.bottom);
	JAssertEqual(203, r.left);
	JAssertEqual(283, r.right);

	JAssertEqual(11, t.GetRowTop(2));
	JAssertEqual(51, t.GetColLeft(2));

	t.SetAllRowHeights(50);
	JAssertEqual(50, t.GetRowHeight(1));
	JAssertEqual(50, t.GetRowHeight(2));
	JAssertEqual(50, t.GetRowHeight(3));
	JAssertEqual(50, t.GetRowHeight(4));
	JAssertEqual(50, t.GetRowHeight(5));

	t.SetAllColWidths(50);
	JAssertEqual(50, t.GetColWidth(1));
	JAssertEqual(50, t.GetColWidth(2));
	JAssertEqual(50, t.GetColWidth(3));
	JAssertEqual(50, t.GetColWidth(4));
	JAssertEqual(50, t.GetColWidth(5));

	// F F F F F
	// F T F T T
	// F F F F F
	// F T F T T
	// F T F T T

	JAssertTrue(s.HasSelection());

	JIndex row, col;
	JAssertTrue(ir.Next(&row, &col));
	JAssertEqual(4, row);
	JAssertEqual(2, col);

	ir.MoveTo(JTableSelectionIterator::kStartAfter, 5,2);

	JAssertTrue(ir.Prev(&row, &col));
	JAssertEqual(5, row);
	JAssertEqual(2, col);

	JAssertTrue(ic.Next(&row, &col));
	JAssertEqual(2, row);
	JAssertEqual(4, col);

	ic.MoveTo(JTableSelectionIterator::kStartAfter, 5,2);

	JAssertTrue(ic.Prev(&row, &col));
	JAssertEqual(5, row);
	JAssertEqual(2, col);

	s.InvertRect(2,2, 3,3);

	// F F F F F
	// F F T F T
	// F T T T F
	// F F T F T
	// F T F T T

	ir.MoveTo(JTableSelectionIterator::kStartAtBeginning, 0,0);
	JAssertTrue(ir.AtBeginning());

	JAssertTrue(ir.Next(&row, &col));
	JAssertEqual(2, row);
	JAssertEqual(3, col);

	JAssertTrue(ir.Next(&row, &col));
	JAssertEqual(2, row);
	JAssertEqual(5, col);

	JAssertTrue(ir.Next(&row, &col));
	JAssertEqual(3, row);
	JAssertEqual(2, col);

	JAssertTrue(ir.Next(&row, &col));
	JAssertEqual(3, row);
	JAssertEqual(3, col);

	JAssertTrue(ir.Next(&row, &col));
	JAssertEqual(3, row);
	JAssertEqual(4, col);

	JAssertTrue(ir.Next(&row, &col));
	JAssertEqual(4, row);
	JAssertEqual(3, col);

	JAssertTrue(ir.Next(&row, &col));
	JAssertEqual(4, row);
	JAssertEqual(5, col);

	JAssertTrue(ir.Next(&row, &col));
	JAssertEqual(5, row);
	JAssertEqual(2, col);

	JAssertTrue(ir.Next(&row, &col));
	JAssertEqual(5, row);
	JAssertEqual(4, col);

	JAssertTrue(ir.Next(&cell));
	JAssertEqual(5, cell.y);
	JAssertEqual(5, cell.x);

	JAssertFalse(ir.Next(&row, &col));
	JAssertTrue(ir.AtEnd());
	JAssertFalse(ir.Next(&row, &col));

	ic.MoveTo(JTableSelectionIterator::kStartAtEnd, 0,0);
	JAssertTrue(ic.AtEnd());

	JAssertTrue(ic.Prev(&row, &col));
	JAssertEqual(5, row);
	JAssertEqual(5, col);

	JAssertTrue(ic.Prev(&row, &col));
	JAssertEqual(4, row);
	JAssertEqual(5, col);

	JAssertTrue(ic.Prev(&row, &col));
	JAssertEqual(2, row);
	JAssertEqual(5, col);

	JAssertTrue(ic.Prev(&row, &col));
	JAssertEqual(5, row);
	JAssertEqual(4, col);

	JAssertTrue(ic.Prev(&row, &col));
	JAssertEqual(3, row);
	JAssertEqual(4, col);

	JAssertTrue(ic.Prev(&row, &col));
	JAssertEqual(4, row);
	JAssertEqual(3, col);

	JAssertTrue(ic.Prev(&row, &col));
	JAssertEqual(3, row);
	JAssertEqual(3, col);

	JAssertTrue(ic.Prev(&row, &col));
	JAssertEqual(2, row);
	JAssertEqual(3, col);

	JAssertTrue(ic.Prev(&row, &col));
	JAssertEqual(5, row);
	JAssertEqual(2, col);

	JAssertTrue(ic.Prev(&cell));
	JAssertEqual(3, cell.y);
	JAssertEqual(2, cell.x);

	JAssertFalse(ic.Prev(&row, &col));
	JAssertTrue(ic.AtBeginning());
	JAssertFalse(ic.Prev(&row, &col));

	JAssertEqual(10, s.GetSelectedCellCount());

	s.InvertRow(5);
	s.InvertCol(5);

	JAssertEqual(10, s.GetSelectedCellCount());

	s.SelectRow(5);
	s.SelectCol(5);

	JAssertEqual(14, s.GetSelectedCellCount());

	s.SelectRect(JPoint(4,4), JPoint(5,5));

	JAssertEqual(15, s.GetSelectedCellCount());

	s.SelectRect(JRect(4,4,6,6), false);

	JAssertEqual(11, s.GetSelectedCellCount());

	s.SelectCell(1,3);
	s.SelectCell(JPoint(4,1));
	s.InvertCell(3,1);
	s.InvertCell(JPoint(1,4));

	JAssertTrue(s.IsSelected(1,4));
	JAssertTrue(s.IsSelected(JPoint(1,4)));

	JAssertTrue(s.GetFirstSelectedCell(&cell, JTableSelectionIterator::kIterateByRow));
	JAssertEqual(1, cell.y);
	JAssertEqual(3, cell.x);

	JAssertTrue(s.GetFirstSelectedCell(&cell, JTableSelectionIterator::kIterateByCol));
	JAssertEqual(3, cell.y);
	JAssertEqual(1, cell.x);

	JAssertTrue(s.GetLastSelectedCell(&cell, JTableSelectionIterator::kIterateByRow));
	JAssertEqual(5, cell.y);
	JAssertEqual(3, cell.x);

	JAssertTrue(s.GetLastSelectedCell(&cell, JTableSelectionIterator::kIterateByCol));
	JAssertEqual(3, cell.y);
	JAssertEqual(5, cell.x);

	JAssertFalse(s.GetSingleSelectedCell(&cell));

	t.SelectSingleCell(JPoint(3,2));

	JAssertTrue(s.GetSingleSelectedCell(&cell));
	JAssertEqual(2, cell.y);
	JAssertEqual(3, cell.x);

	t.TestRemoveRow(5);
	t.TestRemoveNextRows(3, 2);
	t.TestRemovePrevRows(2, 2);

	JAssertEqual(0, t.GetRowCount());

	t.TestRemoveCol(5);
	t.TestRemoveNextCols(3, 2);
	t.TestRemovePrevCols(2, 2);

	JAssertEqual(0, t.GetColCount());
}

JTEST(TableData)
{
	JFloatTableData d(0.0);

	JAssertTrue(d.IsEmpty());

	d.AppendCols(3);
	d.AppendRows(3);

	JAssertFalse(d.IsEmpty());

	TestTable t(10, 50);
	t.SetTableData(&d);

	d.InsertCols(3, 1);
	d.InsertRows(3, 1);

	d.PrependCols(1);
	d.PrependRows(1);

	JAssertEqual(5, t.GetRowCount());
	JAssertEqual(5, d.GetRowCount());
	JAssertTrue(d.RowIndexValid(5));
	JAssertFalse(d.RowIndexValid(6));

	JAssertEqual(5, t.GetColCount());
	JAssertEqual(5, d.GetColCount());
	JAssertTrue(d.ColIndexValid(5));
	JAssertFalse(d.ColIndexValid(6));

	JAssertTrue(d.CellValid(JPoint(5,5)));
	JAssertFalse(d.CellValid(JPoint(5,6)));
	JAssertFalse(d.CellValid(JPoint(6,5)));

	d.SetElement(2,2, 1.0);
	d.SetElement(JPoint(3,3), -1.0);

	JAssertEqual(1.0, d.GetElement(2,2));
	JAssertEqual(-1.0, d.GetElement(JPoint(3,3)));

	d.DuplicateRow(2,6);

	JAssertEqual(6, t.GetRowCount());
	JAssertEqual(6, d.GetRowCount());
	JAssertEqual(5, t.GetColCount());
	JAssertEqual(5, d.GetColCount());

	JAssertEqual(0.0, d.GetElement(6,1));
	JAssertEqual(1.0, d.GetElement(6,2));
	JAssertEqual(0.0, d.GetElement(6,3));
	JAssertEqual(0.0, d.GetElement(6,4));
	JAssertEqual(0.0, d.GetElement(6,5));

	d.DuplicateCol(3,6);

	JAssertEqual(6, t.GetRowCount());
	JAssertEqual(6, d.GetRowCount());
	JAssertEqual(6, t.GetColCount());
	JAssertEqual(6, d.GetColCount());

	JAssertEqual(0.0, d.GetElement(JPoint(6,1)));
	JAssertEqual(0.0, d.GetElement(JPoint(6,2)));
	JAssertEqual(-1.0, d.GetElement(JPoint(6,3)));
	JAssertEqual(0.0, d.GetElement(JPoint(6,4)));
	JAssertEqual(0.0, d.GetElement(JPoint(6,5)));
	JAssertEqual(0.0, d.GetElement(JPoint(6,6)));

	d.MoveRow(6,4);

	JAssertEqual(0.0, d.GetElement(4,1));
	JAssertEqual(1.0, d.GetElement(4,2));
	JAssertEqual(0.0, d.GetElement(4,3));
	JAssertEqual(0.0, d.GetElement(4,4));
	JAssertEqual(0.0, d.GetElement(4,5));
	JAssertEqual(0.0, d.GetElement(4,6));

	d.MoveCol(6,4);

	JAssertEqual(0.0, d.GetElement(JPoint(4,1)));
	JAssertEqual(0.0, d.GetElement(JPoint(4,2)));
	JAssertEqual(-1.0, d.GetElement(JPoint(4,3)));
	JAssertEqual(0.0, d.GetElement(JPoint(4,4)));
	JAssertEqual(0.0, d.GetElement(JPoint(4,5)));
	JAssertEqual(0.0, d.GetElement(JPoint(4,6)));

	d.RemoveRow(5);
	d.RemoveNextRows(3, 2);
	d.RemovePrevRows(2, 2);
	d.RemoveAllRows();

	JAssertEqual(0, t.GetRowCount());
	JAssertEqual(0, d.GetRowCount());

	d.RemoveCol(5);
	d.RemoveNextCols(3, 2);
	d.RemovePrevCols(2, 2);
	d.RemoveAllCols();

	JAssertEqual(0, t.GetColCount());
	JAssertEqual(0, d.GetColCount());
}
