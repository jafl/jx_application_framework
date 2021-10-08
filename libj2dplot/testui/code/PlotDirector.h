/******************************************************************************
 PlotDirector.h

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_PlotDirector
#define _H_PlotDirector

#include <jx-af/jx/JXWindowDirector.h>

class JXTextMenu;
class JX2DPlotWidget;
class JXPSPrinter;
class JX2DPlotEPSPrinter;

class PlotDirector : public JXWindowDirector
{
public:

	PlotDirector(JXDirector* supervisor);

	~PlotDirector() override;

	JX2DPlotWidget*	GetPlotWidget() const;

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

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
};


/******************************************************************************
 GetPlotWidget

 ******************************************************************************/

inline JX2DPlotWidget*
PlotDirector::GetPlotWidget()
	const
{
	return itsPlotWidget;
}

#endif
