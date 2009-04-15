/******************************************************************************
 TestFloatTableDirector.h

	Interface for the TestFloatTableDirector class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestFloatTableDirector
#define _H_TestFloatTableDirector

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWindowDirector.h>

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

	virtual ~TestFloatTableDirector();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

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

	// not allowed

	TestFloatTableDirector(const TestFloatTableDirector& source);
	const TestFloatTableDirector& operator=(const TestFloatTableDirector& source);
};

#endif
