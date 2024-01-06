/******************************************************************************
 TextButton.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_TextButton
#define _H_TextButton

#include "CoreWidget.h"
#include <jx-af/jcore/JFontStyle.h>

class JXTextButton;
class WidgetLabelPanel;

class TextButton : public CoreWidget
{
public:

	TextButton(LayoutContainer* layout, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);
	TextButton(LayoutContainer* layout, const JString& label,
				JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);
	TextButton(LayoutContainer* layout, std::istream& input, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	~TextButton() override;

	void	StreamOut(std::ostream& output) const override;

protected:

	void	AddPanels(WidgetParametersDialog* dlog) override;
	void	SavePanelData() override;

private:

	JXTextButton*		itsButton;
//	WidgetLabelPanel*	itsPanel;	// nullptr unless editing

private:

	void	TextButtonX(const JString& label,
						const JCoordinate x, const JCoordinate y,
						const JCoordinate w, const JCoordinate h);
};

#endif
