/******************************************************************************
 TextButton.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_TextButton
#define _H_TextButton

#include "BaseWidget.h"
#include <jx-af/jcore/JFontStyle.h>

class JXTextButton;

class TextButton : public BaseWidget
{
	friend class JXRadioButton;

public:

	TextButton(JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);
	TextButton(const JString& label,
				JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);
	TextButton(std::istream& input, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	~TextButton() override;

	void	StreamOut(std::ostream& output) const override;

	const JString&	GetLabel() const;
	void			SetLabel(const JString& label);

protected:

	void	Draw(JXWindowPainter& p, const JRect& rect) override;

private:

	JString		itsLabel;
	JString		itsShortcuts;
	JString		itsFontName;
	JSize		itsFontSize;
	JFontStyle	itsFontStyle;
	JColorID	itsNormalColor;
	JColorID	itsPushedColor;

	JXTextButton*	itsWidget;

private:

	void	TextButtonX(const JCoordinate x, const JCoordinate y,
						const JCoordinate w, const JCoordinate h);
};


/******************************************************************************
 GetLabel

 ******************************************************************************/

inline const JString&
TextButton::GetLabel()
	const
{
	return itsLabel;
}

#endif
