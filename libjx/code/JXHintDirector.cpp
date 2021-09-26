/******************************************************************************
 JXHintDirector.cpp

	Displays the given text in a separate window close to the specified
	widget.

	BASE CLASS = JXWindowDirector

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include "jx-af/jx/JXHintDirector.h"
#include "jx-af/jx/JXDisplay.h"
#include "jx-af/jx/JXWindow.h"
#include "jx-af/jx/JXBorderRect.h"
#include "jx-af/jx/JXStaticText.h"
#include "jx-af/jx/JXFontManager.h"
#include "jx-af/jx/JXColorManager.h"
#include <jx-af/jcore/jAssert.h>

const JCoordinate kHMargin = 3;
const JCoordinate kVMargin = 4;

static const JRGB kBackColor(0xFFFF, 0xFFFF, 0xC000);	// light yellow

/******************************************************************************
 Constructor

 ******************************************************************************/

JXHintDirector::JXHintDirector
	(
	JXDirector*		supervisor,
	JXContainer*	widget,
	const JString&	text
	)
	:
	JXWindowDirector(supervisor)
{
	const JRect frameR = (widget->GetWindow())->GlobalToRoot(widget->GetFrameGlobal());
	BuildWindow(frameR, text);
}

JXHintDirector::JXHintDirector
	(
	JXDirector*		supervisor,
	JXContainer*	widget,
	const JRect&	rect,
	const JString&	text
	)
	:
	JXWindowDirector(supervisor)
{
	const JRect frameR =
		(widget->GetWindow())->GlobalToRoot(widget->JXContainer::LocalToGlobal(rect));
	BuildWindow(frameR, text);
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
	const JRect&	frameR,
	const JString&	text
	)
{
	// create window and contents

	auto* window = jnew JXWindow(this, 10,10, JString::empty, true);
	assert( window != nullptr );

	window->SetWMWindowType(JXWindow::kWMTooltipType);

	auto* border =
		jnew JXBorderRect(window, JXWidget::kHElastic, JXWidget::kVElastic,
						 0,0, 10,10);
	assert( border != nullptr );
	border->FitToEnclosure();

	auto* textWidget =
		jnew JXStaticText(text, border,
						 JXWidget::kFixedLeft, JXWidget::kFixedTop,
						 kHMargin, kVMargin, 0,0);
	assert( textWidget != nullptr );

	JCoordinate ascent = 0, descent = 0;
	if (!text.IsEmpty())
	{
		textWidget->GetText()->GetFont(1).GetLineHeight(window->GetFontManager(), &ascent, &descent);
	}

	const JCoordinate w = 2*kHMargin + textWidget->GetFrameWidth();
	const JCoordinate h = 2*kVMargin + ascent + descent;
	window->SetSize(w,h);

	// place window

	const JRect rootBounds = GetDisplay()->GetBounds();

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

	const JColorID backColorIndex = JColorManager::GetColorID(kBackColor);
	window->SetBackColor(backColorIndex);
	border->SetBackColor(backColorIndex);
	textWidget->SetBackgroundColor(backColorIndex);
}
