/******************************************************************************
 SCExprEditorSet.h

	Interface for the SCExprEditorSet class

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SCExprEditorSet
#define _H_SCExprEditorSet

#include <JXWidgetSet.h>

class JXMenuBar;
class SCCircuitDocument;
class SCExprEditor;

class SCExprEditorSet : public JXWidgetSet
{
public:

	SCExprEditorSet(SCCircuitDocument* doc,
					SCExprEditor** exprWidget, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	SCExprEditorSet(SCCircuitDocument* doc, JXMenuBar* menuBar,
					SCExprEditor** exprWidget, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	SCExprEditorSet(SCCircuitDocument* doc, SCExprEditor* menuProvider,
					SCExprEditor** exprWidget, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	virtual ~SCExprEditorSet();

private:

	// not allowed

	SCExprEditorSet(const SCExprEditorSet& source);
	const SCExprEditorSet& operator=(const SCExprEditorSet& source);
};

#endif
