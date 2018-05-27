/******************************************************************************
 TestDNDTextDirector.cpp

	This creates a window with two JXTextEditorSet's, each containing some
	styled text.  This provides a convenient way to test Drag-And-Drop.

	BASE CLASS = JXWindowDirector

	Written by John Lindal.

 ******************************************************************************/

#include "TestDNDTextDirector.h"
#include <JXWindow.h>
#include <JXHorizPartition.h>
#include <JXTextEditorSet.h>
#include <JXTextEditor.h>
#include <JXFontManager.h>
#include <JXColorManager.h>
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

#define TEXT_RANGE(a,b) text->CharToTextRange(NULL, JCharacterRange(a, b))

void
TestDNDTextDirector::BuildWindow()
{
JIndex i;

	JXWindow* window = jnew JXWindow(this, kWindowWidth,kWindowHeight, JString::empty);
	assert( window != NULL );

	window->SetTitle(JGetString("WindowTitle::TestDNDTextDirector"));

	JArray<JCoordinate> sizes;
	JArray<JCoordinate> minSizes;
	for (i=1; i<=2; i++)
		{
		sizes.AppendElement(kInitWidth);
		minSizes.AppendElement(kMinWidth);
		}

	JXHorizPartition* partition =
		jnew JXHorizPartition(sizes, 0, minSizes, window,
							 JXWidget::kHElastic, JXWidget::kVElastic,
							 0,0, kWindowWidth, kWindowHeight);
	assert( partition != NULL );

	window->SetWMClass("testjx", "TestDNDTextDirector");
	window->SetMinSize(partition->GetMinTotalSize(), kWindowHeight);

	JXTextEditor* te;
	for (i=1; i<=2; i++)
		{
		JStyledText* text = jnew JStyledText(kJTrue, kJTrue);
		assert( text != NULL );

		JXTextEditorSet* teSet =
			jnew JXTextEditorSet(text, kJTrue, &te, partition->GetCompartment(i),
								JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 100,100);
		assert( teSet != NULL );
		teSet->FitToEnclosure();

		// create something to drag around

		text->SetText(JString(
			"plain\n\nbold\n\nitalicunderline\n\ntrplunderline\n\nstrike\n\nboldred\n",
			0, kJFalse));

		text->SetFontStyle(TEXT_RANGE(8, 13), JFontStyle(kJTrue, kJFalse, 0, kJFalse), kJTrue);
		text->SetFontStyle(TEXT_RANGE(14, 30), JFontStyle(kJFalse, kJTrue, 1, kJFalse), kJTrue);
		text->SetFontStyle(TEXT_RANGE(31, 45), JFontStyle(kJFalse, kJFalse, 3, kJFalse), kJTrue);
		text->SetFontStyle(TEXT_RANGE(46, 53), JFontStyle(kJFalse, kJFalse, 0, kJTrue), kJTrue);
		text->SetFontStyle(TEXT_RANGE(54, 62), JFontStyle(kJTrue, kJFalse, 0, kJFalse,
											JColorManager::GetRedColor()), kJTrue);
		}
}

#undef TEXT_RANGE
