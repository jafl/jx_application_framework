/******************************************************************************
 MenuBar.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_MenuBar
#define _H_MenuBar

#include "LayoutWidget.h"

class MenuBar : public LayoutWidget
{
public:

	MenuBar(LayoutContainer* layout,
			const HSizingOption hSizing, const VSizingOption vSizing,
			const JCoordinate x, const JCoordinate y,
			const JCoordinate w, const JCoordinate h);
	MenuBar(std::istream& input, const JFileVersion vers, LayoutContainer* layout,
			const HSizingOption hSizing, const VSizingOption vSizing,
			const JCoordinate x, const JCoordinate y,
			const JCoordinate w, const JCoordinate h);

	~MenuBar() override;

	void	StreamOut(std::ostream& output) const override;

protected:

	void	Draw(JXWindowPainter& p, const JRect& rect) override;
	void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;

	JString	GetClassName() const override;

private:

	void	MenuBarX(LayoutContainer* layout);
};

#endif
