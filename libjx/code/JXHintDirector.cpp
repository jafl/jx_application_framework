/******************************************************************************
 JXHintDirector.cpp

	Displays the given text in a separate window close to the specified
	widget.

	BASE CLASS = JXWindowDirector

	Copyright © 1999 by John Lindal.  All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXHintDirector.h>
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXBorderRect.h>
#include <JXStaticText.h>
#include <JXFontManager.h>
#include <JXColormap.h>
#include <jAssert.h>

const JCoordinate kHMargin = 3;
const JCoordinate kVMargin = 4;

static const JRGB kBackColor(0xFFFF, 0xFFFF, 0xC000);	// light yellow

/******************************************************************************
 Constructor

 ******************************************************************************/

JXHintDirector::JXHintDirector
	(
	JXDirector*			supervisor,
	JXContainer*		widget,
	const JCharacter*	text
	)
	:
	JXWindowDirector(supervisor)
{
	const JRect frameR = (widget->GetWindow())->GlobalToRoot(widget->GetFrameGlobal());
	BuildWindow(widget->GetColormap(), frameR, text);
}

JXHintDirector::JXHintDirector
	(
	JXDirector*			supervisor,
	JXContainer*		widget,
	const JRect&		rect,
	const JCharacter*	text
	)
	:
	JXWindowDirector(supervisor)
{
	const JRect frameR =
		(widget->GetWindow())->GlobalToRoot(widget->JXContainer::LocalToGlobal(rect));
	BuildWindow(widget->GetColormap(), frameR, text);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXHintDirector::~JXHintDirector()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JXHintDirector::BuildWindow
	(
	JXColormap*			colormap,
	const JRect&		frameR,
	const JCharacter*	text
	)
{
	// create window and contents

	JXWindow* window = new JXWindow(this, 10,10, "", kJFalse, colormap, kJTrue);
    assert( window != NULL );
    SetWindow(window);

	JXBorderRect* border =
		new JXBorderRect(window, JXWidget::kHElastic, JXWidget::kVElastic,
						 0,0, 10,10);
	assert( border != NULL );
	border->FitToEnclosure();

	JXStaticText* textWidget =
		new JXStaticText(text, window,
						 JXWidget::kFixedLeft, JXWidget::kFixedTop,
						 kHMargin, kVMargin, 0,0);
	assert( textWidget != NULL );

	JCoordinate ascent = 0, descent = 0;
	if (!JStringEmpty(text))
		{
		JString fontName;
		JSize fontSize;
		JFontStyle fontStyle;
		textWidget->GetFont(1, &fontName, &fontSize, &fontStyle);

		(textWidget->GetFontManager())->GetLineHeight(fontName, fontSize, fontStyle,
													  &ascent, &descent);
		}

	const JCoordinate w = 2*kHMargin + textWidget->GetFrameWidth();
	const JCoordinate h = 2*kVMargin + textWidget->GetFrameHeight() - descent;
	window->SetSize(w,h);

	// place window

	const JRect rootBounds = (GetDisplay())->GetBounds();

	JCoordinate x = frameR.left + 1;
	JCoordinate y = frameR.bottom + 1;

	if (x + w > rootBounds.right)
		{
		x = rootBounds.right - w - 1;
		}
	if (x < 0)
		{
		x = rootBounds.left + 1;
		}

	if (y + h > rootBounds.bottom)
		{
		y = frameR.top - h - 1;
		}

	window->Place(x,y);

	// use standard background color

	JColorIndex backColorIndex;
	if (!colormap->JColormap::AllocateStaticColor(kBackColor, &backColorIndex))
		{
		backColorIndex = colormap->GetGray90Color();
		}
	window->SetBackColor(backColorIndex);
	border->SetBackColor(backColorIndex);
	textWidget->SetBackgroundColor(backColorIndex);
}
