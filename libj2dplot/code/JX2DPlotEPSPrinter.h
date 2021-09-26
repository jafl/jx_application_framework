/******************************************************************************
 JX2DPlotEPSPrinter.h

	Interface for the JX2DPlotEPSPrinter class

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_JX2DPlotEPSPrinter
#define _H_JX2DPlotEPSPrinter

#include <jx-af/jx/JXEPSPrinter.h>
#include "jx-af/j2dplot/JX2DPlotPrintEPSDialog.h"		// need def of Unit

class JX2DPlotEPSPrinter : public JXEPSPrinter
{
public:

	JX2DPlotEPSPrinter(JXDisplay* display);

	virtual ~JX2DPlotEPSPrinter();

	void	UsePlotSetup(const bool use);

	JRect	GetPlotBounds() const;
	void	SetPlotBounds(const JRect& rect);

	// saving setup information

	void	ReadX2DEPSSetup(std::istream& input);
	void	WriteX2DEPSSetup(std::ostream& output) const;

protected:

	virtual JXEPSPrintSetupDialog*
		CreatePrintSetupDialog(const JString& fileName,
							   const bool preview, const bool bw);

	virtual bool	EndUserPrintSetup(const JBroadcaster::Message& message,
										  bool* changed);

private:

	bool	itsUsePlotSetupFlag;
	JCoordinate	itsPlotWidth;
	JCoordinate	itsPlotHeight;

	JX2DPlotPrintEPSDialog::Unit	itsUnit;

	JX2DPlotPrintEPSDialog*	itsPlotSetupDialog;
};


/******************************************************************************
 UsePlotSetup

	Call this to toggle between the default setup dialog and the
	customized setup dialog.

 ******************************************************************************/

inline void
JX2DPlotEPSPrinter::UsePlotSetup
	(
	const bool use
	)
{
	itsUsePlotSetupFlag = use;
}

/******************************************************************************
 Plot bounds

 ******************************************************************************/

inline JRect
JX2DPlotEPSPrinter::GetPlotBounds()
	const
{
	return JRect(0, 0, itsPlotHeight, itsPlotWidth);
}

inline void
JX2DPlotEPSPrinter::SetPlotBounds
	(
	const JRect& rect
	)
{
	itsPlotWidth  = rect.width();
	itsPlotHeight = rect.height();
}

#endif
