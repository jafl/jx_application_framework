/******************************************************************************
 TestStringTable.h

	Interface for the TestStringTable class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestStringTable
#define _H_TestStringTable

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXStringTable.h>

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

	virtual ~TestStringTable();

	void	TurnOnRowResizing(JXRowHeaderWidget* rowHeader);

protected:

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);

	virtual JCoordinate	GetPrintHeaderHeight(JPagePrinter& p) const;
	virtual JCoordinate	GetPrintFooterHeight(JPagePrinter& p) const;
	virtual void		DrawPrintHeader(JPagePrinter& p, const JCoordinate headerHeight);
	virtual void		DrawPrintFooter(JPagePrinter& p, const JCoordinate footerHeight);

	virtual void	Receive(JBroadcaster* sender, const Message& message);

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
	JBoolean	itsIsDraggingFlag;

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

	// not allowed

	TestStringTable(const TestStringTable& source);
	const TestStringTable& operator=(const TestStringTable& source);
};

#endif
