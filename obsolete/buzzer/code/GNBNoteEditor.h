/******************************************************************************
 GNBNoteEditor.h

	Interface for the GNBNoteEditor class

	Copyright © 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_GNBNoteEditor
#define _H_GNBNoteEditor

#include <JXTextEditor.h>

class JString;
class JXXFontMenu;
class JXFontSizeMenu;
class JXToolBar;
class GNBNoteDir;

class GNBNoteEditor : public JXTextEditor//JXTEBase
{
public:

	GNBNoteEditor(GNBNoteDir* dir, JXMenuBar* menuBar, 
				 JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	virtual ~GNBNoteEditor();

	void	Print();
	void	LoadDefaultToolBar(JXToolBar* toolBar);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	GNBNoteDir* itsDir;

private:

	// not allowed

	GNBNoteEditor(const GNBNoteEditor& source);
	const GNBNoteEditor& operator=(const GNBNoteEditor& source);

public:

	// JBroadcaster messages

	static const JCharacter* kFontWillChange;

	class FontWillChange : public JBroadcaster::Message
	{
	public:

		FontWillChange()
			:
			JBroadcaster::Message(kFontWillChange)
			{ };
	};
};

#endif
