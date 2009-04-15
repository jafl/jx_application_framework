/******************************************************************************
 JXExprEditorSet.h

	Interface for the JXExprEditorSet class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXExprEditorSet
#define _H_JXExprEditorSet

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWidgetSet.h>

class JVariableList;
class JXMenuBar;
class JXExprEditor;

class JXExprEditorSet : public JXWidgetSet
{
public:

	JXExprEditorSet(const JVariableList* varList,
					JXExprEditor** exprWidget, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	JXExprEditorSet(const JVariableList* varList, JXMenuBar* menuBar,
					JXExprEditor** exprWidget, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	JXExprEditorSet(const JVariableList* varList, JXExprEditor* menuProvider,
					JXExprEditor** exprWidget, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	virtual ~JXExprEditorSet();

private:

	// not allowed

	JXExprEditorSet(const JXExprEditorSet& source);
	const JXExprEditorSet& operator=(const JXExprEditorSet& source);
};

#endif
