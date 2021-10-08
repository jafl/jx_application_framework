/******************************************************************************
 JXStaticText.h

	Interface for the JXStaticText class

	Copyright (C) 1996-2017 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXStaticText
#define _H_JXStaticText

#include "jx-af/jx/JXTEBase.h"

class JXStaticText : public JXTEBase
{
public:

	JXStaticText(const JString& text, JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	JXStaticText(const JString& text, const bool wordWrap,
				 const bool selectable, const bool allowStyles,
				 JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	~JXStaticText() override;

	void	SetFontName(const JString& name);
	void	SetFontSize(const JSize size);
	void	SetFontStyle(const JFontStyle& style);
	void	SetFont(const JFont& f);

	void	SetBackgroundColor(const JColorID color);

	void	SetToLabel(const bool centerHorizontally = false);

	JString	ToString() const override;

protected:

	void		BoundsMoved(const JCoordinate dx, const JCoordinate dy) override;
	void		BoundsResized(const JCoordinate dw, const JCoordinate dh) override;
	JCoordinate	GetFTCMinContentSize(const bool horizontal) const override;

private:

	bool	itsCenterHorizFlag;
	bool	itsCenterVertFlag;

private:

	void	JXStaticTextX(const JString& text,
						  const JCoordinate w, const JCoordinate h);

	void	Center();
};


/******************************************************************************
 Change font

 ******************************************************************************/

inline void
JXStaticText::SetFontName
	(
	const JString& name
	)
{
	SelectAll();
	SetCurrentFontName(name);
	GetText()->SetDefaultFontName(name);
}

inline void
JXStaticText::SetFontSize
	(
	const JSize size
	)
{
	SelectAll();
	SetCurrentFontSize(size);
	GetText()->SetDefaultFontSize(size);
}

inline void
JXStaticText::SetFontStyle
	(
	const JFontStyle& style
	)
{
	SelectAll();
	SetCurrentFontStyle(style);
	GetText()->SetDefaultFontStyle(style);
}

inline void
JXStaticText::SetFont
	(
	const JFont& f
	)
{
	SelectAll();
	SetCurrentFont(f);
	GetText()->SetDefaultFont(f);
}

/******************************************************************************
 SetBackgroundColor

 ******************************************************************************/

inline void
JXStaticText::SetBackgroundColor
	(
	const JColorID color
	)
{
	SetBackColor(color);
	SetFocusColor(color);
}

#endif
