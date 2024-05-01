/******************************************************************************
 JXExprEditorSet.cpp

	Maintains an enclosure for all the objects required by a JXExprEditor.

	BASE CLASS = JXWidgetSet

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXExprEditorSet.h"
#include "JXExprEditor.h"
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jexpr/JVariableList.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXExprEditorSet::JXExprEditorSet
	(
	const JVariableList*	varList,
	JXExprEditor**			exprWidget,
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
	auto* menuBar =
		jnew JXMenuBar(this, kHElastic, kFixedTop, 0,0, w,kJXDefaultMenuBarHeight);

	auto* scrollbarSet =
		jnew JXScrollbarSet(this, kHElastic, kVElastic,
						   0,kJXDefaultMenuBarHeight, w,h-kJXDefaultMenuBarHeight);

	*exprWidget =
		jnew JXExprEditor(varList, menuBar, scrollbarSet,
						 scrollbarSet->GetScrollEnclosure(),
						 kHElastic, kVElastic, 0,0, 10,10);
	(**exprWidget).FitToEnclosure();
}

JXExprEditorSet::JXExprEditorSet
	(
	const JVariableList*	varList,
	JXMenuBar*				menuBar,
	JXExprEditor**			exprWidget,
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
	assert( menuBar != nullptr );

	auto* scrollbarSet =
		jnew JXScrollbarSet(this, kHElastic, kVElastic, 0,0, w,h);

	*exprWidget =
		jnew JXExprEditor(varList, menuBar, scrollbarSet,
						 scrollbarSet->GetScrollEnclosure(),
						 kHElastic, kVElastic, 0,0, 10,10);
	(**exprWidget).FitToEnclosure();
}

JXExprEditorSet::JXExprEditorSet
	(
	const JVariableList*	varList,
	JXExprEditor*			menuProvider,
	JXExprEditor**			exprWidget,
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
	auto* scrollbarSet =
		jnew JXScrollbarSet(this, kHElastic, kVElastic, 0,0, w,h);

	*exprWidget =
		jnew JXExprEditor(varList, menuProvider, scrollbarSet,
						 scrollbarSet->GetScrollEnclosure(),
						 kHElastic, kVElastic, 0,0, 10,10);
	(**exprWidget).FitToEnclosure();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXExprEditorSet::~JXExprEditorSet()
{
}
