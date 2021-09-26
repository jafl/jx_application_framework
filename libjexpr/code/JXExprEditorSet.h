/******************************************************************************
 JXExprEditorSet.h

	Interface for the JXExprEditorSet class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXExprEditorSet
#define _H_JXExprEditorSet

#include <jx-af/jx/JXWidgetSet.h>

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
};

#endif
