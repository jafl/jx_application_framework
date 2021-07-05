/******************************************************************************
 CBShellEditor.h

	Copyright © 2009 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBShellEditor
#define _H_CBShellEditor

#include "CBTextEditor.h"

class CBShellDocument;

class CBShellEditor : public CBTextEditor
{
public:

	CBShellEditor(CBTextDocument* document, const JString& fileName,
				 JXMenuBar* menuBar, CBTELineIndexInput* lineInput,
				 CBTEColIndexInput* colInput,
				 JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	virtual ~CBShellEditor();

	void	InsertText(const JString& text);

	virtual void	HandleKeyPress(const JUtf8Character& c,
								   const int keySym, const JXKeyModifiers& modifiers) override;

private:

	CBShellDocument*		itsShellDoc;
	JStyledText::TextIndex	itsInsertIndex;
	JFont					itsInsertFont;

private:

	// not allowed

	CBShellEditor(const CBShellEditor& source);
	const CBShellEditor& operator=(const CBShellEditor& source);
};

#endif
