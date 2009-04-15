/******************************************************************************
 TestSliderDirector.h

	Interface for the TestSliderDirector class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestSliderDirector
#define _H_TestSliderDirector

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWindowDirector.h>

class JXSlider;
class JXLevelControl;

class TestSliderDirector : public JXWindowDirector
{
public:

	TestSliderDirector(JXWindowDirector* supervisor);

	virtual ~TestSliderDirector();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

// begin JXLayout

    JXSlider*       itsMaxSlider;
    JXLevelControl* itsMaxLevel;
    JXSlider*       itsSlider;
    JXLevelControl* itsLevel;

// end JXLayout

private:

	void	BuildWindow();

	// not allowed

	TestSliderDirector(const TestSliderDirector& source);
	const TestSliderDirector& operator=(const TestSliderDirector& source);
};

#endif
