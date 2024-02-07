/******************************************************************************
 ToolBar.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_ToolBar
#define _H_ToolBar

#include "ContainerWidget.h"

class ToolBarPanel;

class ToolBar : public ContainerWidget
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
	JString	GetEnclosureName(const LayoutWidget* widget) const override;

	void	PrepareToGenerateCode(std::ostream& output, const JString& indent,
								  JStringManager* stringdb) const override;
	void	GenerateCodeFinished(std::ostream& output, const JString& indent,
								 JStringManager* stringdb) const override;

protected:

	void	Draw(JXWindowPainter& p, const JRect& rect) override;

	JString	GetClassName() const override;
	void	PrintCtorArgsWithComma(std::ostream& output,
								   const JString& varName,
								   JStringManager* stringdb) const override;

	void	AddPanels(WidgetParametersDialog* dlog) override;
	void	SavePanelData() override;

private:

	JString			itsPrefsMgr;
	JString			itsPrefID;
	JString			itsMenuBar;
	ToolBarPanel*	itsPanel;

private:

	void	ToolBarX();
};

#endif
