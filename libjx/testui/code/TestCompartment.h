/******************************************************************************
 TestCompartment.h

	Interface for the TestCompartment class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestCompartment
#define _H_TestCompartment

#include <jx-af/jx/JXDownRect.h>

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

	void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;
	void	Draw(JXWindowPainter& p, const JRect& rect) override;

	void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers) override;

	JCoordinate	GetFTCMinContentSize(const bool horizontal) const override;

private:

	TestPartitionDirector*	itsDirector;

private:

	bool	IsElastic() const;
};

#endif
