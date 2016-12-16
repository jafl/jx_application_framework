/******************************************************************************
 GMAttachmentTable.cc

	BASE CLASS = public JXEditTable

	Copyright (C) 1999 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#include "GMAttachmentTable.h"
#include "GMMIMEParser.h"
#include "GMAttachDNDCursor.h"
#include "GMUpdateAttachTableWidth.h"
#include "GMCopyProcess.h"
#include <GMApp.h>

#include <GMGlobals.h>

#include "binary_file.xpm"

#include <JXFSBindingManager.h>

#include <JXDisplay.h>
#include <JXColormap.h>
#include <JXDNDManager.h>
#include <JXFileSelection.h>
#include <JXImage.h>
#include <JXSelectionManager.h>
#include <JXTEBase.h>
#include <JXTextMenu.h>
#include <JXWindow.h>
#include <JXChooseSaveFile.h>

#include <JPainter.h>
#include <JTableSelection.h>
#include <JTableSelectionIterator.h>

#include <jx_plain_file_small.xpm>
#include <jXUtil.h>

#include <jDirUtil.h>
#include <jProcessUtil.h>
#include <jASCIIConstants.h>

#include <jFStreamUtil.h>
#include <jAssert.h>

const JCoordinate kDefRowHeight	= 20;
const JCoordinate kDefColWidth	= 100;

const JCoordinate kIconColWidth	= 20;
const JCoordinate kFileColWidth	= 100;
const JCoordinate kHMarginWidth = 5;

const JCoordinate kNameIndex	= 1;

const JCoordinate kDebounceWidth	= 3;
static JCharacter* kAttachDirPrefix	= "gmimeattach";
static JCharacter* kDNDClassID		= "GMAttachmentTable";

/******************************************************************************
 Constructor

 *****************************************************************************/

