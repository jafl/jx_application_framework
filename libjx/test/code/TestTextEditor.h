/******************************************************************************
 TestTextEditor.h

	Interface for the TestTextEditor class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestTextEditor
#define _H_TestTextEditor

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXTextEditor.h>

class JXPSPrinter;

class TestTextEditor : public JXTextEditor
{
public:

	TestTextEditor(const JBoolean editable, JXMenuBar* menuBar,
				   JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	virtual ~TestTextEditor();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JXPSPrinter* itsPrinter;

	JIndex	itsAutoIndentCmdIndex;		// item appended to edit menu
	JIndex	itsFirstUndoDepthCmdIndex;	// first undo depth

private:

	void		UpdateCustomEditMenuItems();
	JBoolean	HandleCustomEditMenuItems(const JIndex index);

	// not allowed

	TestTextEditor(const TestTextEditor& source);
	const TestTextEditor& operator=(const TestTextEditor& source);
};

#endif
