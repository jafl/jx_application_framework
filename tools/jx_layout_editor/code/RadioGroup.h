/******************************************************************************
 RadioGroup.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_RadioGroup
#define _H_RadioGroup

#include "ContainerWidget.h"

class RadioGroupPanel;

class RadioGroup : public ContainerWidget
{
public:

	RadioGroup(LayoutContainer* layout,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);
	RadioGroup(std::istream& input, const JFileVersion vers, LayoutContainer* layout,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	~RadioGroup() override;

	void	StreamOut(std::ostream& output) const override;

protected:

	void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;

	JString	GetClassName() const override;
	void	PrintConfiguration(std::ostream& output,
							   const JString& indent,
							   const JString& varName,
							   JStringManager* stringdb) const override;

	void	AddPanels(WidgetParametersDialog* dlog) override;
	void	SavePanelData() override;

private:

	RadioGroupPanel*	itsPanel;

private:

	void	RadioGroupX();
};

#endif
