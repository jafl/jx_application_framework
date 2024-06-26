/******************************************************************************
 TestDNDTextDirector.cpp

	This creates a window with two JXTextEditorSet's, each containing some
	styled text.  This provides a convenient way to test Drag-And-Drop.

	BASE CLASS = JXWindowDirector

	Written by John Lindal.

 ******************************************************************************/

#include "TestDNDTextDirector.h"
#include <jx-af/jx/JXStyledText.h>
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXHorizPartition.h>
#include <jx-af/jx/JXTextEditorSet.h>
#include <jx-af/jx/JXTextEditor.h>
#include <jx-af/jx/JXFontManager.h>
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jx/jXGlobals.h>
#include <jx-af/jcore/jAssert.h>

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

#define TEXT_RANGE(a,b) text->CharToTextRange(nullptr, JCharacterRange(a, b))

void
TestDNDTextDirector::BuildWindow()
{
	JXWindow* window = jnew JXWindow(this, kWindowWidth,kWindowHeight, JString::empty);

	window->SetTitle(JGetString("WindowTitle::TestDNDTextDirector"));

	JArray<JCoordinate> sizes;
	JArray<JCoordinate> minSizes;
	for (JIndex i=1; i<=2; i++)
	{
		sizes.AppendItem(kInitWidth);
		minSizes.AppendItem(kMinWidth);
	}

	JXHorizPartition* partition =
		jnew JXHorizPartition(sizes, 0, minSizes, window,
							 JXWidget::kHElastic, JXWidget::kVElastic,
							 0,0, kWindowWidth, kWindowHeight);

	window->SetWMClass("testjx", "TestDNDTextDirector");
	window->SetMinSize(partition->GetMinTotalSize(), kWindowHeight);

	JXTextEditor* te;
	for (const JIndex i : {1,2})
	{
		JXStyledText* text = jnew JXStyledText(true, true, GetDisplay()->GetFontManager());

		JXTextEditorSet* teSet =
			jnew JXTextEditorSet(text, true, &te, partition->GetCompartment(i),
								JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 100,100);
		teSet->FitToEnclosure();

		// create something to drag around

		text->SetText(
			"plain\n\nbold\n\nitalicunderline\n\ntrplunderline\n\nstrike\n\nboldred\n");

		text->SetFontStyle(TEXT_RANGE( 8, 13), JFontStyle(true, false, 0, false), true);
		text->SetFontStyle(TEXT_RANGE(14, 30), JFontStyle(false, true, 1, false), true);
		text->SetFontStyle(TEXT_RANGE(31, 45), JFontStyle(false, false, 3, false), true);
		text->SetFontStyle(TEXT_RANGE(46, 53), JFontStyle(false, false, 0, true), true);
		text->SetFontStyle(TEXT_RANGE(54, 61),
			JFontStyle(true, false, 0, false, JColorManager::GetRedColor()), true);
	}
}

#undef TEXT_RANGE
