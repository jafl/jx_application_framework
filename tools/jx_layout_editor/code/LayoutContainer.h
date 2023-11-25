/******************************************************************************
 LayoutContainer.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_LayoutContainer
#define _H_LayoutContainer

#include <jx-af/jx/JXWidget.h>

class LayoutDocument;

class LayoutContainer : public JXWidget
{
public:

	LayoutContainer(LayoutDocument* doc, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	~LayoutContainer() override;

protected:

	void	Draw(JXWindowPainter& p, const JRect& rect) override;
	void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;

	void	BoundsResized(const JCoordinate dw, const JCoordinate dh) override;

private:

	LayoutDocument*	itsDoc;
	JSize			itsGridSpacing;
};

#endif
