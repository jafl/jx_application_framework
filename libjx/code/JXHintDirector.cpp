/******************************************************************************
 JXHintDirector.cpp

	Displays the given text in a separate window close to the specified
	widget.

	BASE CLASS = JXWindowDirector

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include "JXHintDirector.h"
#include "JXDisplay.h"
#include "JXWindow.h"
#include "JXBorderRect.h"
#include "JXStaticText.h"
#include "JXFontManager.h"
#include "JXColorManager.h"
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
	const JRect frameR = widget->GetWindow()->GlobalToRoot(widget->GetFrameGlobal());
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
	const JRect frameR = widget->GetWindow()->GlobalToRoot(widget->LocalToGlobal(rect));
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

	window->SetWMWindowType(JXWindow::kWMTooltipType);

	auto* border =
		jnew JXBorderRect(window, JXWidget::kHElastic, JXWidget::kVElastic,
						 0,0, 10,10);
	border->FitToEnclosure();

	auto* textWidget =
		jnew JXStaticText(text, border,
						 JXWidget::kFixedLeft, JXWidget::kFixedTop,
						 kHMargin, kVMargin, 0,0);

	const JCoordinate w = 2*kHMargin + textWidget->GetFrameWidth();
	const JCoordinate h = 2*kVMargin + textWidget->GetFrameHeight();
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
