/******************************************************************************
 TestTable.h

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestTable
#define _H_TestTable

#include "JTable.h"

class TestTable : public JTable
{
public:

	TestTable(const JCoordinate defRowHeight, const JCoordinate defColWidth);

	~TestTable() override;

	void	TestInsertRows(const JIndex index, const JSize count, const JCoordinate rowHeight = 0);
	void	TestPrependRows(const JSize count, const JCoordinate rowHeight = 0);
	void	TestAppendRows(const JSize count, const JCoordinate rowHeight = 0);
	void	TestRemoveRow(const JIndex index);
	void	TestRemoveNextRows(const JIndex firstIndex, const JSize count);
	void	TestRemovePrevRows(const JIndex lastIndex, const JSize count);
	void	TestRemoveAllRows();
	void	TestMoveRow(const JIndex origIndex, const JIndex newIndex);

	void	TestInsertCols(const JIndex index, const JSize count, const JCoordinate colWidth = 0);
	void	TestPrependCols(const JSize count, const JCoordinate colWidth = 0);
	void	TestAppendCols(const JSize count, const JCoordinate colWidth = 0);
	void	TestRemoveCol(const JIndex index);
	void	TestRemoveNextCols(const JIndex firstIndex, const JSize count);
	void	TestRemovePrevCols(const JIndex lastIndex, const JSize count);
	void	TestRemoveAllCols();
	void	TestMoveCol(const JIndex origIndex, const JIndex newIndex);

	bool	TestPaginate(const JCoordinate pageWidth, const JCoordinate pageHeight,
						 const bool userPrintRowHeader,
						 JArray<JCoordinate>* rowBreakpts, bool* printRowHeader,
						 const bool userPrintColHeader,
						 JArray<JCoordinate>* colBreakpts, bool* printColHeader) const;

	bool	TestIsDraggingSelection() const;
	void	TestSetSelectionBehavior(const bool allowMultiple,
									 const bool allowDiscont);
	void	TestBeginSelectionDrag(const JPoint& cell, const bool extendSelection,
								   const bool selectDiscont);
	void	TestContinueSelectionDrag(const JPoint& cell);
	void	TestFinishSelectionDrag();
	bool	TestHandleSelectionKeyPress(const char key,
										const bool extendSelection);

protected:

	void	TableRefresh() override;
	void	TableRefreshRect(const JRect& rect) override;
	void	TableSetGUIBounds(const JCoordinate w, const JCoordinate h) override;
	void	TableSetScrollSteps(const JCoordinate hStep,
								const JCoordinate vStep) override;
	void	TableHeightChanged(const JCoordinate y, const JCoordinate delta) override;
	void	TableHeightScaled(const JFloat scaleFactor) override;
	void	TableRowMoved(const JCoordinate origY, const JSize height,
						  const JCoordinate newY) override;
	void	TableWidthChanged(const JCoordinate x, const JCoordinate delta) override;
	void	TableWidthScaled(const JFloat scaleFactor) override;
	void	TableColMoved(const JCoordinate origX, const JSize width,
						  const JCoordinate newX) override;
	bool	TableScrollToCellRect(const JRect& cellRect,
								  const bool centerInDisplay = false) override;
	JCoordinate	TableGetApertureWidth() const override;

	void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;
};

#endif
