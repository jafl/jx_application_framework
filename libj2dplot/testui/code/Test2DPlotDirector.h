/******************************************************************************
 Test2DPlotDirector.h

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_Test2DPlotDirector
#define _H_Test2DPlotDirector

#include <jx-af/jx/JXWindowDirector.h>

class JXTextMenu;
class JX2DPlotWidget;
class JXPSPrinter;
class JX2DPlotEPSPrinter;

class Test2DPlotDirector : public JXWindowDirector
{
public:

	Test2DPlotDirector(JXDirector* supervisor);

	virtual ~Test2DPlotDirector();

	JX2DPlotWidget*	GetPlotWidget() const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXTextMenu*	itsActionsMenu;

	JXPSPrinter*		itsPSPrinter;
	JX2DPlotEPSPrinter*	itsEPSPrinter;

// begin JXLayout

	JX2DPlotWidget* itsPlotWidget;

// end JXLayout

private:

	void	BuildWindow();

	void	UpdateActionsMenu();
	void	HandleActionsMenu(const JIndex index);

	// not allowed

	Test2DPlotDirector(const Test2DPlotDirector& source);
	const Test2DPlotDirector& operator=(const Test2DPlotDirector& source);
};


/******************************************************************************
 GetPlotWidget

 ******************************************************************************/

inline JX2DPlotWidget*
Test2DPlotDirector::GetPlotWidget()
	const
{
	return itsPlotWidget;
}

#endif
