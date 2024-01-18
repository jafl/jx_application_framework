/******************************************************************************
 ToolBar.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_ToolBar
#define _H_ToolBar

#include "BaseWidget.h"

class ToolBarPanel;

class ToolBar : public BaseWidget
{
public:

	ToolBar(LayoutContainer* layout,
			const HSizingOption hSizing, const VSizingOption vSizing,
			const JCoordinate x, const JCoordinate y,
			const JCoordinate w, const JCoordinate h);
	ToolBar(const JString& prefsMgr, const JString& prefID, const JString& menuBar,
			LayoutContainer* layout,
			const HSizingOption hSizing, const VSizingOption vSizing,
			const JCoordinate x, const JCoordinate y,
			const JCoordinate w, const JCoordinate h);
	ToolBar(std::istream& input, const JFileVersion vers, LayoutContainer* layout,
			const HSizingOption hSizing, const VSizingOption vSizing,
			const JCoordinate x, const JCoordinate y,
			const JCoordinate w, const JCoordinate h);

	~ToolBar() override;

	void	StreamOut(std::ostream& output) const override;
	JString	ToString() const override;
	bool	GetLayoutContainer(LayoutContainer** layout) const override;

protected:

	void	Draw(JXWindowPainter& p, const JRect& rect) override;
	void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;

	JString	GetClassName() const override;
	void	PrintCtorArgsWithComma(std::ostream& output,
								   const JString& varName,
								   JStringManager* stringdb) const override;

	void	AddPanels(WidgetParametersDialog* dlog) override;
	void	SavePanelData() override;

private:

	JString				itsPrefsMgr;
	JString				itsPrefID;
	JString				itsMenuBar;
	LayoutContainer*	itsLayout;
	ToolBarPanel*		itsPanel;

private:

	void	ToolBarX(LayoutContainer* layout);
};

#endif
