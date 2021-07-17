/******************************************************************************
 JXTextEditorSet.h

	Interface for the JXTextEditorSet class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXTextEditorSet
#define _H_JXTextEditorSet

#include "JXWidgetSet.h"

class JStyledText;
class JXMenuBar;
class JXTextEditor;

class JXTextEditorSet : public JXWidgetSet
{
public:

	JXTextEditorSet(JStyledText* text, const bool ownsText,
					JXTextEditor** textEditor, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	JXTextEditorSet(JXMenuBar* menuBar, JStyledText* text, const bool ownsText,
					JXTextEditor** textEditor, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	virtual ~JXTextEditorSet();

private:

	// not allowed

	JXTextEditorSet(const JXTextEditorSet& source);
	const JXTextEditorSet& operator=(const JXTextEditorSet& source);
};

#endif
