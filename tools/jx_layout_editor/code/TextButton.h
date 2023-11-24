/******************************************************************************
 TextButton.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_TextButton
#define _H_TextButton

#include "CoreWidget.h"
#include <jx-af/jcore/JFontStyle.h>

class JXTextButton;

class TextButton : public CoreWidget
{
public:

	TextButton(LayoutDocument* dir, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);
	TextButton(LayoutDocument* dir, const JString& label,
				JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);
	TextButton(LayoutDocument* dir, std::istream& input, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	~TextButton() override;

	void	StreamOut(std::ostream& output) const override;

	const JString&	GetLabel() const;
	void			SetLabel(const JString& label);

private:

	JXTextButton*	itsButton;

private:

	void	TextButtonX(const JString& label,
						const JCoordinate x, const JCoordinate y,
						const JCoordinate w, const JCoordinate h);
};

#endif
