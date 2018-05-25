/******************************************************************************
 JXTextEditor.h

	Interface for the JXTextEditor class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXTextEditor
#define _H_JXTextEditor

#include <JXTEBase.h>

class JString;
class JXMenuBar;
class JXFontNameMenu;
class JXFontSizeMenu;
class JXTEStyleMenu;

class JXTextEditor : public JXTEBase
{
public:

	JXTextEditor(JStyledText* text, const JBoolean ownsText, JXMenuBar* menuBar,
				 JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	virtual ~JXTextEditor();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	// owned by the menu bar

	JXMenuBar*		itsMenuBar;
	JXFontNameMenu*	itsFontMenu;
	JXFontSizeMenu*	itsSizeMenu;
	JXTEStyleMenu*	itsStyleMenu;

private:

	void	AdjustMenuBar();

	// not allowed

	JXTextEditor(const JXTextEditor& source);
	const JXTextEditor& operator=(const JXTextEditor& source);
};

#endif
