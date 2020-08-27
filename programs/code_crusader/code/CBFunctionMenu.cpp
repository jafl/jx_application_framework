/******************************************************************************
 CBFunctionMenu.cpp

	This menu is an action menu, so all messages that are broadcast are
	meaningless to outsiders.

	BASE CLASS = JXTextMenu

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include "CBFunctionMenu.h"
#include "CBFnMenuUpdater.h"
#include "cbmUtil.h"

#if defined CODE_CRUSADER
#include "CBTextDocument.h"
#include "CBTextEditor.h"
#endif

#include <JXFileDocument.h>
#include <JXDisplay.h>
#include <JXTEBase.h>
#include <jFileUtil.h>
#include <jGlobals.h>
#include <stdio.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBFunctionMenu::CBFunctionMenu
	(
	JXFileDocument*			doc,
	const CBTextFileType	type,
	JXTEBase*				te,
	JXContainer*			enclosure,
	const HSizingOption		hSizing,
	const VSizingOption		vSizing,
	const JCoordinate		x,
	const JCoordinate		y,
	const JCoordinate		w,
	const JCoordinate		h
	)
	:
	JXTextMenu(CBCtagsUser::GetFunctionMenuTitle(type),
			   enclosure, hSizing, vSizing, x,y, w,h)
{
	CBFunctionMenuX(doc, type, te);
}

CBFunctionMenu::CBFunctionMenu
	(
	JXFileDocument*			doc,
	const CBTextFileType	type,
	JXTEBase*				te,
	JXMenu*					owner,
	const JIndex			itemIndex,
	JXContainer*			enclosure
	)
	:
	JXTextMenu(owner, itemIndex, enclosure)
{
	CBFunctionMenuX(doc, type, te);
}

// private

void
CBFunctionMenu::CBFunctionMenuX
	(
	JXFileDocument*			doc,
	const CBTextFileType	type,
	JXTEBase*				te
	)
{
	itsDoc            = doc;
	itsFileType       = kCBUnknownFT;
	itsLang           = kCBOtherLang;
	itsTE             = te;
	itsCaretItemIndex = 0;

	itsLineIndexList = jnew JArray<JIndex>(100);
	assert( itsLineIndexList != nullptr );

	SetEmptyMenuItems();
	SetUpdateAction(kDisableNone);
	ListenTo(this);

	TextChanged(type, JString::empty);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBFunctionMenu::~CBFunctionMenu()
{
	jdelete itsLineIndexList;
}

/******************************************************************************
 TextChanged

 ******************************************************************************/

void
CBFunctionMenu::TextChanged
	(
	const CBTextFileType	type,
	const JString&			fileName
	)
{
	itsFileType    = type;
	itsNeedsUpdate = kJTrue;

	SetTitle(CBCtagsUser::GetFunctionMenuTitle(type), nullptr, kJFalse);

#ifdef CODE_MEDIC

	itsFileName = fileName;

#endif
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
CBFunctionMenu::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == this && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateMenu();
		}
	else if (sender == this && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleSelection(selection->GetIndex());
		}

	else
		{
		JXTextMenu::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateMenu (private)

 ******************************************************************************/

void
CBFunctionMenu::UpdateMenu()
{
	CBFnMenuUpdater* updater        = CBMGetFnMenuUpdater();
	const JXKeyModifiers& modifiers = GetDisplay()->GetLatestKeyModifiers();

	JBoolean sort = updater->WillSortFnNames();
	if (modifiers.meta())
		{
		sort = !sort;
		}

	JBoolean includeNS = updater->WillIncludeNamespace();
	if (modifiers.shift())
		{
		includeNS = !includeNS;
		}

	const JBoolean pack = updater->WillPackFnNames();

	if (itsNeedsUpdate || sort != itsSortFlag ||
		includeNS != itsIncludeNSFlag || pack != itsPackFlag)
		{
		#if defined CODE_CRUSADER

		assert( itsDoc != nullptr );

		JString fileName;
		JBoolean deleteFile = kJFalse;
		if (itsDoc->NeedsSave())
			{
			itsDoc->SafetySave(JXDocumentManager::kTimer);
			if (!itsDoc->GetSafetySaveFileName(&fileName) &&
				(JCreateTempFile(&fileName)).OK())
				{
				// directory may not be writable

				deleteFile = kJTrue;

				// itsDoc can't be CBTextDocument, because Code Medic uses us

				CBTextDocument* textDoc = dynamic_cast<CBTextDocument*>(itsDoc);
				assert( textDoc != nullptr );

				std::ofstream output(fileName.GetBytes());
				textDoc->GetTextEditor()->GetText()->GetText().Print(output);
				}
			}
		else
			{
			JBoolean onDisk;
			fileName = itsDoc->GetFullName(&onDisk);
			if (!onDisk)
				{
				fileName.Clear();
				}
			}

		#elif defined CODE_MEDIC

		const JString& fileName = itsFileName;

		#endif

		if (!fileName.IsEmpty())
			{
			itsLang = updater->UpdateMenu(fileName, itsFileType, sort, includeNS, pack,
										  this, itsLineIndexList);
			}
		else
			{
			itsLang = kCBOtherLang;
			this->RemoveAllItems();
			itsLineIndexList->RemoveAll();
			}

		if (IsEmpty())
			{
			SetEmptyMenuItems();
			}

		itsNeedsUpdate    = kJFalse;
		itsSortFlag       = sort;
		itsIncludeNSFlag  = includeNS;
		itsPackFlag       = pack;
		itsCaretItemIndex = 0;						// nothing to remove
		itsTE->GetText()->DeactivateCurrentUndo();	// force another TextChanged

		#if defined CODE_CRUSADER

		if (deleteFile)
			{
			JRemoveFile(fileName);
			}

		#endif
		}

	// mark caret location

	if (!sort && !itsLineIndexList->IsEmpty())
		{
		if (itsCaretItemIndex > 0)
			{
			ShowSeparatorAfter(itsCaretItemIndex, kJFalse);
			itsCaretItemIndex = 0;
			}

		const JIndex lineIndex =
			itsTE->VisualLineIndexToCRLineIndex(
						itsTE->GetLineForChar(itsTE->GetInsertionCharIndex()));

		const JSize count = GetItemCount();
		assert( count == itsLineIndexList->GetElementCount() );

		for (JIndex i=1; i<=count; i++)
			{
			if (itsLineIndexList->GetElement(i) > lineIndex)
				{
				itsCaretItemIndex = i-1;
				if (itsCaretItemIndex > 0)
					{
					ShowSeparatorAfter(itsCaretItemIndex, kJTrue);
					}
				break;
				}
			}
		}
}

/******************************************************************************
 HandleSelection (private)

	ctags can't consider wrapped lines.

 ******************************************************************************/

void
CBFunctionMenu::HandleSelection
	(
	const JIndex index
	)
{
	JIndexRange r;
	r.first = r.last = itsLineIndexList->GetElement(index);
	CBMSelectLines(itsTE, r);
	CBMScrollForDefinition(itsTE, itsLang);
}

/******************************************************************************
 SetEmptyMenuItems (private)

 ******************************************************************************/

void
CBFunctionMenu::SetEmptyMenuItems()
{
	JString name = CBCtagsUser::GetFunctionMenuTitle(itsFileType);
	name.ToLower();

	const JUtf8Byte* map[] =
		{
		"name", name.GetBytes()
		};
	const JString menuItems = JGetString("EmptyMenu::CBFunctionMenu", map, sizeof(map));
	SetMenuItems(menuItems.GetBytes());
}
