/******************************************************************************
 LayoutContainer.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_LayoutContainer
#define _H_LayoutContainer

#include <jx-af/jx/JXWidget.h>

class LayoutContainer : public JXWidget
{
public:

	LayoutContainer(JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	~LayoutContainer() override;

protected:

	void	Draw(JXWindowPainter& p, const JRect& rect) override;
	void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;

private:

	JSize	itsGridSpacing;
};

#endif
