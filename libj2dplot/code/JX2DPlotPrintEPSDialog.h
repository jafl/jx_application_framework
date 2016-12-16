/******************************************************************************
 JX2DPlotPrintEPSDialog.h

	Copyright (C) 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JX2DPlotPrintEPSDialog
#define _H_JX2DPlotPrintEPSDialog

#include <JXEPSPrintSetupDialog.h>

class JXTextMenu;
class JXFloatInput;

class JX2DPlotPrintEPSDialog : public JXEPSPrintSetupDialog
{
public:

	enum Unit
	{
		kPixels = 0,	// internal use only
		kMillimeters,	// order on menu
		kCentimeters,
		kInches
	};

public:

	static JX2DPlotPrintEPSDialog*
		Create(const JCharacter* fileName,
			   const JBoolean printPreview, const JBoolean bw,
			   const JCoordinate w, const JCoordinate h, const Unit unit);

	virtual ~JX2DPlotPrintEPSDialog();

	void	GetPlotSize(JCoordinate* w, JCoordinate* h, Unit* unit);

protected:

	JX2DPlotPrintEPSDialog();

	virtual JBoolean	OKToDeactivate();
	virtual void		Receive(JBroadcaster* sender, const Message& message);

private:

	Unit	itsUnit;

// begin JXLayout

	JXFloatInput* itsWidthInput;
	JXFloatInput* itsHeightInput;
	JXTextMenu*   itsUnitMenu;
	JXTextMenu*   itsPredefSizeMenu;

// end JXLayout

private:

	void	BuildWindow(const JCharacter* fileName,
						const JBoolean printPreview, const JBoolean bw,
						const JCoordinate w, const JCoordinate h,
						const Unit unit);

	void	HandlePredefSizeMenu(const JIndex index);

	void	UpdateUnitMenu();
	void	HandleUnitMenu(const JIndex index);
	void	UpdateSize(const Unit origUnit, const Unit newUnit,
					   JXFloatInput* input);

	// not allowed

	JX2DPlotPrintEPSDialog(const JX2DPlotPrintEPSDialog& source);
	const JX2DPlotPrintEPSDialog& operator=(const JX2DPlotPrintEPSDialog& source);
};

std::istream& operator>>(std::istream& input, JX2DPlotPrintEPSDialog::Unit& u);
std::ostream& operator<<(std::ostream& output, const JX2DPlotPrintEPSDialog::Unit u);

#endif
