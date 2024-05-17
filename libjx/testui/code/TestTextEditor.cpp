/******************************************************************************
 TestTextEditor.cpp

	BASE CLASS = JXTextEditor

	Written by John Lindal.

 ******************************************************************************/

#include "TestTextEditor.h"
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXPSPrinter.h>
#include <jx-af/jcore/JUndoRedoChain.h>
#include <jx-af/jcore/jAssert.h>

#include "TestTextEditor-Edit.h"

// Edit menu

static const JSize kUndoDepth[] = { 1, 2, 3, 100 };
const JSize kUndoDepthCount     = sizeof(kUndoDepth)/sizeof(JSize);

/******************************************************************************
 Constructor

 ******************************************************************************/

TestTextEditor::TestTextEditor
	(
	JStyledText*		text,
	const bool			ownsText,
	const bool			editable,
	JXMenuBar*			menuBar,
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXTextEditor(text, ownsText, menuBar, scrollbarSet,
				 enclosure, hSizing, vSizing, x,y, w,h),
	itsAutoIndentCmdIndex(0),
	itsFirstUndoDepthCmdIndex(0)
{
	if (!editable)
	{
		SetType(kSelectableText);
	}

	itsPrinter = jnew JXPSPrinter(GetDisplay());
	SetPSPrinter(itsPrinter);

	// adjust the edit menu

	JXTextMenu* editMenu;
	if (GetEditMenu(&editMenu))
	{
		const JSize editCount = editMenu->GetItemCount();
		editMenu->ShowSeparatorAfter(editCount);

		itsAutoIndentCmdIndex     = editCount + 1;
		itsFirstUndoDepthCmdIndex = itsAutoIndentCmdIndex + 1;
		editMenu->AppendMenuItems(kEditMenuStr);
		ConfigureEditMenu(editMenu, editCount);
	}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TestTextEditor::~TestTextEditor()
{
	jdelete itsPrinter;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
TestTextEditor::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	JXTextMenu* editMenu;
	GetEditMenu(&editMenu);

	if (sender == editMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdateCustomEditMenuItems();
	}
	else if (sender == editMenu && message.Is(JXMenu::kItemSelected))
	{
		auto& selection = dynamic_cast<const JXMenu::ItemSelected&>(message);
		if (HandleCustomEditMenuItems(selection.GetIndex()))
		{
			return;
		}
	}

	JXTextEditor::Receive(sender, message);
}

/******************************************************************************
 UpdateCustomEditMenuItems (private)

 ******************************************************************************/

void
TestTextEditor::UpdateCustomEditMenuItems()
{
	if (itsAutoIndentCmdIndex == 0)
	{
		return;
	}

	JXTextMenu* editMenu;
	const bool ok = GetEditMenu(&editMenu);
	assert( ok );

	editMenu->EnableItem(itsAutoIndentCmdIndex);
	if (GetText()->WillAutoIndent())
	{
		editMenu->CheckItem(itsAutoIndentCmdIndex);
	}

	const JSize undoDepth = GetText()->GetUndoRedoChain()->GetUndoDepth();
	for (JUnsignedOffset i=0; i<kUndoDepthCount; i++)
	{
		const JIndex itemIndex = itsFirstUndoDepthCmdIndex + i;
		editMenu->EnableItem(itemIndex);
		if (undoDepth == kUndoDepth[i])
		{
			editMenu->CheckItem(itemIndex);
		}
	}
}

/******************************************************************************
 HandleCustomEditMenuItems (private)

	Returns true if it is one of our items.

 ******************************************************************************/

bool
TestTextEditor::HandleCustomEditMenuItems
	(
	const JIndex index
	)
{
	if (index == itsAutoIndentCmdIndex)
	{
		GetText()->ShouldAutoIndent(!GetText()->WillAutoIndent());
		return true;
	}
	else if (itsFirstUndoDepthCmdIndex > 0 && index >= itsFirstUndoDepthCmdIndex)
	{
		GetText()->GetUndoRedoChain()->SetUndoDepth(kUndoDepth[ index - itsFirstUndoDepthCmdIndex ]);
		return true;
	}
	else
	{
		return false;
	}
}
