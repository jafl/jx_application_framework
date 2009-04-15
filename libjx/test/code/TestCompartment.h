/******************************************************************************
 TestCompartment.h

	Interface for the TestCompartment class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestCompartment
#define _H_TestCompartment

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDownRect.h>

class TestPartitionDirector;

class TestCompartment : public JXDownRect
{
public:

	TestCompartment(TestPartitionDirector* director, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	virtual ~TestCompartment();

protected:

	virtual void	Draw(JXWindowPainter& p, const JRect& rect);

	virtual void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers);

private:

	TestPartitionDirector*	itsDirector;

private:

	// not allowed

	TestCompartment(const TestCompartment& source);
	const TestCompartment& operator=(const TestCompartment& source);
};

#endif
