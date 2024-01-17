/******************************************************************************
 RadioGroup.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_RadioGroup
#define _H_RadioGroup

#include "BaseWidget.h"

class RadioGroupPanel;

class RadioGroup : public BaseWidget
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
	bool	GetLayoutContainer(LayoutContainer** layout) const override;

protected:

	void	Draw(JXWindowPainter& p, const JRect& rect) override;
	void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;

	JString	GetClassName() const override;

private:

	LayoutContainer*	itsLayout;

private:

	void	RadioGroupX(LayoutContainer* layout);
};

#endif
