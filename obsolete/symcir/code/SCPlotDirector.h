/******************************************************************************
 SCPlotDirector.h

	Interface for the SCPlotDirector class

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_SCPlotDirector
#define _H_SCPlotDirector

#include <JXWindowDirector.h>

class JFunction;
class JX2DPlotWidget;
class SCCircuitDocument;

class SCPlotDirector : public JXWindowDirector
{
public:

	SCPlotDirector(SCCircuitDocument* supervisor);
	SCPlotDirector(std::istream& input, const JFileVersion vers,
				   SCCircuitDocument* supervisor);

	virtual ~SCPlotDirector();

	void	AddFunction(const JFunction& f, const JCharacter* name,
						const JFloat xMin, const JFloat xMax);

	void	StreamOut(std::ostream& output) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	SCCircuitDocument*	itsDoc;		// owns us

// begin JXLayout

	JX2DPlotWidget* itsPlot;

// end JXLayout

private:

	void	BuildWindow();
	void	AddFunction(JFunction* f, const JCharacter* name,
						const JFloat xMin, const JFloat xMax);

	// not allowed

	SCPlotDirector(const SCPlotDirector& source);
	const SCPlotDirector& operator=(const SCPlotDirector& source);
};

#endif
