/******************************************************************************
 SimpleBorderRect.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_SimpleBorderRect
#define _H_SimpleBorderRect

#include "ContainerWidget.h"

class BorderWidthPanel;
class ColorChooserPanel;

class SimpleBorderRect : public ContainerWidget
{
public:

	enum Type
	{
		kBorderType,
		kUpType,
		kDownType
	};

public:

	SimpleBorderRect(const Type type, LayoutContainer* layout,
					 const HSizingOption hSizing, const VSizingOption vSizing,
					 const JCoordinate x, const JCoordinate y,
					 const JCoordinate w, const JCoordinate h);
	SimpleBorderRect(const Type type, const JSize width, const JColorID color,
					 LayoutContainer* layout,
					 const HSizingOption hSizing, const VSizingOption vSizing,
					 const JCoordinate x, const JCoordinate y,
					 const JCoordinate w, const JCoordinate h);
	SimpleBorderRect(std::istream& input, const JFileVersion vers, LayoutContainer* layout,
					 const HSizingOption hSizing, const VSizingOption vSizing,
					 const JCoordinate x, const JCoordinate y,
					 const JCoordinate w, const JCoordinate h);

	~SimpleBorderRect() override;

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

	Type				itsType;
	JSize				itsWidth;
	JColorID			itsColor;
	BorderWidthPanel*	itsWidthPanel;
	ColorChooserPanel*	itsColorPanel;

private:

	void	SimpleBorderRectX();
};

#endif
