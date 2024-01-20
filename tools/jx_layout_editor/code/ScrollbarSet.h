/******************************************************************************
 ScrollbarSet.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_ScrollbarSet
#define _H_ScrollbarSet

#include "BaseWidget.h"

class ScrollbarSet : public BaseWidget
{
public:

	ScrollbarSet(LayoutContainer* layout,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);
	ScrollbarSet(std::istream& input, const JFileVersion vers, LayoutContainer* layout,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	~ScrollbarSet() override;

	void	StreamOut(std::ostream& output) const override;
	bool	GetLayoutContainer(LayoutContainer** layout) const override;
	JString	GetEnclosureName() const override;

	void	PrepareToGenerateCode() const override;
	void	GenerateCodeFinished() const override;

protected:

	void	Draw(JXWindowPainter& p, const JRect& rect) override;
	void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;

	JString	GetClassName() const override;

private:

	LayoutContainer*	itsLayout;

private:

	void	ScrollbarSetX(LayoutContainer* layout);
};

#endif
