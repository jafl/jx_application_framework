/******************************************************************************
 TestSliderDirector.h

	Interface for the TestSliderDirector class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestSliderDirector
#define _H_TestSliderDirector

#include <JXWindowDirector.h>

class JXSlider;
class JXLevelControl;

class TestSliderDirector : public JXWindowDirector
{
public:

	TestSliderDirector(JXWindowDirector* supervisor);

	virtual ~TestSliderDirector();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

// begin JXLayout

	JXSlider*       itsMaxSlider;
	JXLevelControl* itsMaxLevel;
	JXSlider*       itsSlider;
	JXLevelControl* itsLevel;

// end JXLayout

private:

	void	BuildWindow();
};

#endif
