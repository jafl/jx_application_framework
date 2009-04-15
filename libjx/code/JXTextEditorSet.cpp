/******************************************************************************
 JXTextEditorSet.cpp

	Maintains an enclosure for all the objects required by a JXTextEditor.

	BASE CLASS = JXWidgetSet

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
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
		new JXMenuBar(this, kHElastic, kFixedTop, 0,0, w,kJXDefaultMenuBarHeight);
	assert( menuBar != NULL );

	JXScrollbarSet* scrollbarSet =
		new JXScrollbarSet(this, kHElastic,kVElastic,
						   0,kJXDefaultMenuBarHeight, w,h-kJXDefaultMenuBarHeight);
	assert( scrollbarSet != NULL );

	*textEditor =
		new JXTextEditor(menuBar, scrollbarSet,
						 scrollbarSet->GetScrollEnclosure(),
						 kHElastic, kVElastic, 0,0, 10,10);
	assert( *textEditor != NULL );

	(**textEditor).FitToEnclosure();
}

JXTextEditorSet::JXTextEditorSet
	(
	JXMenuBar*			menuBar,
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
	assert( menuBar != NULL );

	JXScrollbarSet* scrollbarSet =
		new JXScrollbarSet(this, kHElastic, kVElastic, 0,0, w,h);
	assert( scrollbarSet != NULL );

	*textEditor =
		new JXTextEditor(menuBar, scrollbarSet,
						 scrollbarSet->GetScrollEnclosure(),
						 kHElastic, kVElastic, 0,0, 10,10);
	assert( *textEditor != NULL );

	(**textEditor).FitToEnclosure();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXTextEditorSet::~JXTextEditorSet()
{
}