GMAttachmentTable::GMAttachmentTable
	(
	const JBoolean		acceptDrop,
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
	JXTable(kDefRowHeight, kDefColWidth, scrollbarSet,
			enclosure, hSizing, vSizing, x,y, w,h),
	itsAcceptDrop(acceptDrop)
{
	itsMinColWidth	= 10;

	itsFileIcon	= new JXImage(GetDisplay(), jx_plain_file_small);
	assert(itsFileIcon != NULL);
	itsFileIcon->ConvertToRemoteStorage();

	itsBinaryIcon	= new JXImage(GetDisplay(), binary_file_xpm);
	assert(itsBinaryIcon != NULL);
	itsBinaryIcon->ConvertToRemoteStorage();

	if (!GetDisplay()->GetCursor(kGMAttachDNDCopyCursorName, &itsDNDCursor))
		{
		itsDNDCursor =
			GetDisplay()->CreateCustomCursor(kGMAttachDNDCopyCursorName, kGMAttachDNDCopyCursor);
		}

	SetRowBorderInfo(0, GetColormap()->GetBlackColor());
	SetColBorderInfo(0, GetColormap()->GetBlackColor());

	itsFiles	= new JPtrArray<JString>(JPtrArrayT::kForgetAll);
	assert(itsFiles != NULL);

	itsTypes	= new JArray<AttachmentType>;
	assert(itsTypes != NULL);

	itsEncoding	= new JArray<JBoolean>;
	assert(itsEncoding != NULL);

	SetSelectionBehavior(kJTrue, kJTrue);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GMAttachmentTable::~GMAttachmentTable()
{
	delete itsFileIcon;
	delete itsBinaryIcon;
	itsFiles->DeleteAll();
	delete itsFiles;
	delete itsTypes;
	delete itsEncoding;
	if (!itsAttachDir.IsEmpty())
		{
		JString cmd = "rm -rf " + itsAttachDir;
		JString errors;
		JRunProgram(cmd, &errors);
		}
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
GMAttachmentTable::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	HilightIfSelected(p, cell, rect);

	// draw icon

	JRect r = rect;
	r.right = r.left + kIconColWidth;
	if (itsEncoding->GetElement(cell.y))
		{
		p.Image(*itsBinaryIcon, itsBinaryIcon->GetBounds(), r);
		}
	else
		{
		p.Image(*itsFileIcon, itsFileIcon->GetBounds(), r);
		}

	// draw name

	r = rect;
	r.left += kIconColWidth + kHMarginWidth;

	JString path;
	JString name;
	JString filename	= *(itsFiles->NthElement(cell.y));
//	std::cout << filename << std::endl;
	JSplitPathAndName(filename, &path, &name);

	const JCoordinate w = p.GetStringWidth(name) + 2 * kHMarginWidth + kIconColWidth;
	if (w > GetColWidth(kNameIndex))
		{
		itsMinColWidth = w;
		GMUpdateAttachTableWidth* task = new GMUpdateAttachTableWidth(this);
		assert( task != NULL );
		task->Go();
		}

	p.String(r, name, JPainter::kHAlignLeft, JPainter::kVAlignCenter);
}

/******************************************************************************
 HandleKeyPress (public)

 ******************************************************************************/

void
GMAttachmentTable::HandleKeyPress
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint topSelCell;
	JTableSelection& s          = GetTableSelection();
	const JBoolean hadSelection = s.GetFirstSelectedCell(&topSelCell);

	if (key == ' ' || key == kJEscapeKey)
		{
		s.ClearSelection();
		}
	else if (key == kJReturnKey)
		{
		OpenSelectedAttachments();
		}
	else if (key == kJDeleteKey || key == kJForwardDeleteKey)
		{
		RemoveSelectedFiles();
		}

	else if (key == kJUpArrow || key == kJDownArrow)
		{
		if (!hadSelection && key == kJUpArrow && GetRowCount() > 0)
			{
			SelectSingleCell(JPoint(1, GetRowCount()), kJTrue);
			}
		else
			{
			HandleSelectionKeyPress(key, modifiers);
			}
		}
	else
		{
		JXTable::HandleKeyPress(key, modifiers);
		}

	TableRefresh();
	GetWindow()->Update();
}

/******************************************************************************
 HandleMouseDown (virtual protected)

	The default is to support scrolling via a wheel mouse.

 ******************************************************************************/

void
GMAttachmentTable::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	itsDownInCell		= kJFalse;
	JTableSelection& s	= GetTableSelection();
	const JBoolean noMod = JNegate( modifiers.shift() || modifiers.control() );

	JPoint cell;

	if (button > kJXRightButton)
		{
		ScrollForWheel(button, modifiers);
		}
	else if (!GetCell(pt, &cell))
		{
		s.ClearSelection();
		}
	else if (clickCount == 1 && button == kJXLeftButton && noMod)
		{
		itsDownInCell	= kJTrue;
		itsDownPt		= pt;
		if (!s.IsSelected(cell))
			{
			s.ClearSelection();
			s.SelectCell(cell);
			s.SetBoat(cell);
			s.SetAnchor(cell);
			}
		}
	else if (button < kJXRightButton && clickCount == 2)
		{
		OpenSelectedAttachments();
		}
	else
		{
		BeginSelectionDrag(cell, button, modifiers);
		}

	TableRefresh();
	GetWindow()->Update();
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
GMAttachmentTable::HandleMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint cell;
	if (itsDownInCell &&
		((JLAbs(pt.x - itsDownPt.x) > kDebounceWidth) ||
		 (JLAbs(pt.y - itsDownPt.y) > kDebounceWidth)))
		{
		JXFileSelection* data = new JXFileSelection(this, kDNDClassID);
		assert( data != NULL );
		BeginDND(pt, buttonStates, modifiers, data);
		}
	else if (GetCell(pt, &cell))
		{
		ContinueSelectionDrag(pt, modifiers);
		}
	TableRefresh();
}

/******************************************************************************
 HandleMouseUp (virtual protected)

 ******************************************************************************/

void
GMAttachmentTable::HandleMouseUp
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	FinishSelectionDrag();
	TableRefresh();
}

/******************************************************************************
 SetParser (public)

 ******************************************************************************/

void
GMAttachmentTable::SetParser
	(
	GMMIMEParser* parser
	)
{
	if (GetColCount() == 0)
		{
		AppendCols(1, kFileColWidth);
		}
	RemoveAllRows();
	itsFiles->DeleteAll();

	const JSize count = parser->GetAttachmentCount();
	AppendRows(count, kDefRowHeight);

	for (JIndex i=1; i<=count; i++)
		{
		JString* s = new JString(parser->GetAttachmentName(i));
		assert(s != NULL);
		itsFiles->Append(s);
		itsEncoding->AppendElement(kJFalse);
		}

	TableRefresh();
}

