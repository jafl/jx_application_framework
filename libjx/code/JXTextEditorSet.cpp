/******************************************************************************
 JXTextEditorSet.cpp

	Maintains an enclosure for all the objects required by a JXTextEditor.

	BASE CLASS = JXWidgetSet

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include <JXTextEditorSet.h>
#include <JXTextEditor.h>
#include <JXMenuBar.h>
#include <JXScrollbarSet.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXTextEditorSet::JXTextEditorSet
	(
	JStyledText*		text,
	const JBoolean		ownsText,
	JXTextEditor**		textEditor,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXWidgetSet(enclosure, hSizing, vSizing, x,y, w,h)
{
	JXMenuBar* menuBar =
		jnew JXMenuBar(this, kHElastic, kFixedTop, 0,0, w,kJXDefaultMenuBarHeight);
	assert( menuBar != nullptr );

	JXScrollbarSet* scrollbarSet =
		jnew JXScrollbarSet(this, kHElastic,kVElastic,
						   0,kJXDefaultMenuBarHeight, w,h-kJXDefaultMenuBarHeight);
	assert( scrollbarSet != nullptr );

	*textEditor =
		jnew JXTextEditor(text, ownsText, menuBar, scrollbarSet,
						  scrollbarSet->GetScrollEnclosure(),
						  kHElastic, kVElastic, 0,0, 10,10);
	assert( *textEditor != nullptr );

	(**textEditor).FitToEnclosure();

	SetNeedsInternalFTC();
}

JXTextEditorSet::JXTextEditorSet
	(
	JXMenuBar*			menuBar,
	JStyledText*		text,
	const JBoolean		ownsText,
	JXTextEditor**		textEditor,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXWidgetSet(enclosure, hSizing, vSizing, x,y, w,h)
{
	assert( menuBar != nullptr );

	JXScrollbarSet* scrollbarSet =
		jnew JXScrollbarSet(this, kHElastic, kVElastic, 0,0, w,h);
	assert( scrollbarSet != nullptr );

	*textEditor =
		jnew JXTextEditor(text, ownsText, menuBar, scrollbarSet,
						  scrollbarSet->GetScrollEnclosure(),
						  kHElastic, kVElastic, 0,0, 10,10);
	assert( *textEditor != nullptr );

	(**textEditor).FitToEnclosure();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXTextEditorSet::~JXTextEditorSet()
{
}
