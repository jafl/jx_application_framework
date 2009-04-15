/******************************************************************************
 GHintsDir.cpp

	BASE CLASS = JXWindowDirector

	Copyright © 1997 by Glenn Bach. All rights reserved.

 ******************************************************************************/

#include <glStdInc.h>
#include "GHintsDir.h"
#include <JXBorderRect.h>

#include <JXStaticText.h>
#include <JXWindow.h>
#include <JXDisplay.h>
#include <jXGlobals.h>

#include <JFontManager.h>
#include <JColormap.h>
#include <jAssert.h>

const JCoordinate kSideBuffer	= 10;
const JCoordinate kTopBuffer	= 3;

/******************************************************************************
 Constructor

 ******************************************************************************/

GHintsDir::GHintsDir
	(
	JXDirector* 		supervisor,
	const JCharacter* 	text,
	JXWidget* 			widget
	)
	:
	JXWindowDirector(supervisor)
{
	JXWindow* window = new JXWindow(this, 30,10, "", kJFalse, NULL, kJTrue);
    assert( window != NULL );
    SetWindow(window);

	const JFontManager* fm = window->GetFontManager();
	const JCoordinate w = fm->GetStringWidth("6x13", kJDefaultFontSize, JFontStyle(), text) + 2 * kSideBuffer;
	const JCoordinate h = fm->GetLineHeight("6x13", kJDefaultFontSize, JFontStyle()) + 2 * kTopBuffer;

	JColorIndex color = JGetCurrColormap()->GetGray90Color();
	JGetCurrColormap()->AllocateStaticColor(255*256,255*256,192*256, &color);

	window->SetBackColor(color);
	window->SetSize(w, h);

	JXWindow* widgetWindow = widget->GetWindow();
	JRect frame = widgetWindow->GlobalToRoot(widget->GetFrameGlobal());

	JRect sRect = window->GetDisplay()->GetBounds();

	const JCoordinate maxX = sRect.width();
	const JCoordinate maxY = sRect.height();

	JCoordinate x = frame.left + 1;
	JCoordinate y = frame.bottom + 1;

	if (x + w > maxX)
		{
		if (frame.right > maxX)
			{
			x = frame.left - w - 1;
			}
		else
			{
			x = frame.right - w - 1; 
			}
		}
	if (x < 0)
		{
		x = frame.right + 1;
		}
	if (y + h > maxY)
		{
		y = frame.top - h - 1;
		}
	
	window->Place(x,y);
	window->LockCurrentSize();

	JXBorderRect* brect = 
		new JXBorderRect(window,
							JXWidget::kFixedLeft, JXWidget::kFixedTop, 
							0,0,w,h);
	assert(brect != NULL);
	brect->SetBackColor(color);

	itsText = 
		new JXStaticText(text, window, 
						 JXWidget::kFixedLeft, JXWidget::kFixedTop, 
						 kSideBuffer - 2, kTopBuffer, 
						 w - kSideBuffer - 2, h - 2*kTopBuffer);
	assert(itsText != NULL);
	itsText->SetFontName("6x13");
	itsText->SetBackgroundColor(color);

	
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GHintsDir::~GHintsDir()
{
}
