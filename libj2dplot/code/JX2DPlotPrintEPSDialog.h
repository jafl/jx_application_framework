/******************************************************************************
 JX2DPlotPrintEPSDialog.h

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_JX2DPlotPrintEPSDialog
#define _H_JX2DPlotPrintEPSDialog

#include <jx-af/jx/JXEPSPrintSetupDialog.h>

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
		Create(const JString& fileName,
			   const bool printPreview, const bool bw,
			   const JCoordinate w, const JCoordinate h, const Unit unit);

	~JX2DPlotPrintEPSDialog() override;

	bool	SetParameters(JXEPSPrinter* p) const override;

protected:

	JX2DPlotPrintEPSDialog();

	bool	OKToDeactivate() override;

private:

	Unit	itsUnit;

// begin JXLayout

	JXTextMenu*   itsPredefSizeMenu;
	JXTextMenu*   itsUnitMenu;
	JXFloatInput* itsWidthInput;
	JXFloatInput* itsHeightInput;

// end JXLayout

private:

	void	BuildWindow(const JString& fileName,
						const bool printPreview, const bool bw,
						const JCoordinate w, const JCoordinate h,
						const Unit unit);

	void	HandlePredefSizeMenu(const JIndex index);

	void	UpdateUnitMenu();
	void	HandleUnitMenu(const JIndex index);
	void	UpdateSize(const Unit origUnit, const Unit newUnit,
					   JXFloatInput* input);

	void	GetSize(JCoordinate* w, JCoordinate* h) const;
};

std::istream& operator>>(std::istream& input, JX2DPlotPrintEPSDialog::Unit& u);
std::ostream& operator<<(std::ostream& output, const JX2DPlotPrintEPSDialog::Unit u);

#endif
