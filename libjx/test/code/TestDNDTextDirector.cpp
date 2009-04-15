/******************************************************************************
 TestDNDTextDirector.cpp

	This creates a window with two JXTextEditorSet's, each containing some
	styled text.  This provides a convenient way to test Drag-And-Drop.

	BASE CLASS = JXWindowDirector

	Written by John Lindal.

 ******************************************************************************/

#include <JXStdInc.h>
#include "TestDNDTextDirector.h"
#include <JXWindow.h>
#include <JXHorizPartition.h>
#include <JXTextEditorSet.h>
#include <JXTextEditor.h>
#include <JXColormap.h>
#include <jXGlobals.h>
#include <jAssert.h>

const JCoordinate kInitWidth    = 100;
const JCoordinate kMinWidth     = 20;
const JCoordinate kWindowWidth  = 2*kInitWidth + JPartition::kDragRegionSize;
const JCoordinate kWindowHeight = 100;

/******************************************************************************
 Constructor

 ******************************************************************************/

TestDNDTextDirector::TestDNDTextDirector
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor)
{
	BuildWindow();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TestDNDTextDirector::~TestDNDTextDirector()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
TestDNDTextDirector::BuildWindow()
{
JIndex i;

	JXWindow* window = new JXWindow(this, kWindowWidth,kWindowHeight, "");
	assert( window != NULL );
	SetWindow(window);

	window->SetTitle("Test Drag-And-Drop (text)");

	JArray<JCoordinate> sizes;
	JArray<JCoordinate> minSizes;
	for (i=1; i<=2; i++)
		{
		sizes.AppendElement(kInitWidth);
		minSizes.AppendElement(kMinWidth);
		}

	JXHorizPartition* partition =
		new JXHorizPartition(sizes, 0, minSizes, window,
							 JXWidget::kHElastic, JXWidget::kVElastic,
							 0,0, kWindowWidth, kWindowHeight);
	assert( partition != NULL );

	window->SetWMClass("testjx", "TestDNDTextDirector");
	window->SetMinSize(partition->GetMinTotalSize(), kWindowHeight);

	JXTextEditor* te;
	for (i=1; i<=2; i++)
		{
		JXTextEditorSet* teSet =
			new JXTextEditorSet(&te, partition->GetCompartment(i),
								JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 100,100);
		assert( teSet != NULL );
		teSet->FitToEnclosure();

		// create something to drag around

		te->SetText("plain\n\nbold\n\nitalicunderline\n\ntrplunderline\n\nstrike\n\n"
					"boldred\n\nsymbol\n");

		te->SetFontStyle(8, 13, JFontStyle(kJTrue, kJFalse, 0, kJFalse), kJTrue);
		te->SetFontStyle(14, 30, JFontStyle(kJFalse, kJTrue, 1, kJFalse), kJTrue);
		te->SetFontStyle(31, 45, JFontStyle(kJFalse, kJFalse, 3, kJFalse), kJTrue);
		te->SetFontStyle(46, 53, JFontStyle(kJFalse, kJFalse, 0, kJTrue), kJTrue);
		te->SetFontStyle(54, 62, JFontStyle(kJTrue, kJFalse, 0, kJFalse,
											(GetColormap())->GetRedColor()), kJTrue);
		te->SetFont(63, 69, JXGetSymbolFontName(), 18,
					JFontStyle(kJFalse, kJTrue, 0, kJFalse), kJTrue);
		}
}
