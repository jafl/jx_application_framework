/******************************************************************************
 ImageRadioButton.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_ImageRadioButton
#define _H_ImageRadioButton

#include "LayoutWidget.h"

class RadioButtonIDPanel;
class ImageChooserPanel;

class ImageRadioButton : public LayoutWidget
{
public:

	ImageRadioButton(LayoutContainer* layout,
					 const HSizingOption hSizing, const VSizingOption vSizing,
					 const JCoordinate x, const JCoordinate y,
					 const JCoordinate w, const JCoordinate h);
	ImageRadioButton(const JString& id, LayoutContainer* layout,
					 const HSizingOption hSizing, const VSizingOption vSizing,
					 const JCoordinate x, const JCoordinate y,
					 const JCoordinate w, const JCoordinate h);
	ImageRadioButton(std::istream& input, const JFileVersion vers, LayoutContainer* layout,
					 const HSizingOption hSizing, const VSizingOption vSizing,
					 const JCoordinate x, const JCoordinate y,
					 const JCoordinate w, const JCoordinate h);

	~ImageRadioButton() override;

	void	StreamOut(std::ostream& output) const override;
	JString	ToString() const override;

protected:

	void	Draw(JXWindowPainter& p, const JRect& rect) override;
	void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;

	JString	GetClassName() const override;
	void	PrintCtorArgsWithComma(std::ostream& output,
								   const JString& varName,
								   JStringManager* stringdb) const override;
	void	PrintConfiguration(std::ostream& output,
							   const JString& indent,
							   const JString& varName,
							   JStringManager* stringdb) const override;

	void	AddPanels(WidgetParametersDialog* dlog) override;
	void	SavePanelData() override;

private:

	JString				itsID;
	JString				itsImageFullName;
	RadioButtonIDPanel*	itsIDPanel;		// nullptr unless editing
	ImageChooserPanel*	itsImagePanel;	// nullptr unless editing

private:

	void	ImageRadioButtonX();
};

#endif
