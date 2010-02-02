/******************************************************************************
 GMessageTable.cc


	Copyright © 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include "GMessageTable.h"
#include "GMessageTableDir.h"
#include "GMessageViewDir.h"
#include "GMessageEditDir.h"
#include "GMessageHeaderList.h"
#include "GMessageHeader.h"
#include "GMailboxTreeDir.h"
#include "GMMIMEParser.h"
#include "GMAccountManager.h"
#include "GMApp.h"
#include "GPrefsMgr.h"
#include <GMessageDragData.h>
#include "GMMailboxData.h"
#include "gMailUtils.h"

#include "envelope-front.xpm"
#include "check1.xpm"

#include "jx_edit_clear.xpm"
#include "cross_out.xpm"
#include "reply.xpm"
#include "reply_all.xpm"
#include "reply_sender.xpm"
#include "forward.xpm"
#include "redirect.xpm"
#include "paperclip.xpm"
#include "filefloppy.xpm"
#include "fileprint.xpm"

#include <JXColormap.h>
#include <JXDeleteObjectTask.h>
#include <JXDNDManager.h>
#include <JXFSDirMenu.h>
#include <JXDisplay.h>
#include <JXFontManager.h>
#include <JXImage.h>
#include <JXMenuBar.h>
#include <JXSelectionManager.h>
#include <JXStaticText.h>
#include <JXTextMenu.h>
#include <JXTimerTask.h>
#include <JDirInfo.h>
#include <JXWindow.h>
#include <JXWindowPainter.h>

#include <JTableSelection.h>
#include <JPainter.h>
#include <JFontManager.h>
#include <JColormap.h>

#include <GMGlobals.h>

#include <jASCIIConstants.h>
#include <jStreamUtil.h>
#include <jProcessUtil.h>
#include <jDirUtil.h>
#include <jFileUtil.h>

#include <X11/keysym.h>
#include <stdio.h>
#include <strstream>
#include <jAssert.h>

const JCoordinate	kDefRowHeight		= 15;
const JCoordinate	kDefColWidth		= 80;
const JCoordinate	kPriorityColWidth	= 18;
const JCoordinate	kIconColWidth		= 30;
const JCoordinate	kAttachColWidth		= 10;
const JCoordinate	kFromColWidth		= 160;
const JCoordinate	kSubjectColWidth	= 200;
const JCoordinate	kDateColWidth		= 100; // used to be 250;
const JCoordinate	kSizeColWidth		= 60;
const JCoordinate	kCellBuffer			= 10;

const JCoordinate	kPriorityIndex		= 1;
const JCoordinate	kIconIndex			= 2;
const JCoordinate	kAttachIndex		= 3;
const JCoordinate	kFromIndex			= 4;
const JCoordinate	kSubjectIndex		= 5;
const JCoordinate	kDateIndex			= 6;
const JCoordinate	kSizeIndex			= 7;

const JCoordinate kDragBeginBuffer				= 5;
static const JCharacter* kDragMessagesXAtomName = "GMailMessages";
static const JCharacter* kDNDActionCopyDescrip  = "copy the message";
static const JCharacter* kDNDActionMoveDescrip  = "move the message";

static const JCharacter* kDNDClassID			= "GMessageTable";

static const JCharacter* kMessageMenuTitleStr = "Message";
static const JCharacter* kMessageMenuStr =
	"Select all %k Meta-A  %i SelectAll::Arrow"
	"| Delete %k Meta-D %i Delete::Arrow"
	"| Undelete %k Meta-U %i Undelete::Arrow"
	"| Mark as new %i MarkNew::Arrow"
	"| Mark as read %i MarkRead::Arrow"
	"%l|Reply %k Meta-R %i Reply::Arrow"
	"| Reply to sender %i ReplyToSender::Arrow"
	"|Reply to all %k Meta-Shift-R %i ReplyToAll::Arrow"
	"| Forward %k Meta-F %i Forward::Arrow"
	"| Redirect %i Redirect::Arrow";

enum
{
	kSelectAllCmd = 1,
	kDeleteCmd,
	kUndeleteCmd,
	kMarkNewCmd,
	kMarkReadCmd,
	kReplyCmd,
	kReplySenderCmd,
	kReplyAllCmd,
	kForwardCmd,
	kRedirectCmd
};

static const JCharacter* kPopupMenuTitleStr = "Message";
static const JCharacter* kPopupMenuStr =
	"   Delete"
	"  |Undelete"
	"  |Mark as read"
	"  |Mark as new"
	"%l|Reply"
	"  |Reply to sender"
	"  |Reply to all"
	"  |Forward"
	"  |Redirect"
	"%l|Save messages..."
	"  |Append messages..."
	"  |Transfer to"
	"  |Copy to"
	"%l|Print"
	;

enum
{
	kDeletePopCmd = 1,
	kUndeletePopCmd,
	kMarkReadPopCmd,
	kMarkNewPopCmd,
	kReplyPopCmd,
	kReplySenderPopCmd,
	kReplyAllPopCmd,
	kForwardPopCmd,
	kRedirectPopCmd,
	kSaveMsgPopCmd,
	kAppendMsgPopCmd,
	kTransferToPopCmd,
	kCopyToPopCmd,
	kPrintMsgPopCmd
};

//#define JTemplateType JXTimerTask
//#include <JXDeleteObjectTask.tmpls>

/******************************************************************************
 Constructor

 ******************************************************************************/

