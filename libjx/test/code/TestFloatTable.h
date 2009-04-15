/******************************************************************************
 TestFloatTable.h

	Interface for the TestFloatTable class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestFloatTable
#define _H_TestFloatTable

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXFloatTable.h>

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

	virtual ~TestFloatTable();

protected:

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
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

	// not allowed

	TestFloatTable(const TestFloatTable& source);
	const TestFloatTable& operator=(const TestFloatTable& source);
};

#endif
