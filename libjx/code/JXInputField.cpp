/******************************************************************************
 JXInputField.cpp

	Maintains a line of editable text.

	BASE CLASS = JXTEBase

	Copyright (C) 1996-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXInputField.h>
#include <JXTextMenu.h>
#include <JXEditTable.h>
#include <JXWindowPainter.h>
#include <JXColormap.h>
#include <jXConstants.h>
#include <jXGlobals.h>
#include <jXKeysym.h>
#include <jAssert.h>

// Context menu

struct MenuItemInfo
{
	JTextEditor::CmdIndex	cmd;
	const JCharacter*		id;
};

#define kContextUndoAction		"ContextUndo::JXInputField"
#define kContextCutAction		"ContextCut::JXInputField"
#define kContextCopyAction		"ContextCopy::JXInputField"
#define kContextPasteAction		"ContextPaste::JXInputField"
#define kContextClearAction		"ContextClear::JXInputField"
#define kContextSelectAllAction	"ContextSelectAll::JXInputField"

static const JCharacter* kMacContextMenuStr =
	"    Undo       %k Meta-Z. %i" kContextUndoAction
	"%l| Cut        %k Meta-X. %i" kContextCutAction
	"  | Copy       %k Meta-C. %i" kContextCopyAction
	"  | Paste      %k Meta-V. %i" kContextPasteAction
	"  | Clear                 %i" kContextClearAction
	"%l| Select All %k Meta-A. %i" kContextSelectAllAction;

static const JCharacter* kWinContextMenuStr =
	"    Undo       %k Ctrl-Z. %i" kContextUndoAction
	"%l| Cut        %k Ctrl-X. %i" kContextCutAction
	"  | Copy       %k Ctrl-C. %i" kContextCopyAction
	"  | Paste      %k Ctrl-V. %i" kContextPasteAction
	"  | Clear                 %i" kContextClearAction
	"%l| Select All %k Ctrl-A. %i" kContextSelectAllAction;

static const MenuItemInfo kContextMenuItemInfo[] =
{
	{ JTextEditor::kUndoCmd,      kContextUndoAction      },
	{ JTextEditor::kCutCmd,       kContextCutAction       },
	{ JTextEditor::kCopyCmd,      kContextCopyAction      },
	{ JTextEditor::kPasteCmd,     kContextPasteAction     },
	{ JTextEditor::kDeleteSelCmd, kContextClearAction     },
	{ JTextEditor::kSelectAllCmd, kContextSelectAllAction }
};
const JSize kContextMenuItemCount = sizeof(kContextMenuItemInfo)/sizeof(MenuItemInfo);

/******************************************************************************
 Constructor

 ******************************************************************************/

