/******************************************************************************
 JXTextEditorSet.h

	Interface for the JXTextEditorSet class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXTextEditorSet
#define _H_JXTextEditorSet

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWidgetSet.h>

class JXMenuBar;
class JXTextEditor;

class JXTextEditorSet : public JXWidgetSet
{
public:

	JXTextEditorSet(JXTextEditor** textEditor, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	JXTextEditorSet(JXMenuBar* menuBar,
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