/******************************************************************************
 HandleDNDResponse (virtual protected)

	This is called when the target indicates whether or not it will accept
	the drop.  If !dropAccepted, the action is undefined.  If the drop target
	is not within the same application, target is NULL.

 ******************************************************************************/

void
GMAttachmentTable::HandleDNDResponse
	(
	const JXContainer*	target,
	const JBoolean		dropAccepted,
	const Atom			action
	)
{
	DisplayCursor(itsDNDCursor);
}

/******************************************************************************
 GetSelectionData (virtual protected)

	This is called when DND is terminated by a drop or when the target
	requests the data during a drag, but only if the delayed evaluation
	constructor for JXSelectionData was used.

	id is the string passed to the JXSelectionData constructor.

 ******************************************************************************/

void
GMAttachmentTable::GetSelectionData
	(
	JXSelectionData*	data,
	const JCharacter*	id
	)
{
	if (strcmp(id, kDNDClassID) == 0)
		{
		JXFileSelection* fileData = dynamic_cast<JXFileSelection*>(data);
		assert( fileData != NULL );

		JTableSelection& s = GetTableSelection();
		assert( s.HasSelection() );

		JPtrArray<JString> fileList(JPtrArrayT::kForgetAll);

		JTableSelectionIterator iter(&s);
		JPoint cell;
		while (iter.Next(&cell))
			{
			fileList.Append(itsFiles->NthElement(cell.y));
			}
		fileData->SetData(fileList);
		}
	else
		{
		JXTable::GetSelectionData(data, id);
		}
}

/******************************************************************************
 AdjustColWidth (private)

 ******************************************************************************/

void
GMAttachmentTable::AdjustColWidth()
{
	JCoordinate w = GetApertureWidth();
	if (w < itsMinColWidth)
		{
		w = itsMinColWidth;
		}
	SetColWidth(kNameIndex,w);
}

/******************************************************************************
 AddFile (public)

 ******************************************************************************/

void
GMAttachmentTable::AddFile
	(
	const JCharacter*	filename,
	const JBoolean		local
	)
{
	if (GetColCount() == 0)
		{
		AppendCols(1, kFileColWidth);
		}

	JString* s = new JString(filename);
	assert(s != NULL);

	if (local)
		{
		if (itsAttachDir.IsEmpty())
			{
			JString dir;
			JBoolean ok	= GMGetApplication()->GetFileDirectory(&dir);
			if (ok)
				{
				const JError err = JCreateTempDirectory(dir, kAttachDirPrefix, &itsAttachDir);
				if (err.OK())
					{
					const JString cmd = "cp \"" + JString(filename) + "\" " + itsAttachDir;
					JString errors;
					JRunProgram(cmd, &errors);
					if (errors.IsEmpty())
						{
						JString name;
						JString path;
						JSplitPathAndName(filename, &path, &name);
						*s = itsAttachDir + name;
						}
					else
						{
						JGetUserNotification()->ReportError(errors);
						}
					}
				else
					{
					err.ReportIfError();
					itsAttachDir.Clear();
					}
				}
			}
		}

	itsFiles->Append(s);
	AttachmentType type	= GetAttachmentType(*s);
	itsTypes->AppendElement(type);
	if (type > k7BitText)
		{
		itsEncoding->AppendElement(kJTrue);
		}
	else
		{
		itsEncoding->AppendElement(kJFalse);
		}
	AppendRows(1, kDefRowHeight);
}

/******************************************************************************
 WillAcceptDrop

 ******************************************************************************/

JBoolean
GMAttachmentTable::WillAcceptDrop
	(
	const JArray<Atom>& typeList,
	Atom*				action,
	const JPoint&		pt,
	const Time			time,
	const JXWidget*		source
	)
{
	if (!itsAcceptDrop ||
		(source == this))
		{
		return kJFalse;
		}
	JXSelectionManager* selMgr	= GetSelectionManager();

	const JSize typeCount = typeList.GetElementCount();
	for (JIndex i = 1; i <= typeCount; i++)
		{
		Atom type = typeList.GetElement(i);
		if (type == selMgr->GetURLXAtom())
			{
			return kJTrue;
			}
		}
	return kJFalse;
}

