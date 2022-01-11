/******************************************************************************
 TestImageWidget.h

	Interface for the TestImageWidget class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestWidget
#define _H_TestWidget

#include <jx-af/jx/JXImageWidget.h>
#include <jx-af/jx/JXImageSelection.h>
#include <jx-af/jcore/JKLRand.h>

class TestImageDirector;

class TestImageWidget : public JXImageWidget
{
public:

	TestImageWidget(TestImageDirector* director,
					JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

protected:

	bool	WillAcceptDrop(const JArray<Atom>& typeList, Atom* action,
						   const JPoint& pt, const Time time,
						   const JXWidget* source) override;
	void	HandleDNDDrop(const JPoint& pt, const JArray<Atom>& typeList,
						  const Atom action, const Time time,
						  const JXWidget* source) override;

private:

	TestImageDirector*	itsDirector;	// owns us

	Atom	itsAtoms[ JXImageSelection::kAtomCount ];
};

#endif
