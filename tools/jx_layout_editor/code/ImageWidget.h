/******************************************************************************
 ImageWidget.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_ImageWidget
#define _H_ImageWidget

#include "LayoutWidget.h"

class ImageChooserPanel;

class ImageWidget : public LayoutWidget
{
public:

	enum Type
	{
		kImageType,
		kButtonType,
		kCheckboxType
	};

public:

	ImageWidget(const Type type, LayoutContainer* layout,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);
	ImageWidget(std::istream& input, const JFileVersion vers, LayoutContainer* layout,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	~ImageWidget() override;

	void	StreamOut(std::ostream& output) const override;

protected:

	void	Draw(JXWindowPainter& p, const JRect& rect) override;
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
	JString				itsFullName;
	ImageChooserPanel*	itsPanel;		// nullptr unless editing

private:

	void	ImageWidgetX();
};

#endif
