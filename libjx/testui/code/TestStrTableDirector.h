/******************************************************************************
 TestStrTableDirector.h

	Interface for the TestStrTableDirector class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestStrTableDirector
#define _H_TestStrTableDirector

#include <jx-af/jx/JXWindowDirector.h>

class JStringTableData;
class TestStringTable;

class JXTextMenu;
class JXRowHeaderWidget;
class JXColHeaderWidget;
class JXPSPrinter;

class TestStrTableDirector : public JXWindowDirector
{
public:

	TestStrTableDirector(JXDirector* supervisor);

	virtual ~TestStrTableDirector();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXPSPrinter*		itsPrinter;
	JStringTableData*	itsData;
	JXTextMenu*			itsFileMenu;			// owned by the menu bar

// begin JXLayout


// end JXLayout

// begin tablelayout

	TestStringTable*   itsTable;
	JXColHeaderWidget* itsColHeader;
	JXRowHeaderWidget* itsRowHeader;

// end tablelayout

private:

	void	BuildWindow();

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex item);
};

#endif
