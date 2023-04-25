/******************************************************************************
 TestStringTable.h

	Interface for the TestStringTable class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestStringTable
#define _H_TestStringTable

#include <jx-af/jx/JXStringTable.h>

class JXMenuBar;
class JXTextMenu;
class JXFontNameMenu;
class JXFontSizeMenu;
class JXStyleTableMenu;
class JXRowHeaderWidget;

class TestStringTable : public JXStringTable
{
public:

	TestStringTable(JStringTableData* data, JXMenuBar* menuBar,
					JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	~TestStringTable() override;

	void	TurnOnRowResizing(JXRowHeaderWidget* rowHeader);

protected:

	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
							const JSize clickCount,
							const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;
	void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;

	JCoordinate	GetPrintHeaderHeight(JPagePrinter& p) const override;
	JCoordinate	GetPrintFooterHeight(JPagePrinter& p) const override;
	void		DrawPrintHeader(JPagePrinter& p, const JCoordinate headerHeight) override;
	void		DrawPrintFooter(JPagePrinter& p, const JCoordinate footerHeight) override;

private:

	enum MouseAction
	{
		kInsertRow = 1,
		kDuplicateRow,
		kMoveRow,
		kRemoveRow,
		kChangeRowBorderWidthCmd,		// place holder

		kInsertCol,
		kDuplicateCol,
		kMoveCol,
		kRemoveCol,
		kChangeColBorderWidthCmd,		// place holder

		kSelectCells,
		kSelectRows,
		kSelectCols,
		kTestSelectionIteratorByRow,	// place holder
		kTestSelectionIteratorByCol,	// place holder
		kAdd4000Rows					// place holder
	};

private:

	MouseAction	itsMouseAction;
	bool		itsIsDraggingFlag;

	JXRowHeaderWidget*	itsRowHeader;		// owned by director

	// owned by the menu bar

	JXTextMenu*		itsTableMenu;
	JXTextMenu*		itsRowBorderMenu;
	JXTextMenu*		itsColBorderMenu;

	JXFontNameMenu*		itsFontMenu;
	JXFontSizeMenu*		itsSizeMenu;
	JXStyleTableMenu*	itsStyleMenu;

private:

	void	UpdateTableMenu();
	void	HandleTableMenu(const JIndex item);

	void	UpdateRowBorderMenu();
	void	HandleRowBorderMenu(const JIndex item);

	void	UpdateColBorderMenu();
	void	HandleColBorderMenu(const JIndex item);
};

#endif
