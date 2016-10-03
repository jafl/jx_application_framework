/******************************************************************************
 SCExprEditorSet.cpp

	Maintains an enclosure for all the objects required by a SCExprEditor.

	BASE CLASS = JXWidgetSet

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "SCExprEditorSet.h"
#include "SCExprEditor.h"
#include <JXMenuBar.h>
#include <JXScrollbarSet.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SCExprEditorSet::SCExprEditorSet
	(
	SCCircuitDocument*	doc,
	SCExprEditor**		exprWidget,
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
		new JXScrollbarSet(this, kHElastic, kVElastic,
						   0,kJXDefaultMenuBarHeight, w,h-kJXDefaultMenuBarHeight);
	assert( scrollbarSet != NULL );

	*exprWidget =
		new SCExprEditor(doc, menuBar, scrollbarSet,
						 scrollbarSet->GetScrollEnclosure(),
						 kHElastic, kVElastic, 0,0, 10,10);
	assert( *exprWidget != NULL );

	(**exprWidget).FitToEnclosure();
}

SCExprEditorSet::SCExprEditorSet
	(
	SCCircuitDocument*	doc,
	JXMenuBar*			menuBar,
	SCExprEditor**		exprWidget,
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

	*exprWidget =
		new SCExprEditor(doc, menuBar, scrollbarSet,
						 scrollbarSet->GetScrollEnclosure(),
						 kHElastic, kVElastic, 0,0, 10,10);
	assert( *exprWidget != NULL );

	(**exprWidget).FitToEnclosure();
}

SCExprEditorSet::SCExprEditorSet
	(
	SCCircuitDocument*		doc,
	SCExprEditor*			menuProvider,
	SCExprEditor**			exprWidget,
	JXContainer*			enclosure,
	const HSizingOption		hSizing,
	const VSizingOption		vSizing,
	const JCoordinate		x,
	const JCoordinate		y,
	const JCoordinate		w,
	const JCoordinate		h
	)
	:
	JXWidgetSet(enclosure, hSizing, vSizing, x,y, w,h)
{
	JXScrollbarSet* scrollbarSet =
		new JXScrollbarSet(this, kHElastic, kVElastic, 0,0, w,h);
	assert( scrollbarSet != NULL );

	*exprWidget =
		new SCExprEditor(doc, menuProvider, scrollbarSet,
						 scrollbarSet->GetScrollEnclosure(),
						 kHElastic, kVElastic, 0,0, 10,10);
	assert( *exprWidget != NULL );

	(**exprWidget).FitToEnclosure();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SCExprEditorSet::~SCExprEditorSet()
{
}
