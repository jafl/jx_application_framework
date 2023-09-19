/******************************************************************************
 test_jTime.cpp

	Program to test jTime.

	Written by John Lindal.

 *****************************************************************************/

#include "JTestManager.h"
#include "TestTable.h"
#include "jAssert.h"

int main()
{
	return JTestManager::Execute();
}

JTEST(Table)
{
	TestTable t(10, 50);

	t.TestAppendRows(3);
	t.TestAppendCols(3);

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

	t.TestRemoveRow(5);
	t.TestRemoveNextRows(3, 2);
	t.TestRemovePrevRows(2, 2);

	JAssertEqual(0, t.GetRowCount());

	t.TestRemoveCol(5);
	t.TestRemoveNextCols(3, 2);
	t.TestRemovePrevCols(2, 2);

	JAssertEqual(0, t.GetColCount());
}
