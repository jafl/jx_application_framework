/******************************************************************************
 JXInputField.cpp

	Maintains a line of editable text.

	BASE CLASS = JXTEBase

	Copyright (C) 1996-98 by John Lindal.

 ******************************************************************************/

#include <JXInputField.h>
#include <JXTextMenu.h>
#include <JXEditTable.h>
#include <JXWindowPainter.h>
#include <JXColorManager.h>
#include <jXConstants.h>
#include <jXGlobals.h>
#include <jXKeysym.h>
#include <JStringIterator.h>
#include <jAssert.h>

// Context menu

struct MenuItemInfo
{
	JTextEditor::CmdIndex	cmd;
	const JUtf8Byte*		id;
};

#define kContextUndoAction		"ContextUndo::JXInputField"
#define kContextCutAction		"ContextCut::JXInputField"
#define kContextCopyAction		"ContextCopy::JXInputField"
#define kContextPasteAction		"ContextPaste::JXInputField"
#define kContextClearAction		"ContextClear::JXInputField"
#define kContextSelectAllAction	"ContextSelectAll::JXInputField"

static const JUtf8Byte* kMacContextMenuStr =
	"    Undo       %k Meta-Z. %i" kContextUndoAction
	"%l| Cut        %k Meta-X. %i" kContextCutAction
	"  | Copy       %k Meta-C. %i" kContextCopyAction
	"  | Paste      %k Meta-V. %i" kContextPasteAction
	"  | Clear                 %i" kContextClearAction
	"%l| Select All %k Meta-A. %i" kContextSelectAllAction;

static const JUtf8Byte* kWinContextMenuStr =
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
	JXTEBase(kFullEditor, jnew StyledText(kJFalse), kJTrue, kJTrue, NULL,
			 enclosure, hSizing, vSizing, x,y, w,h)
{
	JXInputFieldX();
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
	JXTEBase(kFullEditor, jnew StyledText(acceptNewline), kJTrue, !wordWrap, NULL,
			 enclosure, hSizing, vSizing, x,y, w,h)
{
	JXInputFieldX();
}

// protected

JXInputField::JXInputField
	(
	StyledText*			text,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXTEBase(kFullEditor, text, kJTrue, kJTrue, NULL,
			 enclosure, hSizing, vSizing, x,y, w,h)
{
	JXInputFieldX();
}

// private

void
JXInputField::JXInputFieldX()
{
	itsMinLength = 0;
	itsMaxLength = 0;

	itsContextMenu = NULL;
	itsTable       = NULL;

	GetText()->SetDefaultFontSize(JGetDefaultFontSize());
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
	return JXTEBase::ToString() + JString(": ", 0, kJFalse) + GetText().GetText();
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
	const JString& name
	)
{
	if (!GetText()->IsEmpty())
		{
		GetText()->SetFontName(GetText()->SelectAll(), name, kJTrue);
		}

	GetText()->SetDefaultFontName(name);
}

void
JXInputField::SetFontSize
	(
	const JSize size
	)
{
	if (!GetText()->IsEmpty())
		{
		GetText()->SetFontSize(GetText()->SelectAll(), size, kJTrue);
		}

	GetText()->SetDefaultFontSize(size);
}

void
JXInputField::SetFontStyle
	(
	const JFontStyle& style
	)
{
	if (!GetText()->IsEmpty())
		{
		GetText()->SetFontStyle(GetText()->SelectAll(), style, kJTrue);
		}

	GetText()->SetDefaultFontStyle(style);
}

void
JXInputField::SetFont
	(
	const JFont& font
	)
{
	if (!GetText()->IsEmpty())
		{
		GetText()->SetFont(GetText()->SelectAll(), font, kJTrue);
		}

	GetText()->SetDefaultFont(font);
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
	GetText()->ClearUndo();
	SelectAll();
}

/******************************************************************************
 HandleUnfocusEvent (virtual protected)

 ******************************************************************************/

void
JXInputField::HandleUnfocusEvent()
{
	JXTEBase::HandleUnfocusEvent();
	GetText()->ClearUndo();
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
			p.SetPenColor(JColorManager::GetDefaultDNDBorderColor());
			}
		else
			{
			p.SetPenColor(JColorManager::GetBlackColor());
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
	const JSize length = GetText()->GetText().GetCharacterCount();

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
			const JUtf8Byte* map[] =
				{
				"count", s.GetBytes()
				};
			errorStr = JGetString("RequireNChar::JXInputField", map, sizeof(map));
			}
		}
	else if (itsMinLength > 0 && itsMaxLength > 0 &&
			 (length < itsMinLength || itsMaxLength < length))
		{
		const JString n(itsMinLength, 0), m(itsMaxLength, 0);
		const JUtf8Byte* map[] =
			{
			"min", n.GetBytes(),
			"max", m.GetBytes()
			};
		errorStr = JGetString("RangeNMChar::JXInputField", map, sizeof(map));
		}
	else if (itsMinLength > 0 && length < itsMinLength)
		{
		const JString n(itsMinLength, 0);
		const JUtf8Byte* map[] =
			{
			"min", n.GetBytes()	// itsMinLength > 1, see above
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
			const JUtf8Byte* map[] =
				{
				"max", n.GetBytes()
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
		SetSelection(JCharacterRange(i, GetText()->GetText().GetCharacterCount()));
		Cut();
		}

	else
		{
		JXTEBase::HandleKeyPress(key, modifiers);
		}
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
		itsContextMenu = jnew JXTextMenu(JString::empty, this, kFixedLeft, kFixedTop, 0,0, 10,10);
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
		GetText()->Undo();
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

/******************************************************************************
 NeedsToFilterText (virtual protected)

	Derived classes should return kJTrue if FilterText() needs to be called.
	This is an optimization, to avoid copying the data if nothing needs to
	be done to it.

 ******************************************************************************/

JBoolean
JXInputField::StyledText::NeedsToFilterText
	(
	const JString& text
	)
	const
{
	return text.Contains("\n");
}

/******************************************************************************
 FilterText (virtual protected)

	Derived classes can override this to enforce restrictions on the text.
	Return kJFalse if the text cannot be used at all.

	*** Note that style may be NULL or empty if the data was plain text.

 ******************************************************************************/

JBoolean
JXInputField::StyledText::FilterText
	(
	JString*			text,
	JRunArray<JFont>*	style
	)
{
	// convert newline to space

	if (!itsAcceptNewlineFlag)
		{
		JStringIterator iter(text);
		while (iter.Next("\n"))
			{
			iter.ReplaceLastMatch(" ");
			}
		}

	return kJTrue;
}
