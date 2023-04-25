/******************************************************************************
 TestFloatTableDirector.h

	Interface for the TestFloatTableDirector class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestFloatTableDirector
#define _H_TestFloatTableDirector

#include <jx-af/jx/JXWindowDirector.h>

class JFloatTableData;
class TestFloatTable;

class JXTextMenu;
class JXFloatInput;
class JXRowHeaderWidget;
class JXColHeaderWidget;
class JXPSPrinter;

class TestFloatTableDirector : public JXWindowDirector
{
public:

	TestFloatTableDirector(JXDirector* supervisor);

	~TestFloatTableDirector() override;

private:

	JXPSPrinter*		itsPrinter;
	JFloatTableData*	itsData;
	JXTextMenu*			itsFileMenu;			// owned by the menu bar

// begin JXLayout


// end JXLayout

// begin tablelayout

	TestFloatTable*    itsTable;
	JXColHeaderWidget* itsColHeader;
	JXRowHeaderWidget* itsRowHeader;

// end tablelayout

private:

	void	BuildWindow();

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex item);
};

#endif
