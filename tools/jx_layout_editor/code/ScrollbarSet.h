/******************************************************************************
 ScrollbarSet.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_ScrollbarSet
#define _H_ScrollbarSet

#include "ContainerWidget.h"

class ScrollbarSet : public ContainerWidget
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
	JString	GetEnclosureName() const override;

	void	PrepareToGenerateCode() const override;
	void	GenerateCodeFinished() const override;

protected:

	void	Draw(JXWindowPainter& p, const JRect& rect) override;

	JString	GetClassName() const override;

private:

	void	ScrollbarSetX();
};

#endif