GMessageTable::GMessageTable
	(
	GMessageTableDir*	dir,
	JXMenuBar*			menuBar,
	JXScrollbarSet*	scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption hSizing,
	const VSizingOption vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXTable(kDefRowHeight, kDefColWidth, scrollbarSet, enclosure,
			hSizing, vSizing, x, y, w, h)
{
	itsDir				= dir;
	itsData				= NULL;
	itsUnlockTask		= NULL;
	itsHasLockedMBox	= kJFalse;

	itsMessageMenu = menuBar->AppendTextMenu(kMessageMenuTitleStr);
	itsMessageMenu->SetMenuItems(kMessageMenuStr);
	itsMessageMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsMessageMenu);

	itsPopupMenu =
		new JXTextMenu("", this, kFixedLeft, kFixedTop, 0,0,10,10);
	assert(itsPopupMenu != NULL);
	itsPopupMenu->CompressHeight();
	itsPopupMenu->SetMenuItems(kPopupMenuStr);
	itsPopupMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsPopupMenu->SetToHiddenPopupMenu();
	itsPopupMenu->CompressHeight();
	ListenTo(itsPopupMenu);

	JPtrArray<JString> nodes(JPtrArrayT::kDeleteAll);
	GGetMailboxTreeDir()->GetTopLevelNodes(&nodes);

	itsTransferMenu = new JXFSDirMenu(nodes, itsPopupMenu, kTransferToPopCmd, this);
	assert(itsTransferMenu != NULL);
	ListenTo(itsTransferMenu);

	JDirInfo* info;
	if (itsTransferMenu->GetDirInfo(&info))
		{
		info->SetContentFilter(GMGetMailRegexStr());
		}

	itsCopyMenu = new JXFSDirMenu(nodes, itsPopupMenu, kCopyToPopCmd, this);
	assert(itsCopyMenu != NULL);
	ListenTo(itsCopyMenu);

	if (itsCopyMenu->GetDirInfo(&info))
		{
		info->SetContentFilter(GMGetMailRegexStr());
		}

	ListenTo(GGetMailboxTreeDir());

	AppendCols(1, kPriorityColWidth);
	AppendCols(1, kIconColWidth);
	AppendCols(1, kAttachColWidth);
	AppendCols(1, kFromColWidth);
	AppendCols(1, kSubjectColWidth);
	AppendCols(1, kDateColWidth);
	AppendCols(1, kSizeColWidth);
	SetRowBorderInfo(0, GetColormap()->GetBlackColor());
	SetColBorderInfo(0, GetColormap()->GetBlackColor());
	SetBackColor(GetColormap()->GetWhiteColor());

	JCoordinate ascent, descent;
	itsLineHeight =
		(GetFontManager())->GetLineHeight(JGetDefaultFontName(),
		12, JFontStyle(), &ascent, &descent) + 5;

	itsMailIcon = new JXImage(GetDisplay(), GetColormap(), JXPM(envelope_front));
	assert( itsMailIcon != NULL );
	itsMailIcon->ConvertToRemoteStorage();

	itsCheckIcon = new JXImage(GetDisplay(), GetColormap(), JXPM(check1_xpm));
	assert( itsCheckIcon != NULL );
	itsCheckIcon->ConvertToRemoteStorage();

	itsClipIcon = new JXImage(GetDisplay(), GetColormap(), JXPM(paperclip));
	assert( itsClipIcon != NULL );
	itsClipIcon->ConvertToRemoteStorage();

	itsLastSelectedIndex = 0;

	itsMessageXAtom = GetDisplay()->RegisterXAtom(kDragMessagesXAtomName);
	itsDNDDropIndex = 0;

	itsMessageMenu->SetItemImage(kDeleteCmd, jx_edit_clear);
	itsMessageMenu->SetItemImage(kReplyCmd, reply_xpm);
	itsMessageMenu->SetItemImage(kReplySenderCmd, reply_sender_xpm);
	itsMessageMenu->SetItemImage(kReplyAllCmd, reply_all_xpm);
	itsMessageMenu->SetItemImage(kForwardCmd, forward_xpm);
	itsMessageMenu->SetItemImage(kRedirectCmd, redirect_xpm);

	itsPopupMenu->SetItemImage(kDeletePopCmd, jx_edit_clear);
	itsPopupMenu->SetItemImage(kReplyPopCmd, reply_xpm);
	itsPopupMenu->SetItemImage(kReplySenderPopCmd, reply_sender_xpm);
	itsPopupMenu->SetItemImage(kReplyAllPopCmd, reply_all_xpm);
	itsPopupMenu->SetItemImage(kForwardPopCmd, forward_xpm);
	itsPopupMenu->SetItemImage(kRedirectPopCmd, redirect_xpm);
	itsPopupMenu->SetItemImage(kSaveMsgPopCmd, filefloppy);
	itsPopupMenu->SetItemImage(kPrintMsgPopCmd, fileprint);

	itsReplyIcon = new JXImage(GetDisplay(), GetColormap(), JXPM(reply_xpm));
	assert( itsReplyIcon != NULL );
	itsReplyIcon->ConvertToRemoteStorage();

	itsReplySenderIcon = new JXImage(GetDisplay(), GetColormap(), JXPM(reply_sender_xpm));
	assert( itsReplySenderIcon != NULL );
	itsReplySenderIcon->ConvertToRemoteStorage();

	itsReplyAllIcon = new JXImage(GetDisplay(), GetColormap(), JXPM(reply_all_xpm));
	assert( itsReplyAllIcon != NULL );
	itsReplyAllIcon->ConvertToRemoteStorage();

	itsForwardIcon = new JXImage(GetDisplay(), GetColormap(), JXPM(forward_xpm));
	assert( itsForwardIcon != NULL );
	itsForwardIcon->ConvertToRemoteStorage();

	itsRedirectIcon = new JXImage(GetDisplay(), GetColormap(), JXPM(redirect_xpm));
	assert( itsRedirectIcon != NULL );
	itsRedirectIcon->ConvertToRemoteStorage();

	JXKeyModifiers mods(GetDisplay());
	
	GetWindow()->InstallShortcuts(this, "#_");
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GMessageTable::~GMessageTable()
{
	delete itsMailIcon;
	delete itsCheckIcon;
	delete itsClipIcon;
	delete itsReplyIcon;
	delete itsReplySenderIcon;
	delete itsReplyAllIcon;
	delete itsForwardIcon;
	delete itsRedirectIcon;
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
GMessageTable::Draw
	(
	JXWindowPainter& p,
	const JRect& rect
	)
{
	JXTable::Draw(p, rect);
	if (itsData != NULL && !itsData->CanReorganize())
		{
		return;
		}

	if (RowIndexValid(itsDNDDropIndex))
		{
		const JCoordinate y = GetRowTop(itsDNDDropIndex);
		const JRect ap      = GetAperture();

		p.ResetClipRect();
		p.Line(ap.left, y, ap.right, y);
		}
	if (itsDNDDropIndex == GetRowCount() + 1)
		{
		const JRect ap     = GetAperture();
		const JRect bounds = GetBounds();

		p.ResetClipRect();
		p.Line(ap.left, bounds.bottom-1, ap.right, bounds.bottom-1);
		}
}

/******************************************************************************
 Receive

 ******************************************************************************/

void
GMessageTable::Receive
	(
	JBroadcaster* sender,
	const Message& message
	)
{
	if (sender == itsData)
		{
		if (message.Is(GMMailboxData::kHeaderRemoved))
			{
			const GMMailboxData::HeaderRemoved* info = 
				dynamic_cast(const GMMailboxData::HeaderRemoved*, &message);
			assert(info != NULL);
			JIndex findex;
			if (itsData->Includes(info->GetHeader(), &findex))
				{
				RemoveNextRows(findex, 1);
				if (GetRowCount() < itsLastSelectedIndex)
					{
					itsLastSelectedIndex = 0;
					}
				}
			}
		else if (message.Is(GMMailboxData::kHeaderAdded))
			{
			const GMMailboxData::HeaderAdded* info = 
				dynamic_cast(const GMMailboxData::HeaderAdded*, &message);
			assert(info != NULL);
			JIndex findex;
			if (itsData->Includes(info->GetHeader(), &findex))
				{
				InsertRows(findex, 1, itsLineHeight);
				}
			}
		else if (message.Is(GMMailboxData::kSortChanged))
			{
			GetTableSelection().ClearSelection();
			}
		TableRefresh();
		}
	else if (sender == itsMessageMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateMessageMenu();
		}
	else if (sender == itsMessageMenu && message.Is(JXMenu::kItemSelected))
		{
		 const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		HandleMessageMenu(selection->GetIndex());
		}
	else if (sender == itsUnlockTask && message.Is(JXTimerTask::kTimerWentOff))
		{
		if (!IsDNDSource())
			{
			JString lockfile = itsData->GetMailFile() + ".lock";
			if (JFileExists(lockfile))
				{
				GUnlockFile(itsData->GetMailFile());
				itsHasLockedMBox	= kJFalse;
				}
			JXDeleteObjectTask<JBroadcaster>::Delete(itsUnlockTask);
			itsUnlockTask = NULL;
			}
		}
	else if (sender == itsPopupMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* info =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert(info != NULL);
		HandlePopupMenu(info->GetIndex());
		}
	else if (sender == itsTransferMenu && message.Is(JXFSDirMenu::kFileSelected))
		{
		const JXFSDirMenu::FileSelected* info =
			dynamic_cast(const JXFSDirMenu::FileSelected*, &message);
		assert(info != NULL);
		HandleMessageTransfer(info->GetFileName(), kJTrue);
		}
	else if (sender == itsCopyMenu && message.Is(JXFSDirMenu::kFileSelected))
		{
		const JXFSDirMenu::FileSelected* info =
			dynamic_cast(const JXFSDirMenu::FileSelected*, &message);
		assert(info != NULL);
		HandleMessageTransfer(info->GetFileName(), kJFalse);
		}
	else if (sender == GGetMailboxTreeDir() && message.Is(GMailboxTreeDir::kMailTreeChanged))
		{
		JPtrArray<JString> nodes(JPtrArrayT::kDeleteAll);
		GGetMailboxTreeDir()->GetTopLevelNodes(&nodes);
		itsTransferMenu->SetFileList(nodes);
		itsCopyMenu->SetFileList(nodes);
		}
	else
		{
		JXTable::Receive(sender, message);
		}
}

/******************************************************************************
 HandleKeyPress

 ******************************************************************************/

void
GMessageTable::HandleKeyPress
	(
	const int key,
	const JXKeyModifiers& modifiers
	)
{
	if ((key == kJEscapeKey) || (key == ' '))
		{
		itsLastSelectedIndex = 0;
		(GetTableSelection()).ClearSelection();
		TableRefresh();
		}
	else if (key == kJForwardDeleteKey)
		{
		JTableSelection& selection = GetTableSelection();
		JTableSelectionIterator* iter =
			new JTableSelectionIterator(&selection);
		JPoint cell;
		while(iter->Next(&cell))
			{
			GMessageHeader* header = itsData->GetHeader(cell.y);
			itsData->ChangeMessageStatus(header, GMessageHeader::kDelete);
			}
		TableRefresh();
		delete iter;
		}
	else if (key == kJUpArrow && !modifiers.control())
		{
		JTableSelection& selection = GetTableSelection();
		JTableSelectionIterator* iter =
			new JTableSelectionIterator(&selection);
		JPoint cell;
		if (modifiers.shift() && RowIndexValid(itsLastSelectedIndex))
			{
			cell.x = 1;
			cell.y = itsLastSelectedIndex;
			if(selection.IsSelected(cell))
				{
				do
					{
					cell.y++;
					}
				while(cell.y <= (JCoordinate)GetRowCount() && selection.IsSelected(cell));

				if (cell.y > (JCoordinate)itsLastSelectedIndex + 1) //shrink
					{
					JCoordinate last = cell.y - 2;
					selection.ClearSelection();
					for (cell.y = itsLastSelectedIndex; cell.y <= last; cell.y++)
						{
						selection.SelectCell(cell);
						}
					cell.y = last;
					TableScrollToCell(cell, kJFalse);
					TableRefresh();
					}
				else //expand
					{
					cell.y = itsLastSelectedIndex - 1;
					while (cell.y >=1 && selection.IsSelected(cell))
						{
						cell.y--;
						}
					if (cell.y >= 1)
						{
						JCoordinate first = cell.y;
						for (cell.y = first; cell.y <= (JCoordinate)itsLastSelectedIndex; cell.y++)
							{
							selection.SelectCell(cell);
							}
						cell.y = first;
						TableScrollToCell(cell, kJFalse);
						TableRefresh();
						}
					}
				}
			}
		else if (modifiers.meta())
			{
			cell.x = 1;
			cell.y = 1;
			selection.ClearSelection();
			selection.SelectCell(cell);
			assert(CellValid(cell));
			TableScrollToCell(cell, kJFalse);
			itsLastSelectedIndex = 1;
			TableRefresh();
			}
		else
			{
			if(iter->Next(&cell))
				{
				if (cell.y > 1)
					{
					selection.ClearSelection();
					cell.y--;
					selection.SelectCell(cell);
					assert(CellValid(cell));
					TableScrollToCell(cell, kJFalse);
					itsLastSelectedIndex = cell.y;
					TableRefresh();
					}
				else if (cell.y == 1)
					{
					selection.ClearSelection();
					selection.SelectCell(cell);
					assert(CellValid(cell));
					TableScrollToCell(cell, kJFalse);
					itsLastSelectedIndex = cell.y;
					TableRefresh();
					}
				}
			else if (GetRowCount() >= 1)
				{
				cell.x = 1;
				cell.y = GetRowCount();
				selection.SelectCell(cell);
				assert(CellValid(cell));
				TableScrollToCell(cell, kJFalse);
				itsLastSelectedIndex = cell.y;
				TableRefresh();
				}
			}
		delete iter;
		}
	else if (key == kJDownArrow && !modifiers.control())
		{
		JTableSelection& selection = GetTableSelection();
		JTableSelectionIterator* iter =
			new JTableSelectionIterator(&selection);
		JPoint cell;
		if (modifiers.shift())
			{
			cell.x = 1;
			cell.y = itsLastSelectedIndex;
			if(selection.IsSelected(cell))
				{
				do
					{
					cell.y--;
					}
				while(cell.y >= 1 && selection.IsSelected(cell));

				if (cell.y < (JCoordinate)itsLastSelectedIndex - 1) //shrink
					{
					JCoordinate first = cell.y + 2;
					selection.ClearSelection();
					for (cell.y = first; cell.y <= (JCoordinate)itsLastSelectedIndex; cell.y++)
						{
						selection.SelectCell(cell);
						}
					cell.y = first;
					TableScrollToCell(cell, kJFalse);
					TableRefresh();
					}
				else //expand
					{
					cell.y = itsLastSelectedIndex + 1;
					while (cell.y <= (JCoordinate)GetRowCount() && selection.IsSelected(cell))
						{
						cell.y++;
						}
					if (cell.y <= (JCoordinate)GetRowCount())
						{
						JCoordinate last = cell.y;
						selection.ClearSelection();
						for (cell.y = itsLastSelectedIndex; cell.y <= last; cell.y++)
							{
							selection.SelectCell(cell);
							}
						cell.y = last;
						TableScrollToCell(cell, kJFalse);
						TableRefresh();
						}
					}
				}

			}
		else if (modifiers.meta())
			{
			cell.x = 1;
			cell.y = GetRowCount();
			selection.ClearSelection();
			selection.SelectCell(cell);
			assert(CellValid(cell));
			TableScrollToCell(cell, kJFalse);
			itsLastSelectedIndex = cell.y;
			TableRefresh();
			}
		else
			{
			if(iter->Next(&cell))
				{
				if (cell.y < (JCoordinate)GetRowCount())
					{
					selection.ClearSelection();
					cell.y++;
					selection.SelectCell(cell);
					assert(CellValid(cell));
					TableScrollToCell(cell, kJFalse);
					itsLastSelectedIndex = cell.y;
					TableRefresh();
					}
				}
			else if (GetRowCount() >= 1)
				{
				cell.x = 1;
				cell.y = 1;
				selection.SelectCell(cell);
				assert(CellValid(cell));
				TableScrollToCell(cell, kJFalse);
				itsLastSelectedIndex = cell.y;
				TableRefresh();
				}
			}
		delete iter;
		}
	else if (key == kJReturnKey)
		{
		JTableSelection& selection = GetTableSelection();
		JTableSelectionIterator* iter =
			new JTableSelectionIterator(&selection);
		JPoint cell;
		while(iter->Next(&cell))
			{
			GMessageHeader* header = itsData->GetHeader(cell.y);
			itsDir->ViewMessage(header);
//			GMessageViewDir* dir =
//				new GMessageViewDir(itsDir, itsDir->GetMailFile(), header);
//			assert(dir != NULL);
//			dir->Activate();
			itsData->ChangeMessageStatus(header, GMessageHeader::kRead);
			TableRefreshRow(cell.y);
			}
		delete iter;
		}
	else if (key == 'w' && modifiers.meta())
		{
		(GetWindow()->GetDirector())->Close();
		}
	else if (key == 'd' && modifiers.meta())
		{
		JTableSelection& selection = GetTableSelection();
		JTableSelectionIterator* iter =
			new JTableSelectionIterator(&selection);
		assert(iter != NULL);
		JPoint cell;
		while(iter->Next(&cell))
			{
			GMessageHeader* header = itsData->GetHeader(cell.y);
			itsData->ChangeMessageStatus(header, GMessageHeader::kDelete);
			}
		TableRefresh();
		}
	else
		{
		JXTable::HandleKeyPress(key, modifiers);
		}
}

/******************************************************************************
 TableDrawCell

 ******************************************************************************/

void
GMessageTable::TableDrawCell
	(
	JPainter& p,
	const JPoint& cell,
	const JRect& rect
	)
{
	if ((GetTableSelection()).IsSelected(cell.y, 1))
		{
		p.SetPenColor(GetColormap()->GetDefaultSelectionColor());
		p.SetFilling(kJTrue);
		p.Rect(rect);
		p.SetPenColor(GetColormap()->GetBlackColor());
		p.SetFilling(kJFalse);
		}
	GMessageHeader* header = itsData->GetHeader(cell.y);
	JBoolean isNew = kJFalse;
	if (header->GetMessageStatus() == GMessageHeader::kNew)
		{
		isNew = kJTrue;
		}
	JBoolean isDelete = kJFalse;
	if (header->GetMessageStatus() == GMessageHeader::kDelete)
		{
		isDelete = kJTrue;
		}
	JString str;
	JPainter::HAlignment align = JPainter::kHAlignLeft;
	if (cell.x == kFromIndex)
		{
		str = header->GetFromNames();
		}
	else if (cell.x == kSubjectIndex)
		{
		str = header->GetSubject();
		}
	else if (cell.x == kDateIndex)
		{
		str = " " + header->GetShortDate();
		}
	else if (cell.x == kSizeIndex)
		{
		JIndex size = header->GetMessageEnd() - header->GetHeaderEnd();
		if (size < 10000)
			{
			str = JString(size,0);
			}
		else if (size < 10000000)
			{
			str = JString(size/1000,0) + "K";
			}
		else
			{
			str = JString(size/1000000,0) + "M";
			}
		str += "  ";
		align = JPainter::kHAlignRight;
		}
	JRect r(rect);
	r.left += kCellBuffer;
//	p.SetFontSize(kJSmallFontSize);
	JColorIndex cindex = JGetCurrColormap()->GetBlackColor();
	if (header->GetTo().IsEmpty())
		{
		cindex = JGetCurrColormap()->GetGray50Color();
		}
	p.SetFontStyle(JFontStyle(isNew, kJFalse, 0, kJFalse, cindex));
	if (cell.x > (JCoordinate)kIconIndex)
		{
		p.String(r, str, align, JPainter::kVAlignCenter);
		}
	if (cell.x == kIconIndex)
		{
		JRect irect = rect;
		irect.left += 3;
		p.Image(*itsMailIcon, itsMailIcon->GetBounds(), irect);
		}
	else if ((cell.x == kAttachIndex) && (header->HasAttachment()))
		{
		JRect irect = rect;
		irect.left += 3;
		p.Image(*itsClipIcon, itsClipIcon->GetBounds(), rect);
		}
	else if (cell.x == kPriorityIndex)
		{
		if (isDelete)
			{
			JRect irect = rect;
			irect.left += 8;
			p.Image(*itsCheckIcon, itsCheckIcon->GetBounds(), rect);
//			p.Image(*itsCheckIcon, itsCheckIcon->GetBounds(), irect.left, irect.top);
			}
		else if (header->GetReplyStatus() == GMessageHeader::kReplied)
			{
			p.Image(*itsReplyIcon, itsReplyIcon->GetBounds(), rect);
			}
		else if (header->GetReplyStatus() == GMessageHeader::kRepliedSender)
			{
			p.Image(*itsReplySenderIcon, itsReplySenderIcon->GetBounds(), rect);
			}
		else if (header->GetReplyStatus() == GMessageHeader::kRepliedAll)
			{
			p.Image(*itsReplyAllIcon, itsReplyAllIcon->GetBounds(), rect);
			}
		else if (header->GetReplyStatus() == GMessageHeader::kForwarded)
			{
			p.Image(*itsForwardIcon, itsForwardIcon->GetBounds(), rect);
			}
		else if (header->GetReplyStatus() == GMessageHeader::kRedirected)
			{
			p.Image(*itsRedirectIcon, itsRedirectIcon->GetBounds(), rect);
			}
		}
}

/******************************************************************************
 HandleMouseDown

 ******************************************************************************/

void
GMessageTable::HandleMouseDown
	(
	const JPoint& pt,
	const JXMouseButton button,
	const JSize clickCount,
	const JXButtonStates& buttonStates,
	const JXKeyModifiers& modifiers
	)
{
	itsIsWaitingForDeselect = kJFalse;
	if (button > kJXRightButton)
		{
		ScrollForWheel(button, modifiers);
		return;
		}
	JPoint cell;
	if (!GetCell(pt, &cell))
		{
		itsDownInCell = kJFalse;
		(GetTableSelection()).ClearSelection();
		itsLastSelectedIndex = 0;
		TableRefresh();
		return;
		}

	itsDownInCell = kJTrue;
	itsDownPt = pt;
	if (clickCount == 1)
		{
		if (modifiers.control())
			{
			AddToSelection(cell.y);
			}
		else if (modifiers.shift())
			{
			ExtendSelection(cell.y);
			}
		else if (button == kJXRightButton)
			{
			if (!GetTableSelection().IsSelected(cell.y, 1))
				{
				(GetTableSelection()).ClearSelection();
				ElementSelected(cell.y);
				}
			itsPopupMenu->PopUp(this, pt, buttonStates, modifiers);
			}
		else
			{
			if (GetTableSelection().IsSelected(cell.y, 1))
				{
				itsIsWaitingForDeselect = kJTrue;
				}
			else
				{
				ElementSelected(cell.y);
				}
			}
		}
	else if (clickCount == 2)
		{
		GMessageHeader* header = itsData->GetHeader(cell.y);
		itsDir->ViewMessage(header);
//		GMessageViewDir* dir =
//			new GMessageViewDir(itsDir, itsDir->GetMailFile(), header);
//		assert(dir != NULL);
//		dir->Activate();
		itsData->ChangeMessageStatus(header, GMessageHeader::kRead);
		TableRefresh();
		}
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
GMessageTable::HandleMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint cell;
	if (itsDownInCell)
		{
		JPoint p = itsDownPt - pt;
		if (sqrt((double)(p.x*p.x + p.y*p.y)) > kDragBeginBuffer)
			{
			itsIsWaitingForDeselect = kJFalse;
//			cout << "a1" << flush;
			JBoolean updated = itsData->Update();
//			cout << "a2" << flush;
			if (!updated && !GFileLocked(itsData->GetMailFile()) &&
				GLockFile(itsData->GetMailFile()))
				{
//				cout << "a3" << flush;
				itsUnlockTask = new JXTimerTask(500);
				assert(itsUnlockTask != NULL);
				JXGetApplication()->InstallIdleTask(itsUnlockTask);
				ListenTo(itsUnlockTask);
				GMessageDragData* data =
					new GMessageDragData(this, kDNDClassID);
				assert(data != NULL);
				BeginDND(pt, buttonStates, modifiers, data);
				}
			}
		}
//	cout << "a4" << endl;
}

/******************************************************************************
 HandleMouseUp

 ******************************************************************************/

void
GMessageTable::HandleMouseUp
	(
	const JPoint& pt,
	const JXMouseButton button,
	const JXButtonStates& buttonStates,
	const JXKeyModifiers& modifiers
	)
{
	JPoint cell;
	if (GetCell(pt, &cell) && itsIsWaitingForDeselect)
		{
		ElementSelected(cell.y);
		}
	itsIsWaitingForDeselect = kJFalse;
}

/******************************************************************************
 HandleDNDEnter (virtual protected)

	This is called when the mouse enters the widget.

 ******************************************************************************/

void
GMessageTable::HandleDNDEnter()
{
	itsDNDDropIndex = 0;
}

/******************************************************************************
 HandleDNDHere

 ******************************************************************************/

void
GMessageTable::HandleDNDHere
	(
	const JPoint& pt,
	const JXWidget* source
	)
{
	if (!itsData->CanReorganize())
		{
		itsDNDDropIndex = GetRowCount()+1;
		Refresh();
		return;
		}
	else if (GetRowCount() == 0)
		{
		itsDNDDropIndex = 1;
		Refresh();
		return;
		}

	JPoint cell;
	const JBoolean ok = GetCell(JPinInRect(pt, GetBounds()), &cell);
	assert( ok );

	const JRect r = GetCellRect(cell);
	if (pt.y < r.ycenter())
		{
		itsDNDDropIndex = cell.y;
		}
	else
		{
		itsDNDDropIndex = cell.y+1;
		}

	Refresh();
}

/******************************************************************************
 HandleDNDLeave

 ******************************************************************************/

void
GMessageTable::HandleDNDLeave()
{
	itsDNDDropIndex = 0;
	if (!IsDNDSource())
		{
		GUnlockFile(itsData->GetMailFile());
		itsHasLockedMBox = kJFalse;
		}
	Refresh();
}

/******************************************************************************
 HandleDNDDrop

 ******************************************************************************/

void
GMessageTable::HandleDNDDrop
	(
	const JPoint&		pt,
	const JArray<Atom>& typeList,
	const Atom			action,
	const Time			time,
	const JXWidget*		source
	)
{
	JIndex dropIndex = itsDNDDropIndex;
	HandleDNDLeave();

	Atom realAction = action;
	JXDNDManager* dndMgr  = GetDNDManager();
	if (action == dndMgr->GetDNDActionAskXAtom())
		{
		JArray<Atom> actionList;
		JPtrArray<JString> descriptionList(JPtrArrayT::kDeleteAll);
		if (!dndMgr->GetAskActions(&actionList, &descriptionList))
			{
			realAction = dndMgr->GetDNDActionMoveXAtom();;
			}
		else if (!dndMgr->ChooseDropAction(actionList, descriptionList,
										   &realAction))
			{
			return;
			}
		}

	const JBoolean dropOnSelf = JI2B( source == this );
	JTableSelection& s        = GetTableSelection();

	if (dropOnSelf)
		{
		assert( s.HasSelection() );

		JPtrArray<GMessageHeader> list(JPtrArrayT::kForgetAll);

		JTableSelectionIterator iter(&s);
		JPoint cell;
		while (iter.Next(&cell))
			{
			list.Append(itsData->GetHeader(cell.y));
			}

		const JSize count = list.GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			itsData->DropHeader(list.NthElement(i), &dropIndex);
			}

		s.ClearSelection();
		for (JIndex index = 1; index <= list.GetElementCount(); index++)
			{
			const GMessageHeader* src = list.NthElement(index);

			JIndex findIndex;
			const JBoolean ok = itsData->Includes(src, &findIndex);
			assert( ok );

			s.SelectCell(findIndex, 1);
			itsLastSelectedIndex = findIndex;
			}

		TableScrollToCell(JPoint(1, itsLastSelectedIndex));
		Refresh();
		}
	else
		{
		unsigned char* data = NULL;
		JSize dataLength;
		Atom returnType;
		JXSelectionManager* selManager = GetSelectionManager();
		JXSelectionManager::DeleteMethod delMethod;
		const Atom dndName = (GetDNDManager())->GetDNDSelectionName();
		if (selManager->GetData(dndName, time, itsMessageXAtom,
										 &returnType, &data, &dataLength, &delMethod))
			{
			std::istrstream is(reinterpret_cast<char*>(data), dataLength);
			GMessageHeaderList list;
			JString mbox;
			is >> mbox;
			int count;
			is >> count;
			for (int index = 1; index <= count; index++)
				{
				GMessageHeader* header = new GMessageHeader();
				assert(header != NULL);
				is >> *header;
				list.Append(header);
				}

			itsData->DropHeaders(mbox, dropIndex - 1, &list);

			selManager->DeleteData(&data, delMethod);
			s.ClearSelection();
			for (JSize index = 1; index <= list.GetElementCount(); index++)
				{
				JIndex findindex;
				GMessageHeader* src = list.NthElement(index);
				JBoolean ok = itsData->Includes(src, &findindex);
				assert(ok);
				s.SelectCell(findindex, 1);
				itsLastSelectedIndex = findindex;
				assert(CellValid(JPoint(1,findindex)));
				TableScrollToCell(JPoint(1, findindex));
				}
			if (realAction == dndMgr->GetDNDActionMoveXAtom())
				{
				selManager->SendDeleteRequest(dndName, time);
				}
			}
		}
}

/******************************************************************************
 GetDNDAction (virtual protected)

	This is called repeatedly during the drag so the drop action can be
	changed based on the current target, buttons, and modifier keys.

 ******************************************************************************/

Atom
GMessageTable::GetDNDAction
	(
	const JXContainer*		target,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (target == this)
		{
		return (GetDNDManager())->GetDNDActionMoveXAtom();
		}
	else if (modifiers.control())
		{
		return (GetDNDManager())->GetDNDActionAskXAtom();
		}
	else if (modifiers.meta())
		{
		return (GetDNDManager())->GetDNDActionCopyXAtom();
		}
	else
		{
		return (GetDNDManager())->GetDNDActionMoveXAtom();
		}
}

/******************************************************************************
 GetDNDAskActions (virtual protected)

	This is called when the value returned by GetDropAction() changes to
	XdndActionAsk.  If GetDropAction() repeatedly returns XdndActionAsk,
	this function is not called again because it is assumed that the
	actions are the same within a single DND session.

	This function must place at least 2 elements in askActionList and
	askDescriptionList.

	The first element should be the default action.

 ******************************************************************************/

void
GMessageTable::GetDNDAskActions
	(
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers,
	JArray<Atom>*			askActionList,
	JPtrArray<JString>*		askDescriptionList
	)
{
	JXDNDManager* dndMgr = GetDNDManager();
	askActionList->AppendElement(dndMgr->GetDNDActionCopyXAtom());
	askActionList->AppendElement(dndMgr->GetDNDActionMoveXAtom());

	JString* s = new JString(kDNDActionCopyDescrip);
	assert( s != NULL );
	askDescriptionList->Append(s);

	s = new JString(kDNDActionMoveDescrip);
	assert( s != NULL );
	askDescriptionList->Append(s);
}

/******************************************************************************
 WillAcceptDrop

 ******************************************************************************/

JBoolean
GMessageTable::WillAcceptDrop
	(
	const JArray<Atom>& typeList,
	Atom*				action,
	const JPoint&		pt,
	const Time			time,
	const JXWidget*		source
	)
{
	if (source == this)
		{
		if (itsData->CanReorganize())
			{
			return kJTrue;
			}
		return kJFalse;		
		}
		
	JBoolean found = kJFalse;
	const JSize typeCount = typeList.GetElementCount();
	for (JSize i=1; i<=typeCount; i++)
		{
		Atom type = typeList.GetElement(i);
		if (type == itsMessageXAtom)
			{
			found = kJTrue;
			}
		}
	if (found)
		{
		unsigned char* data = NULL;
		JSize dataLength;
		Atom returnType;
		JXSelectionManager* selManager = GetSelectionManager();
		JXSelectionManager::DeleteMethod delMethod;
		const Atom dndName = (GetDNDManager())->GetDNDSelectionName();
		if (selManager->GetData(dndName, time, itsMessageXAtom,
										 &returnType, &data, &dataLength, &delMethod))
			{
			std::istrstream is(reinterpret_cast<char*>(data), dataLength);
			JPtrArray<GMessageHeader> list(JPtrArrayT::kForgetAll);
			JString mbox;
			is >> mbox;
			if (mbox == itsData->GetMailFile())
				{
				found = kJFalse;
				}
			selManager->DeleteData(&data, delMethod);
			}
		}
	if (found && !itsHasLockedMBox)
		{
		found	= GLockFile(itsData->GetMailFile());
		itsHasLockedMBox	= kJTrue;
		}
	return found;
}

/******************************************************************************
 GetSelectionData (virtual protected)

	This is called when DND is terminated by a drop or when the target
	requests the data during a drag, but only if the delayed evaluation
	constructor for JXSelectionData was used.

	id is the string passed to the JXSelectionData constructor.

 ******************************************************************************/

void
GMessageTable::GetSelectionData
	(
	JXSelectionData*	data,
	const JCharacter*	id
	)
{
	if (strcmp(id, kDNDClassID) == 0)
		{
		GMessageDragData* messageData =
			dynamic_cast(GMessageDragData*, data);
		assert(messageData != NULL);

		messageData->SetDirector(itsDir);
		JPtrArray<GMessageHeader> headers(JPtrArrayT::kForgetAll);

		JTableSelection& selection = GetTableSelection();
		JTableSelectionIterator iter(&selection);
		JPoint cell;
		while(iter.Next(&cell))
			{
			GMessageHeader* header = itsData->GetHeader(cell.y);
			headers.Append(header);
			}
		messageData->SetHeaders(&headers);
		}
	else
		{
		JXTable::GetSelectionData(data, id);
		}
}

/******************************************************************************
 ExtendSelection

******************************************************************************/

void
GMessageTable::ExtendSelection
	(
	const JIndex index
	)
{
	if (itsLastSelectedIndex == 0)
		{
		return;
		}

	(GetTableSelection()).ClearSelection();
	if (itsLastSelectedIndex > index)
		{
		for (JIndex i = index; i <= itsLastSelectedIndex; i++)
			{
			(GetTableSelection()).SelectCell(i, 1);
			}
		}
	else
		{
		for (JIndex i = itsLastSelectedIndex; i <= index; i++)
			{
			(GetTableSelection()).SelectCell(i, 1);
			}
		}
	TableRefresh();
}

/******************************************************************************
 AddToSelection

******************************************************************************/

void
GMessageTable::AddToSelection
	(
	const JIndex index
	)
{
	if ((GetTableSelection()).IsSelected(index, 1))
		{
		(GetTableSelection()).SelectCell(index, 1, kJFalse);
		}

	else
		{
		(GetTableSelection()).SelectCell(index, 1);
		itsLastSelectedIndex = index;
		}
	TableRefreshRow(index);
}

/******************************************************************************
 ElementSelected

******************************************************************************/

void
GMessageTable::ElementSelected
	(
	const JIndex index
	)
{
	(GetTableSelection()).ClearSelection();
	(GetTableSelection()).SelectCell(index, 1);
	itsLastSelectedIndex = index;
	TableRefresh();
}

/******************************************************************************
 SetData

 ******************************************************************************/

void
GMessageTable::SetData
	(
	GMMailboxData* data
	)
{
	if (itsData != NULL)
		{
		StopListening(itsData);
		}
	itsData = data;
	ListenTo(itsData);
	SyncWithList();
	JBoolean found = kJFalse;
	JSize count = itsData->GetHeaderCount();
	if (count == 0)
		{
		return;
		}
	// is last element new?
	GMessageHeader* header = itsData->GetHeader(count);
	if (header->GetMessageStatus() != GMessageHeader::kNew)
		{
		TableScrollToCell(JPoint(1, GetRowCount()));
		return;
		}
	ScrollToFirstNew();
}

/******************************************************************************
 ScrollToFirstNew (public)

 ******************************************************************************/

void
GMessageTable::ScrollToFirstNew()
{
	// find first new message (from the bottom)
	JIndex firstNew = itsData->GetHeaderCount();
	while (firstNew > 1)
		{
		GMessageHeader* header = itsData->GetHeader(firstNew - 1);
		if (header->GetMessageStatus() == GMessageHeader::kNew)
			{
			firstNew--;
			}
		else
			{
			break;
			}
		}

	// scroll the first message to the top of the screen
	UpdateScrollbars();
	JRect rect = GetCellRect(JPoint(1, firstNew));
	ScrollTo(0, rect.top);
}

/******************************************************************************
 SyncWithList

 ******************************************************************************/

void
GMessageTable::SyncWithList()
{
	AppendRows(itsData->GetHeaderCount(), itsLineHeight);
}

/******************************************************************************
 UpdateMessageMenu


 ******************************************************************************/

void
GMessageTable::UpdateMessageMenu()
{
	JTableSelection& selection = GetTableSelection();
	JTableSelectionIterator iter(&selection);
	JPoint cell;
	JBoolean deleted	= kJFalse;
	JBoolean undeleted	= kJFalse;
	while(iter.Next(&cell))
		{
		GMessageHeader* header = itsData->GetHeader(cell.y);
		if (header->GetMessageStatus() == GMessageHeader::kDelete)
			{
			deleted	= kJTrue;
			}
		else
			{
			undeleted	= kJTrue;
			}		
		}
	if (deleted)
		{
		itsMessageMenu->EnableItem(kUndeleteCmd);
		}
	else
		{
		itsMessageMenu->DisableItem(kUndeleteCmd);
		}
	if (undeleted)
		{
		itsMessageMenu->EnableItem(kDeleteCmd);
		}
	else
		{
		itsMessageMenu->DisableItem(kDeleteCmd);
		}
	if (selection.GetSelectedCellCount() == 1)
		{
		itsMessageMenu->EnableItem(kReplyCmd);
		itsMessageMenu->EnableItem(kReplySenderCmd);
		itsMessageMenu->EnableItem(kReplyAllCmd);
		itsMessageMenu->EnableItem(kForwardCmd);
		itsMessageMenu->EnableItem(kRedirectCmd);
		}
	else
		{
		itsMessageMenu->DisableItem(kReplyCmd);
		itsMessageMenu->DisableItem(kReplySenderCmd);
		itsMessageMenu->DisableItem(kReplyAllCmd);
		itsMessageMenu->DisableItem(kForwardCmd);
		itsMessageMenu->DisableItem(kRedirectCmd);
		}
	if (selection.GetSelectedCellCount() > 0)
		{
		itsMessageMenu->EnableItem(kMarkReadCmd);
		itsMessageMenu->EnableItem(kMarkNewCmd);
		}
	else
		{
		itsMessageMenu->DisableItem(kMarkReadCmd);
		itsMessageMenu->DisableItem(kMarkNewCmd);
		}
}

/******************************************************************************
 HandleMessageMenu


 ******************************************************************************/

void
GMessageTable::HandleMessageMenu
	(
	const JIndex index
	)
{
	JTableSelection& selection = GetTableSelection();
	if (index == kSelectAllCmd)
		{
		if (GetRowCount() > 0)
			{
			selection.SelectCol(1);
			TableRefresh();
			}
		}
	else if (index == kDeleteCmd)
		{
		HandleDelete();
		}
	else if (index == kUndeleteCmd)
		{
		HandleUndelete();
		}
	else if (index == kMarkReadCmd)
		{
		HandleMarkRead();
		}
	else if (index == kMarkNewCmd)
		{
		HandleMarkNew();
		}
	else if (index == kReplyCmd)
		{
		HandleReplies(kReplyCmd);
		}
	else if (index == kReplySenderCmd)
		{
		HandleReplies(kReplySenderCmd);
		}
	else if (index == kReplyAllCmd)
		{
		HandleReplies(kReplyAllCmd);
		}
	else if (index == kForwardCmd)
		{
		HandleReplies(kForwardCmd);
		}
	else if (index == kRedirectCmd)
		{
		HandleReplies(kRedirectCmd);
		}
}

/******************************************************************************
 HandleDelete (private)

 ******************************************************************************/

void
GMessageTable::HandleDelete()
{
	JTableSelectionIterator iter(&GetTableSelection());
	JPoint cell;
	while(iter.Next(&cell))
		{
		GMessageHeader* header = itsData->GetHeader(cell.y);
		itsData->ChangeMessageStatus(header, GMessageHeader::kDelete);
		}
	TableRefresh();
}

/******************************************************************************
 HandleUndelete (private)

 ******************************************************************************/

void
GMessageTable::HandleUndelete()
{
	JTableSelectionIterator iter(&GetTableSelection());
	JPoint cell;
	while(iter.Next(&cell))
		{
		GMessageHeader* header = itsData->GetHeader(cell.y);
		itsData->ChangeMessageStatus(header, GMessageHeader::kRead);
		}
	TableRefresh();
}

/******************************************************************************
 HandleMarkRead (private)

 ******************************************************************************/

void
GMessageTable::HandleMarkRead()
{
	JTableSelectionIterator iter(&GetTableSelection());
	JPoint cell;
	while(iter.Next(&cell))
		{
		GMessageHeader* header = itsData->GetHeader(cell.y);
		itsData->ChangeMessageStatus(header, GMessageHeader::kRead);
		}
	TableRefresh();
}

/******************************************************************************
 HandleMarkNew (private)

 ******************************************************************************/

void
GMessageTable::HandleMarkNew()
{
	JTableSelectionIterator iter(&GetTableSelection());
	JPoint cell;
	while(iter.Next(&cell))
		{
		GMessageHeader* header = itsData->GetHeader(cell.y);
		itsData->ChangeMessageStatus(header, GMessageHeader::kNew);
		}
	TableRefresh();
}

/******************************************************************************
 HandleReplies (private)

 ******************************************************************************/

void
GMessageTable::HandleReplies
	(
	const JIndex index
	)
{
	JTableSelectionIterator iter(&GetTableSelection());
	JPoint cell;
	JBoolean ok = iter.Next(&cell);
	assert(ok);
	GMessageHeader* header = itsData->GetHeader(cell.y);
	if (index == kReplyCmd)
		{
		Reply(header);
		}
	else if (index == kReplySenderCmd)
		{
		ReplySender(header);
		}
	else if (index == kReplyAllCmd)
		{
		ReplyAll(header);
		}
	else if (index == kForwardCmd)
		{
		Forward(header);
		}
	else if (index == kRedirectCmd)
		{
		Redirect(header);
		}

}

/******************************************************************************
 SelectHeader


 ******************************************************************************/

void
GMessageTable::SelectHeader
	(
	GMessageHeader* header
	)
{
	JIndex findindex;
	if (itsData->Includes(header, &findindex))
		{
		GetTableSelection().ClearSelection();
		GetTableSelection().SelectCell(findindex, 1);
		assert(CellValid(JPoint(1, findindex)));
		TableScrollToCell(JPoint(1, findindex));
		itsLastSelectedIndex = findindex;
		TableRefresh();
		}
}

/******************************************************************************
 UnSelectHeader

 ******************************************************************************/

void
GMessageTable::UnSelectHeader
	(
	GMessageHeader* header
	)
{
	JIndex findindex;
	if (itsData->Includes(header, &findindex))
		{
		GetTableSelection().SelectCell(findindex, 1, kJFalse);
		TableRefreshRow(findindex);
		}
}

/******************************************************************************
 Reply

 ******************************************************************************/

void
GMessageTable::Reply
	(
	GMessageHeader*				header,
	GMessageHeader::ReplyStatus status,
	const JBoolean				useReplyTo,
	const JBoolean				useCC,
	const JBoolean				fillTo,
	const JBoolean				insertText,
	const JBoolean				quoteText
	)
{
	JString tempfile;
	JError err = JCreateTempFile(&tempfile);
	if (!err.OK())
		{
		err.ReportIfError();
		return;
		}

	GMessageEditDir* dir = new GMessageEditDir(JXGetApplication());
	assert(dir != NULL);
	JString sub = header->GetSubject();
	GFixHeaderForReply(status, &sub);
	dir->SetSubject(sub);
	dir->SetSentFrom(header->GetFrom());
	dir->SetSentDate(header->GetDate());
	dir->SetReplyType(status);
	dir->SetHeader(header, status);
	if (useCC)
		{
		JString cc = header->GetCC();
		if (cc.IsEmpty())
			{
			cc = header->GetTo();
			}
		else
			{
			cc += ", " + header->GetTo();
			}
		dir->SetCC(cc);
		}
	ofstream os(tempfile);
	fstream is(itsData->GetMailFile(), ios::in|ios::out);
	assert(is.good());
	JString text;

	// Check for MIME encoding
	GMMIMEParser* parser	= header->GetMIMEParser();
	JString filedir;
	JBoolean ok	= GMGetApplication()->GetFileDirectory(&filedir);
	if ( ok &&
		(parser == NULL) &&
		(header->IsMIME()))
		{
		JString data;
		is.seekp(header->GetHeaderStart());
		data.Read(is, header->GetMessageEnd() - header->GetHeaderStart());
		parser	= new GMMIMEParser(&data, filedir);
		assert(parser != NULL);
		header->SetMIMEParser(parser);
		}

	if (ok && header->IsMIME())
		{
		JString data;
		const JSize count = parser->GetTextSegmentCount();
		// The text I get back from the parser may be html. Since
		// JTextEditor already parses html, I create a temporary editor,
		// have it parse the data, and write it out as plain text.
		JXStaticText* editor	=
			new JXStaticText("", this, 
				JXWidget::kFixedLeft, JXWidget::kFixedTop, 
				0,0,100,100);
		assert(editor != NULL);
		
		for (JIndex i = 1; i <= count; i++)
			{
			GMMIMEParser::TextFormat format;
			JString charset;
			parser->GetTextSegment(i, &data, &format, &charset);
			data.AppendCharacter('\n');
			if (format == GMMIMEParser::kHTML)
				{
				std::istrstream is(data.GetCString(), data.GetLength());
				editor->PasteHTML(is);				
				}
			else
				{
				editor->Paste(data);
				}
			}
		editor->WritePlainText(os, JTextEditor::kUNIXText);
		delete editor;
		}
	else
		{
		ok	= kJFalse;
		}

	if (!ok)
		{
		is.seekp(header->GetHeaderEnd());
		text = JRead(is, header->GetMessageEnd() - header->GetHeaderEnd());
		text.Print(os);
		}

	os.close();
	dir->SetText(tempfile);
	dir->Activate();
	JString replyTo = header->GetReplyTo();
	if (replyTo.IsEmpty())
		{
		replyTo = header->GetFrom();
		}
	else if (useCC)
		{
		replyTo += ", " + header->GetFrom();
		}
	if (fillTo)
		{
		if (replyTo.IsEmpty() || !useReplyTo)
			{
			dir->SetTo(header->GetFrom());
			}
		else
			{
			dir->SetTo(replyTo);
			}
		}
	if (insertText)
		{
		dir->InsertText(quoteText);
		}
	else if (GGetPrefsMgr()->AutoInsertingQuote() && (text.GetLength() <= GGetPrefsMgr()->GetMaxQuote()))
		{
		dir->InsertText(kJTrue);
		}
}

/******************************************************************************
 ReplySender

 ******************************************************************************/

void
GMessageTable::ReplySender
	(
	GMessageHeader* header
	)
{
	Reply(header, GMessageHeader::kRepliedSender, kJFalse);
}

/******************************************************************************
 ReplyAll

 ******************************************************************************/

void
GMessageTable::ReplyAll
	(
	GMessageHeader* header
	)
{
	Reply(header, GMessageHeader::kRepliedAll, kJTrue, kJTrue);
}

/******************************************************************************
 Forward

 ******************************************************************************/

void
GMessageTable::Forward
	(
	GMessageHeader* header
	)
{
	Reply(header, GMessageHeader::kForwarded, kJFalse, kJFalse, kJFalse, kJTrue, kJTrue);
}

/******************************************************************************
 Redirect

 ******************************************************************************/

void
GMessageTable::Redirect
	(
	GMessageHeader* header
	)
{
	Reply(header, GMessageHeader::kRedirected, kJFalse, kJFalse, kJFalse, kJTrue, kJFalse);
}

/******************************************************************************
 AbortMessageDND (public)

 ******************************************************************************/

void
GMessageTable::AbortMessageDND()
{
if (IsDNDSource())
	{
	GetDNDManager()->CancelDND();
	GUnlockFile(itsData->GetMailFile());
	itsHasLockedMBox	= kJFalse;
	}
}

/******************************************************************************
 HandlePopupMenu (private)

 ******************************************************************************/

void
GMessageTable::HandlePopupMenu
	(
	const JIndex index
	)
{
	if (index == kDeletePopCmd)
		{
		HandleDelete();
		}
	else if (index == kUndeletePopCmd)
		{
		HandleUndelete();
		}
	else if (index == kMarkReadPopCmd)
		{
		HandleMarkRead();
		}
	else if (index == kMarkNewPopCmd)
		{
		HandleMarkNew();
		}
	else if (index == kReplyPopCmd)
		{
		HandleReplies(kReplyCmd);
		}
	else if (index == kReplySenderPopCmd)
		{
		HandleReplies(kReplySenderCmd);
		}
	else if (index == kReplyAllPopCmd)
		{
		HandleReplies(kReplyAllCmd);
		}
	else if (index == kForwardPopCmd)
		{
		HandleReplies(kForwardCmd);
		}
	else if (index == kRedirectPopCmd)
		{
		HandleReplies(kRedirectCmd);
		}
	else if (index == kSaveMsgPopCmd)
		{
		itsDir->SaveSelectedMessages();
		}
	else if (index == kAppendMsgPopCmd)
		{
		itsDir->AppendSelectedMessages();
		}
	else if (index == kPrintMsgPopCmd)
		{
		itsDir->PrintSelectedMessages();
		}
}

/******************************************************************************
 HandleShortcut (virtual public)

 ******************************************************************************/

void
GMessageTable::HandleShortcut
	(
	const int key,
	const JXKeyModifiers& modifiers
	)
{
	if (key == '_' &&
		modifiers.meta())
		{
		GGetPrefsMgr()->DeleteInbox(itsData->GetMailFile());
		}
	JXTable::HandleShortcut(key, modifiers);
}

/******************************************************************************
 HandleMessageTransfer (private)

 ******************************************************************************/

void
GMessageTable::HandleMessageTransfer
	(
	const JCharacter*	dest,
	const JBoolean		move
	)
{
	JTableSelection& s = GetTableSelection();
	JTableSelectionIterator iter(&s);
	if (!s.HasSelection())
		{
		return;
		}
	GMessageHeaderList* headers = new GMessageHeaderList();
	assert(headers != NULL);
	
	JPoint cell;
	while(iter.Next(&cell))
		{
		headers->Append(itsData->GetHeader(cell.y));
		}

	itsData->HandleMessageTransfer(dest, move, headers);
	delete headers;
}