/******************************************************************************
 HandleDNDDrop

 ******************************************************************************/

void
GMAttachmentTable::HandleDNDDrop
	(
	const JPoint&		pt,
	const JArray<Atom>& typeList,
	const Atom			action,
	const Time			time,
	const JXWidget*	source
	)
{
	JBoolean dropOnSelf = kJTrue;
	if (source != this)
		{
		dropOnSelf = kJFalse;
		}

	HandleDNDLeave();
	JPoint cell;
	JBoolean isCell = GetCell(pt, &cell);
	if (isCell)
		{
		}
	unsigned char* data = NULL;
	JSize dataLength;
	Atom returnType;
	JXSelectionManager* selManager = GetSelectionManager();
	JXSelectionManager::DeleteMethod delMethod;
	const Atom dndName = GetDNDManager()->GetDNDSelectionName();
	if (selManager->GetData(dndName, time, selManager->GetURLXAtom(),
									 &returnType, &data, &dataLength, &delMethod))
		{
		JPtrArray<JString> fileNameList(JPtrArrayT::kForgetAll), urlList(JPtrArrayT::kForgetAll);
		JXUnpackFileNames((char*) data, dataLength, &fileNameList, &urlList);
		const JSize count	= fileNameList.GetElementCount();
		for (JIndex i = 1; i <= count; i++)
			{
			JBoolean local = JI2B(source != NULL);
			AddFile(*(fileNameList.NthElement(i)), local);
			}
		selManager->DeleteData(&data, delMethod);
		}
}

/******************************************************************************
 GetAttachmentName

 ******************************************************************************/

const JString&
GMAttachmentTable::GetAttachmentName
	(
	const JIndex	index,
	JBoolean*		binary
	)
	const
{
	assert(itsFiles->IndexValid(index));
	assert(itsEncoding->IndexValid(index));
	*binary	= itsEncoding->GetElement(index);
	return *(itsFiles->NthElement(index));
}

/******************************************************************************
 GetAttachmentType

 ******************************************************************************/

GMAttachmentTable::AttachmentType
GMAttachmentTable::GetAttachmentType
	(
	const JCharacter* filename
	)
{
	std::ifstream is(filename);
	assert(is.good());

	AttachmentType type	= k7BitText;

	JString data;
	const JSize bufferSize	= 50000;
	while (1)
		{
		data.Read(is, bufferSize);
		JSize count		= data.GetLength();
		for (JIndex i = 1; i <= count; i++)
			{
			unsigned char c	= data.GetCharacter(i);
			if (((c >= ' ') &&
				 (c <= '~')) ||
				 (c == '\t') ||
				 (c == '\n'))
				{
				// still 7Bit
				}
			else if (c > 127)
				{
				type	= k8BitText;
				}
			else
				{
				return kBinary;
				}
			}
		if (count < bufferSize)
			{
			break;
			}
		}

	return type;
}

/******************************************************************************
 RemoveSelectedFiles (public)

 ******************************************************************************/

void
GMAttachmentTable::RemoveSelectedFiles()
{
	JTableSelection& s = GetTableSelection();
	JTableSelectionIterator iter(&s);

	JPoint cell;
	while (iter.Next(&cell))
		{
		itsFiles->DeleteElement(cell.y);
		itsTypes->RemoveElement(cell.y);
		itsEncoding->RemoveElement(cell.y);
		RemoveRow(cell.y);
		}
}

/******************************************************************************
 SetEditMenuHandler (public)

 ******************************************************************************/

