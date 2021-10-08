/******************************************************************************
 TestFloatTable.h

	Interface for the TestFloatTable class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestFloatTable
#define _H_TestFloatTable

#include <jx-af/jx/JXFloatTable.h>

class JFloatTableData;
class JXMenuBar;
class JXTextMenu;
class JXFontSizeMenu;
class JXStyleTableMenu;

class TestFloatTable : public JXFloatTable
{
public:

	TestFloatTable(JFloatTableData* data, JXMenuBar* menuBar,
				   JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	~TestFloatTable() override;

protected:

	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
							const JSize clickCount,
							const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;

	JCoordinate	GetPrintHeaderHeight(JPagePrinter& p) const override;
	JCoordinate	GetPrintFooterHeight(JPagePrinter& p) const override;
	void		DrawPrintHeader(JPagePrinter& p, const JCoordinate headerHeight) override;
	void		DrawPrintFooter(JPagePrinter& p, const JCoordinate footerHeight) override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	enum MouseAction
	{
		kInsertRow = 1,
		kDuplicateRow,
		kMoveRow,
		kRemoveRow,

		kInsertCol,
		kDuplicateCol,
		kMoveCol,
		kRemoveCol
	};

private:

	MouseAction	itsMouseAction;

	// owned by the menu bar

	JXTextMenu*			itsTableMenu;
	JXFontSizeMenu*		itsSizeMenu;
	JXStyleTableMenu*	itsStyleMenu;

private:

	void	UpdateTableMenu();
	void	HandleTableMenu(const JIndex item);
};

#endif