JXInputField::JXInputField
	(
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXTEBase(kFullEditor, kJTrue, kJFalse, NULL,
			 enclosure, hSizing, vSizing, x,y, w,h)
{
	JXInputFieldX(kJFalse);
}

JXInputField::JXInputField
	(
	const JBoolean		wordWrap,
	const JBoolean		acceptNewline,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXTEBase(kFullEditor, !wordWrap, kJFalse, NULL,
			 enclosure, hSizing, vSizing, x,y, w,h)
{
	JXInputFieldX(acceptNewline);
}

// private

void
JXInputField::JXInputFieldX
	(
	const JBoolean acceptNewline
	)
{
	itsAcceptNewlineFlag = acceptNewline;
	itsMinLength         = 0;
	itsMaxLength         = 0;

	itsContextMenu = NULL;
	itsTable       = NULL;

	SetDefaultFontSize(JGetDefaultFontSize());
	TESetLeftMarginWidth(kMinLeftMarginWidth);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXInputField::~JXInputField()
{
}

/******************************************************************************
 ToString (virtual)

 ******************************************************************************/

JString
JXInputField::ToString()
	const
{
	return JXTEBase::ToString() + ": " + GetText();
}

/******************************************************************************
 SetTable

	This can only be called once.

 ******************************************************************************/

void
JXInputField::SetTable
	(
	JXEditTable* table
	)
{
	assert( itsTable == NULL && table != NULL );

	itsTable = table;
	WantInput(kJTrue, kJTrue);
	SetBorderWidth(1);
}

/******************************************************************************
 Change font

 ******************************************************************************/

void
JXInputField::SetFontName
	(
	const JCharacter* name
	)
{
	if (!IsEmpty())
		{
		JXTEBase::SetFontName(1, GetTextLength(), name, kJTrue);
		}

	SetDefaultFontName(name);
}

void
JXInputField::SetFontSize
	(
	const JSize size
	)
{
	if (!IsEmpty())
		{
		JXTEBase::SetFontSize(1, GetTextLength(), size, kJTrue);
		}

	SetDefaultFontSize(size);
}

void
JXInputField::SetFontStyle
	(
	const JFontStyle& style
	)
{
	if (!IsEmpty())
		{
		JXTEBase::SetFontStyle(1, GetTextLength(), style, kJTrue);
		}

	SetDefaultFontStyle(style);
}

void
JXInputField::SetFont
	(
	const JFont& font
	)
{
	if (!IsEmpty())
		{
		JXTEBase::SetFont(1, GetTextLength(), font, kJTrue);
		}

	SetDefaultFont(font);
}

/******************************************************************************
 SetMinLength

 ******************************************************************************/

void
JXInputField::SetMinLength
	(
	const JSize minLength
	)
{
	assert( itsMaxLength == 0 || minLength <= itsMaxLength );
	itsMinLength = minLength;
}

/******************************************************************************
 SetMaxLength

 ******************************************************************************/

void
JXInputField::SetMaxLength
	(
	const JSize maxLength
	)
{
	assert( maxLength > 0 && itsMinLength <= maxLength );
	itsMaxLength = maxLength;
}

/******************************************************************************
 SetLengthLimits

 ******************************************************************************/

void
JXInputField::SetLengthLimits
	(
	const JSize minLength,
	const JSize maxLength
	)
{
	assert( maxLength > 0 && minLength <= maxLength );
	itsMinLength = minLength;
	itsMaxLength = maxLength;
}

/******************************************************************************
 HandleFocusEvent (virtual protected)

 ******************************************************************************/

void
JXInputField::HandleFocusEvent()
{
	JXTEBase::HandleFocusEvent();
	ClearUndo();
	SelectAll();
}

/******************************************************************************
 HandleUnfocusEvent (virtual protected)

 ******************************************************************************/

void
JXInputField::HandleUnfocusEvent()
{
	JXTEBase::HandleUnfocusEvent();
	ClearUndo();
}

/******************************************************************************
 OKToUnfocus (virtual protected)

 ******************************************************************************/

JBoolean
JXInputField::OKToUnfocus()
{
	if (!JXTEBase::OKToUnfocus())
		{
		return kJFalse;
		}
	else if (itsTable != NULL)
		{
		return itsTable->EndEditing();
		}
	else
		{
		return InputValid();
		}
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
JXInputField::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
	if (itsTable != NULL)
		{
		if (IsDNDTarget())
			{
			p.SetPenColor((p.GetColormap())->GetDefaultDNDBorderColor());
			}
		else
			{
			p.SetPenColor(GetColormap()->GetBlackColor());
			}
		p.JPainter::Rect(frame);
		}
	else
		{
		JXTEBase::DrawBorder(p, frame);
		}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
JXInputField::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (button == kJXRightButton)
		{
//		if (!PointInSelection(pt))
//			{
//			JXTEBase::HandleMouseDown(pt, kJXLeftButton, 1, buttonStates, modifiers);
//			}
		CreateContextMenu();
		itsContextMenu->PopUp(this, pt, buttonStates, modifiers);
		}
	else
		{
		ShouldAllowDragAndDrop(modifiers.meta());
		JXTEBase::HandleMouseDown(pt, button, clickCount, buttonStates, modifiers);
		}
}

/******************************************************************************
 HandleDNDDrop (virtual protected)

	This is called when the data is dropped.  We do not accept the drop unless
	we can first obtain focus, because we cannot otherwise guarantee that
	our constraints will be checked.

 ******************************************************************************/

void
JXInputField::HandleDNDDrop
	(
	const JPoint&		pt,
	const JArray<Atom>&	typeList,
	const Atom			action,
	const Time			time,
	const JXWidget*		source
	)
{
	if (Focus())
		{
		JXTEBase::HandleDNDDrop(pt, typeList, action, time, source);
		}
}

/******************************************************************************
 InputValid (virtual)

 ******************************************************************************/

JBoolean
JXInputField::InputValid()
{
	const JSize length  = GetTextLength();

	JString errorStr;
	if (itsMinLength == 1 && length == 0)
		{
		errorStr = JGetString("EmptyError::JXInputField");
		}
	else if (itsMinLength > 0 && itsMinLength == itsMaxLength &&
			 length != itsMinLength)
		{
		if (itsMaxLength == 1)
			{
			errorStr = JGetString("Require1Char::JXInputField");
			}
		else
			{
			const JString s(itsMinLength, 0);
			const JCharacter* map[] =
				{
				"count", s.GetCString()
				};
			errorStr = JGetString("RequireNChar::JXInputField", map, sizeof(map));
			}
		}
	else if (itsMinLength > 0 && itsMaxLength > 0 &&
			 (length < itsMinLength || itsMaxLength < length))
		{
		const JString n(itsMinLength, 0), m(itsMaxLength, 0);
		const JCharacter* map[] =
			{
			"min", n.GetCString(),
			"max", m.GetCString()
			};
		errorStr = JGetString("RangeNMChar::JXInputField", map, sizeof(map));
		}
	else if (itsMinLength > 0 && length < itsMinLength)
		{
		const JString n(itsMinLength, 0);
		const JCharacter* map[] =
			{
			"min", n.GetCString()	// itsMinLength > 1, see above
			};
		errorStr = JGetString("MinNChar::JXInputField", map, sizeof(map));
		}
	else if (itsMaxLength > 0 && length > itsMaxLength)
		{
		if (itsMaxLength == 1)
			{
			errorStr = JGetString("Max1Char::JXInputField");
			}
		else
			{
			const JString n(itsMaxLength, 0);
			const JCharacter* map[] =
				{
				"max", n.GetCString()
				};
			errorStr = JGetString("MaxNChar::JXInputField", map, sizeof(map));
			}
		}

	if (errorStr.IsEmpty())
		{
		return kJTrue;
		}
	else
		{
		(JGetUserNotification())->ReportError(errorStr);
		return kJFalse;
		}
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

void
JXInputField::HandleKeyPress
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsTable != NULL && itsTable->WantsInputFieldKey(key, modifiers))
		{
		itsTable->HandleKeyPress(key, modifiers);
		return;
		}
	else if (itsTable != NULL)
		{
		JPoint cell;
		const JBoolean ok = itsTable->GetEditedCell(&cell);
		assert( ok );
		itsTable->TableScrollToCell(cell);
		}

	if (key == JXCtrl('K') && modifiers.control() && HasSelection())
		{
		Cut();
		}
	else if (key == JXCtrl('K') && modifiers.control())
		{
		JIndex i;
		const JBoolean ok = GetCaretLocation(&i);
		assert( ok );
		SetSelection(i, GetTextLength());
		Cut();
		}

	else
		{
		JXTEBase::HandleKeyPress(key, modifiers);
		}
}

/******************************************************************************
 NeedsToFilterText (virtual protected)

	Derived classes should return kJTrue if FilterText() needs to be called.
	This is an optimization, to avoid copying the data if nothing needs to
	be done to it.

 ******************************************************************************/

JBoolean
JXInputField::NeedsToFilterText
	(
	const JCharacter* text
	)
	const
{
	return JI2B(JXTEBase::NeedsToFilterText(text) ||
				strchr(text, '\n') != NULL);
}

/******************************************************************************
 FilterText (virtual protected)

	Derived classes can override this to enforce restrictions on the text.
	Return kJFalse if the text cannot be used at all.

	*** Note that style may be NULL or empty if the data was plain text.

 ******************************************************************************/

JBoolean
JXInputField::FilterText
	(
	JString*			text,
	JRunArray<JFont>*	style
	)
{
	if (!JXTEBase::FilterText(text, style))
		{
		return kJFalse;
		}

	// convert newline to space

	if (!itsAcceptNewlineFlag)
		{
		const JSize length = text->GetLength();
		for (JIndex i=1; i<=length; i++)
			{
			if (text->GetCharacter(i) == '\n')
				{
				text->SetCharacter(i, ' ');
				}
			}
		}

	return kJTrue;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXInputField::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsContextMenu && message.Is(JXTextMenu::kNeedsUpdate))
		{
		UpdateContextMenu();
		}
	else if (sender == itsContextMenu && message.Is(JXTextMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleContextMenu(selection->GetIndex());
		}

	else
		{
		JXTEBase::Receive(sender, message);
		}
}

/******************************************************************************
 CreateContextMenu (private)

 ******************************************************************************/

void
JXInputField::CreateContextMenu()
{
	if (itsContextMenu == NULL)
		{
		itsContextMenu = jnew JXTextMenu("", this, kFixedLeft, kFixedTop, 0,0, 10,10);
		assert( itsContextMenu != NULL );
		if (JXMenu::GetDefaultStyle() == JXMenu::kMacintoshStyle)
			{
			itsContextMenu->SetMenuItems(kMacContextMenuStr, "JXInputField");
			}
		else
			{
			itsContextMenu->SetMenuItems(kWinContextMenuStr, "JXInputField");
			}
		itsContextMenu->SetUpdateAction(JXMenu::kDisableAll);
		itsContextMenu->SetToHiddenPopupMenu();
		ListenTo(itsContextMenu);
		}
}

/******************************************************************************
 UpdateContextMenu (private)

 ******************************************************************************/

void
JXInputField::UpdateContextMenu()
{
	JString crmActionText, crm2ActionText;
	JBoolean isReadOnly;
	const JArray<JBoolean> enableFlags =
		GetCmdStatus(&crmActionText, &crm2ActionText, &isReadOnly);

	const JSize count = itsContextMenu->GetItemCount();
	for (JIndex i=1; i<=count; i++)
		{
		CmdIndex cmd;
		if (ContextMenuIndexToCmd(i, &cmd))
			{
			JBoolean enable;
			if (cmd == JTextEditor::kDeleteSelCmd)
				{
				enable = kJTrue;
				}
			else
				{
				enable = enableFlags.GetElement(cmd);
				}
			itsContextMenu->SetItemEnable(i, enable);
			}
		}
}

/******************************************************************************
 HandleContextMenu (private)

 ******************************************************************************/

void
JXInputField::HandleContextMenu
	(
	const JIndex index
	)
{
	CmdIndex cmd;
	if (!ContextMenuIndexToCmd(index, &cmd))
		{
		return;
		}

	if (cmd == kUndoCmd)
		{
		Undo();
		}

	else if (cmd == kCutCmd)
		{
		Cut();
		}
	else if (cmd == kCopyCmd)
		{
		Copy();
		}
	else if (cmd == kPasteCmd)
		{
		Paste();
		}
	else if (cmd == kDeleteSelCmd)
		{
		if (!HasSelection())
			{
			SelectAll();
			}
		DeleteSelection();
		}

	else if (cmd == kSelectAllCmd)
		{
		SelectAll();
		}
}

/******************************************************************************
 Context menu index <-> cmd

 ******************************************************************************/

#define ClassName    JXInputField
#define IndexToCmdFn ContextMenuIndexToCmd
#define CmdToIndexFn ContextMenuCmdToIndex
#define MenuVar      itsContextMenu
#define CmdCount     kContextMenuItemCount
#define CmdIDList    kContextMenuItemInfo
#include <JXMenuItemIDUtil.th>
#undef ClassName
#undef IndexToCmdFn
#undef CmdToIndexFn
#undef MenuVar
#undef CmdCount
#undef CmdIDList

/******************************************************************************
 GetFTCMinContentSize (virtual protected)

 ******************************************************************************/

JCoordinate
JXInputField::GetFTCMinContentSize
	(
	const JBoolean horizontal
	)
	const
{
	if (!horizontal)
		{
		const JSize lineHeight = GetLineHeight(1);
		if (GetApertureHeight() < lineHeight)
			{
			return lineHeight;
			}
		}

	return JXTEBase::GetFTCMinContentSize(horizontal);
}