void
GMAttachmentTable::SetEditMenuHandler
	(
	JXTEBase* editor
	)
{
	itsEditMenuHandler	= editor;
	JXTextMenu* menu;
	JBoolean ok = itsEditMenuHandler->GetEditMenu(&menu);
	assert(ok);
	ListenTo(menu);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
GMAttachmentTable::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	JXTextMenu* menu;
	JBoolean ok = itsEditMenuHandler->GetEditMenu(&menu);
	assert(ok);

	if (sender == menu && message.Is(JXMenu::kNeedsUpdate))
		{
		if (HasFocus())
			{
			UpdateEditMenu();
			}
		}
	else if (sender == menu && message.Is(JXMenu::kItemSelected))
		{
		if (HasFocus())
			{
			const JXMenu::ItemSelected* selection =
				dynamic_cast<const JXMenu::ItemSelected*>(&message);
			assert( selection != NULL );
			HandleEditMenu(selection->GetIndex());
			}
		}
	JXTable::Receive(sender, message);
}

/******************************************************************************
 UpdateEditMenu (private)

 ******************************************************************************/

void
GMAttachmentTable::UpdateEditMenu()
{
	JXTextMenu* menu;
	JBoolean ok = itsEditMenuHandler->GetEditMenu(&menu);
	assert(ok);

	const JSize count = menu->GetItemCount();
	for (JIndex i=1; i<=count; i++)
		{
		JTextEditor::CmdIndex cmd;
		if (itsEditMenuHandler->EditMenuIndexToCmd(i, &cmd))
			{
			JBoolean enable = kJTrue;
			if (cmd == JTextEditor::kDeleteSelCmd && itsAcceptDrop)
				{
				menu->SetItemEnable(i, enable);
				}
			else if (cmd == JTextEditor::kSelectAllCmd)
				{
				menu->SetItemEnable(i, enable);
				}
			}
		}
}

/******************************************************************************
 HandleEditMenu (private)

 ******************************************************************************/

void
GMAttachmentTable::HandleEditMenu
	(
	const JIndex index
	)
{
	JXTextMenu* menu;
	JBoolean ok = itsEditMenuHandler->GetEditMenu(&menu);
	assert(ok);

	JTextEditor::CmdIndex cmd;
	if (itsEditMenuHandler->EditMenuIndexToCmd(index, &cmd))
		{
		if (cmd == JTextEditor::kDeleteSelCmd)
			{
			RemoveSelectedFiles();
			}
		else if (cmd == JTextEditor::kSelectAllCmd)
			{
			GetTableSelection().SelectAll();
			TableRefresh();
			}
		}
}

/******************************************************************************
 OpenSelectedAttachments (public)

 ******************************************************************************/

void
GMAttachmentTable::OpenSelectedAttachments()
{
	JTableSelection& s	= GetTableSelection();
	JTableSelectionIterator iter(&s);
	JPtrArray<JString> files(JPtrArrayT::kDeleteAll);
	JPoint cell;
	while (iter.Next(&cell))
		{
		JString* str	= new JString(*itsFiles->NthElement(cell.y));
		assert(str != NULL);
		files.Append(str);
		}

	JXFSBindingManager::Instance()->Exec(files, kJFalse);
}

/******************************************************************************
 SaveSelectedAttachments (public)

 ******************************************************************************/

void
GMAttachmentTable::SaveSelectedAttachments()
{
	JTableSelection& s = GetTableSelection();
	JPoint cell;
	if (s.GetSingleSelectedCell(&cell))
		{
		JBoolean binary;
		const JString& fullname = GetAttachmentName(cell.y, &binary);

		JString path, name, newname;
		JSplitPathAndName(fullname, &path, &name);

		if (JXGetChooseSaveFile()->SaveFile("Save attachment as:", "", name, &newname))
			{
			JString cmd	= "cp \"" + fullname + "\" \"" + newname + "\"";
			JString errors;
			JRunProgram(cmd, &errors);
			if (!errors.IsEmpty())
				{
				JGetUserNotification()->ReportError(errors);
				}
			}
		}
	else
		{
		JString path;
		if (JXGetChooseSaveFile()->ChooseRWPath("Save attachments in:", "", "", &path))
			{
			JTableSelectionIterator iter(&s);
			JPtrArray<JString>* files = new JPtrArray<JString>(JPtrArrayT::kDeleteAll);
			assert(files != NULL);

			while (iter.Next(&cell))
				{
				JBoolean binary;
				files->Append(GetAttachmentName(cell.y, &binary));
				}

			GMCopyProcess::Copy(files, path);
			}
		}
}
