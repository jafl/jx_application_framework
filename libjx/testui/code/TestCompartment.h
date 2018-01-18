/******************************************************************************
 TestCompartment.h

	Interface for the TestCompartment class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestCompartment
#define _H_TestCompartment

#include <JXDownRect.h>

class TestPartitionDirector;

class TestCompartment : public JXWidget
{
public:

	TestCompartment(TestPartitionDirector* director, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	virtual ~TestCompartment();

protected:

	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;
	virtual void	Draw(JXWindowPainter& p, const JRect& rect) override;

	virtual void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers) override;

	virtual JCoordinate$1GetFTCMinContentSize(const JBoolean horizontal) const override;

private:

	TestPartitionDirector*	itsDirector;

private:

	JBoolean	IsElastic() const;

	// not allowed

	TestCompartment(const TestCompartment& source);
	const TestCompartment& operator=(const TestCompartment& source);
};

#endif
