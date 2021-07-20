/******************************************************************************
 CBTreeWidget.cpp

	BASE CLASS = JXScrollableWidget

	Copyright © 1996-98 by John Lindal.

 ******************************************************************************/

#include "CBTreeWidget.h"
#include "CBTree.h"
#include "CBClass.h"
#include "CBProjectDocument.h"
#include "CBTreeDirector.h"
#include "CBEditSearchPathsDialog.h"
#include "cbGlobals.h"

#include <JXDNDManager.h>
#include <JXFileSelection.h>
#include <JXWindowDirector.h>
#include <JXWindow.h>
#include <JXTextMenu.h>
#include <JXScrollbar.h>
#include <JXWindowPainter.h>
#include <JXColorManager.h>
#include <JXImage.h>
#include <JXImageCache.h>
#include <jXUtil.h>

#include <JPagePrinter.h>
#include <JEPSPrinter.h>
#include <JString.h>
#include <jASCIIConstants.h>
#include <jDirUtil.h>
#include <jMouseUtil.h>
#include <jMath.h>
#include <jAssert.h>

bool CBTreeWidget::itsRaiseWhenSingleMatchFlag = false;
const JSize CBTreeWidget::kBorderWidth             = 5;

static const JUtf8Byte* kSelectionDataID = "CBTreeWidget";

/******************************************************************************
 Constructor

 ******************************************************************************/

CBTreeWidget::CBTreeWidget
	(
	CBTreeDirector*		director,
	CBTree*				tree,
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
	JXScrollableWidget(scrollbarSet, enclosure, hSizing, vSizing, x,y, w,h)
{
	CBClass::SetGhostNameColor(JColorManager::GetBlackColor());

	itsDirector = director;
	itsTree     = tree;

	itsFnMenu = jnew JXTextMenu(JString::empty, this, kFixedLeft, kFixedTop, 0,0, 10,10);
	assert( itsFnMenu != nullptr );
	itsFnMenu->Hide();
	itsFnMenu->SetToHiddenPopupMenu(true);
	itsFnMenu->CompressHeight();

	WantInput(true, false, true);	// need Ctrl-Tab

	const JColorID gray75Color = JColorManager::GetGrayColor(75);
	SetBackColor(gray75Color);
	SetFocusColor(gray75Color);

	SetVertStepSize(CBClass::GetTotalHeight(itsTree, GetFontManager()));

	ListenTo(itsTree);

	JCoordinate w1,h1;
	itsTree->GetBounds(&w1, &h1);
	SetBounds(w1, h1);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBTreeWidget::~CBTreeWidget()
{
}

/******************************************************************************
 FindClass

	Searches for classes with the given name, not full name.

	If there is a single match, button determines what to open:

		kJXLeftButton:   source
		kJXMiddleButton: header
		kJXRightButton:  nothing

	If raiseTreeWindow, the Tree window is raised -before- opening the source
	or header file.

 ******************************************************************************/

bool
CBTreeWidget::FindClass
	(
	const JString&		name,
	const JXMouseButton	button,
	const bool		raiseTreeWindow,
	const bool		reportNotFound,
	const bool		openFileIfSingleMatch,
	const bool		deselectAll
	)
	const
{
	itsTree->SelectClasses(name, deselectAll);

	JRect selRect;
	JSize selCount;
	if (itsTree->GetSelectionCoverage(&selRect, &selCount))
		{
		const_cast<CBTreeWidget*>(this)->ScrollToRectCentered(selRect, true);

		if (raiseTreeWindow &&
			(selCount > 1 || button == kJXRightButton ||
			 itsRaiseWhenSingleMatchFlag))
			{
			GetWindow()->GetDirector()->Activate();
			}

		if (openFileIfSingleMatch && button != kJXRightButton)
			{
			JPtrArray<CBClass> classList(JPtrArrayT::kForgetAll);
			const bool ok = itsTree->GetSelectedClasses(&classList);
			assert( ok );
			if (classList.GetElementCount() == 1)
				{
				CBClass* theClass = classList.GetFirstElement();
				if (button == kJXLeftButton)
					{
					theClass->ViewSource();
					}
				else if (button == kJXMiddleButton)
					{
					theClass->ViewHeader();
					}
				}
			}

		return true;
		}
	else
		{
		if (reportNotFound)
			{
			JGetUserNotification()->ReportError(
				JGetString("ClassNotFound::CBTreeWidget"));
			}
		return false;
		}
}

/******************************************************************************
 FindFunction

	Searches for classes that implement the given function.

	If there is a single match, button determines what to open:

		kJXLeftButton:   definition
		kJXMiddleButton: declaration
		kJXRightButton:  nothing

	If raiseTreeWindow, the Tree window is raised -before- opening the source
	or header file.

 ******************************************************************************/

bool
CBTreeWidget::FindFunction
	(
	const JString&		fnName,
	const bool		caseSensitive,
	const JXMouseButton	button,
	const bool		raiseTreeWindow,
	const bool		reportNotFound,
	const bool		openFileIfSingleMatch,
	const bool		deselectAll
	)
	const
{
	itsTree->SelectImplementors(fnName, caseSensitive, deselectAll);
/*
	JRect selRect;
	JSize selCount;
	if (itsTree->GetSelectionCoverage(&selRect, &selCount))
		{
		const_cast<CBTreeWidget*>(this)->ScrollToRectCentered(selRect, true);

		if (raiseTreeWindow &&
			(selCount > 1 || button == kJXRightButton ||
			 itsRaiseWhenSingleMatchFlag))
			{
			GetWindow()->GetDirector()->Activate();
			}

		if (openFileIfSingleMatch && button != kJXRightButton)
			{
			JPtrArray<CBClass> classList(JPtrArrayT::kForgetAll);
			const bool ok = itsTree->GetSelectedClasses(&classList);
			assert( ok );
			if (classList.GetElementCount() == 1)
				{
				CBClass* theClass = classList.GetFirstElement();
				if (button == kJXLeftButton)
					{
					theClass->ViewDefinition(fnName, caseSensitive, false);
					}
				else if (button == kJXMiddleButton)
					{
					theClass->ViewDeclaration(fnName, caseSensitive, false);
					}
				}
			}

		return true;
		}
	else
		{
		if (reportNotFound)
			{
			JGetUserNotification()->ReportError(
				JGetString("FunctionNotFound::CBTreeWidget"));
			}
		return false;
		}
*/
	return false;
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
CBTreeWidget::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	itsTree->Draw(p, rect);
}

/*****************************************************************************
 Print

	Since CBTree::UpdatePlacement() quantizes the y-coordinate of each class,
	we never have to worry that we might cut the frame of an ancestor in half
	at the bottom of the page.

 ******************************************************************************/

void
CBTreeWidget::Print
	(
	JPagePrinter& p
	)
{
	if (!itsTree->IsEmpty() && p.OpenDocument())
		{
		if (p.WillPrintBlackWhite())
			{
			CBClass::SetGhostNameColor(JColorManager::GetWhiteColor());
			}

		const JCoordinate lineHeight   = itsTree->GetLineHeight();
		const JCoordinate footerHeight = JRound(1.5 * p.JPainter::GetLineHeight());

		const JRect bounds    = GetBounds();
		const JSize lineCount = (bounds.height() - 2*kBorderWidth) / lineHeight;

		const JRect pageRect     = p.GetPageRect();
		const JSize linesPerPage = (pageRect.height() - footerHeight) / lineHeight;
		const JSize drawHeight   = linesPerPage * lineHeight;
		JSize pageCount          = lineCount / linesPerPage;
		if (lineCount % linesPerPage != 0)
			{
			pageCount++;
			}

		bool cancelled = false;
		for (JIndex i=1; i<=pageCount; i++)
			{
			if (!p.NewPage())
				{
				cancelled = true;
				break;
				}

			const JString pageNumberStr = "Page " + JString((JUInt64) i);
			p.String(pageRect.left, pageRect.bottom - footerHeight, pageNumberStr,
					 pageRect.width(), JPainter::kHAlignCenter,
					 footerHeight, JPainter::kVAlignBottom);
			p.LockFooter(pageRect.height() - drawHeight);

			p.SetPenColor(JColorManager::GetYellowColor());
			p.JPainter::Rect(p.GetPageRect());
			p.SetPenColor(JColorManager::GetBlackColor());

			const JCoordinate top = (i-1)*drawHeight;
			p.ShiftOrigin(0, 1-kBorderWidth-top);

			const JRect drawRect(top, pageRect.left, top + drawHeight, pageRect.right);
			itsTree->Draw(p, drawRect);
			}

		if (!cancelled)
			{
			p.CloseDocument();
			}

		CBClass::SetGhostNameColor(JColorManager::GetBlackColor());
		}
}

/*****************************************************************************
 Print

 ******************************************************************************/

void
CBTreeWidget::Print
	(
	JEPSPrinter& p
	)
{
	if (itsTree->IsEmpty())
		{
		return;
		}

	if (p.PSWillPrintBlackWhite())
		{
		CBClass::SetGhostNameColor(JColorManager::GetWhiteColor());
		}

	const JRect bounds = GetBounds();

	if (p.WantsPreview())
		{
		JPainter& p1 = p.GetPreviewPainter(bounds);
		itsTree->Draw(p1, bounds);
		}

	if (p.OpenDocument(bounds))
		{
		itsTree->Draw(p, bounds);
		p.CloseDocument();
		}

	CBClass::SetGhostNameColor(JColorManager::GetBlackColor());
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
CBTreeWidget::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	CBGetDocumentManager()->SetActiveProjectDocument(itsDirector->GetProjectDoc());

	itsKeyBuffer.Clear();
	itsDragType = kInvalidDrag;

	CBClass* theClass = nullptr;
	if (ScrollForWheel(button, modifiers))
		{
		return;
		}

	else if (itsTree->GetClass(pt, &theClass))
		{
		if (modifiers.shift())
			{
			theClass->ToggleSelected();
			}
		else if (!theClass->IsSelected())
			{
			itsTree->DeselectAll();
			theClass->SetSelected(true);
			ExpectPopupFnMenu(pt, button, theClass);
			}
		else if (clickCount == 1)
			{
			ExpectPopupFnMenu(pt, button, theClass);
			}

		else if (button == kJXLeftButton && clickCount == 2)
			{
			theClass->ViewSource();
			}
		else if (button == kJXMiddleButton && clickCount == 2)
			{
			theClass->ViewHeader();
			}
		else if (button == kJXRightButton && clickCount == 2 &&
				 !theClass->IsGhost())
			{
			itsDirector->ViewFunctionList(theClass);
			}
		else if (button == kJXRightButton && clickCount == 2)
			{
			JGetUserNotification()->ReportError(JGetString("NoGhostFile::CBTreeWidget"));
			}
		}

	else if (!modifiers.shift())
		{
		itsTree->DeselectAll();
		}
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
CBTreeWidget::HandleMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsDragType == kWaitForPopupFnMenuDrag &&
		!JMouseMoved(itsStartPt, pt) &&
		JXGetApplication()->GetCurrentTime() >= itsMouseDownTime + kJXDoubleClickTime)
		{
/*
		itsFnMenuDir = jnew CBFnListDirector(itsDirector, nullptr, itsFnMenuClass, this,
											itsDirector->ShowInheritedFns(), true);

		assert( itsFnMenuDir != nullptr );

		CBFnListWidget* fnList = itsFnMenuDir->GetFnListWidget();
		fnList->PrepareFunctionMenu(itsFnMenu);
		fnList->SetMenuButton(itsFnMenuButton);
*/
		itsFnMenu->PopUp(this, pt, buttonStates, modifiers);
		itsDragType = kInvalidDrag;
		}
	else if (itsDragType == kWaitForPopupFnMenuDrag && JMouseMoved(itsStartPt, pt))
		{
		auto* data = jnew JXFileSelection(this, kSelectionDataID);
		assert( data != nullptr );

		BeginDND(pt, buttonStates, modifiers, data);
		itsDragType = kInvalidDrag;
		}
}

/******************************************************************************
 ExpectPopupFnMenu (private)

 ******************************************************************************/

void
CBTreeWidget::ExpectPopupFnMenu
	(
	const JPoint&		pt,
	const JXMouseButton	button,
	CBClass*			theClass
	)
{
	itsDragType      = kWaitForPopupFnMenuDrag;
	itsStartPt       = pt;
	itsFnMenuButton  = button;
	itsMouseDownTime = JXGetApplication()->GetCurrentTime();
	itsFnMenuClass   = theClass;
}

/******************************************************************************
 HitSamePart (virtual protected)

 ******************************************************************************/

bool
CBTreeWidget::HitSamePart
	(
	const JPoint& pt1,
	const JPoint& pt2
	)
	const
{
	CBClass* theClass = nullptr;
	return itsTree->HitSameClass(pt1, pt2, &theClass);
}

/******************************************************************************
 GetSelectionData (virtual protected)

	This is called when DND is terminated by a drop or when the target
	requests the data during a drag, but only if the delayed evaluation
	constructor for JXSelectionData was used.

	id is the string passed to the JXSelectionData constructor.

 ******************************************************************************/

void
CBTreeWidget::GetSelectionData
	(
	JXSelectionData*	data,
	const JString&		id
	)
{
	if (id == kSelectionDataID)
		{
		auto* fileData = dynamic_cast<JXFileSelection*>(data);
		assert( fileData != nullptr );

		CBDocumentManager* docMgr = CBGetDocumentManager();

		auto* list = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
		assert( list != nullptr );

		JPtrArray<CBClass> classList(JPtrArrayT::kForgetAll);
		const bool hasSelection = itsTree->GetSelectedClasses(&classList);
		assert( hasSelection );

		const JSize classCount = classList.GetElementCount();
		JString headerName, sourceName;
		for (JIndex i=1; i<=classCount; i++)
			{
			CBClass* c = classList.GetElement(i);
			if (c->GetFileName(&headerName))
				{
				auto* s = jnew JString(headerName);
				assert( s != nullptr );
				list->Append(s);

				if (docMgr->GetComplementFile(headerName, itsTree->GetFileType(),
											  &sourceName, itsDirector->GetProjectDoc()))
					{
					s = jnew JString(sourceName);
					assert( s != nullptr );
					list->Append(s);
					}
				}
			}

		fileData->SetData(list);
		}
	else
		{
		JXScrollableWidget::GetSelectionData(data, id);
		}
}

/******************************************************************************
 GetDNDAction (virtual protected)

 ******************************************************************************/

Atom
CBTreeWidget::GetDNDAction
	(
	const JXContainer*		target,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	return GetDNDManager()->GetDNDActionPrivateXAtom();
}

/******************************************************************************
 HandleDNDResponse (virtual protected)

 ******************************************************************************/

void
CBTreeWidget::HandleDNDResponse
	(
	const JXContainer*	target,
	const bool		dropAccepted,
	const Atom			action
	)
{
	DisplayCursor(GetDNDManager()->GetDNDFileCursor(dropAccepted, action));
}

/******************************************************************************
 WillAcceptDrop (virtual protected)

	Accept text/uri-list.

 ******************************************************************************/

bool
CBTreeWidget::WillAcceptDrop
	(
	const JArray<Atom>&	typeList,
	Atom*				action,
	const JPoint&		pt,
	const Time			time,
	const JXWidget*		source
	)
{
	// dropping on ourselves makes no sense since we don't accept files

	if (this == const_cast<JXWidget*>(source))
		{
		return false;
		}

	// we accept drops of type text/uri-list

	const Atom urlXAtom = GetSelectionManager()->GetURLXAtom();

	const JSize typeCount = typeList.GetElementCount();
	for (JIndex i=1; i<=typeCount; i++)
		{
		const Atom a = typeList.GetElement(i);
		if (a == urlXAtom)
			{
			*action = GetDNDManager()->GetDNDActionPrivateXAtom();
			return true;
			}
		}

	return false;
}

/******************************************************************************
 HandleDNDDrop (virtual protected)

	This is called when the data is dropped.  The data is accessed via
	the selection manager, just like Paste.

	Since we only accept text/uri-list, we don't bother to check typeList.

 ******************************************************************************/

void
CBTreeWidget::HandleDNDDrop
	(
	const JPoint&		pt,
	const JArray<Atom>&	typeList,
	const Atom			action,
	const Time			time,
	const JXWidget*		source
	)
{
	JXSelectionManager* selMgr = GetSelectionManager();

	Atom returnType;
	unsigned char* data;
	JSize dataLength;
	JXSelectionManager::DeleteMethod delMethod;
	if (!selMgr->GetData(GetDNDManager()->GetDNDSelectionName(),
						 time, selMgr->GetURLXAtom(),
						 &returnType, &data, &dataLength, &delMethod))
		{
		return;
		}

	if (returnType != selMgr->GetURLXAtom())
		{
		selMgr->DeleteData(&data, delMethod);
		return;
		}

	JPtrArray<JString> dirList(JPtrArrayT::kDeleteAll),
					   urlList(JPtrArrayT::kDeleteAll);
	JXUnpackFileNames((char*) data, dataLength, &dirList, &urlList);

	const JSize fileCount = dirList.GetElementCount();
	for (JIndex i=fileCount; i>=1; i--)
		{
		const JString* name = dirList.GetElement(i);
		if (!JDirectoryExists(*name))
			{
			dirList.DeleteElement(i);
			}
		}

	if (dirList.IsEmpty() && urlList.IsEmpty())
		{
		JGetUserNotification()->ReportError(
			JGetString("OnlyDropFolders::CBTreeWidget"));
		}
	else if (dirList.IsEmpty())
		{
		JXReportUnreachableHosts(urlList);
		}
	else
		{
		CBEditSearchPathsDialog* dlog =
			itsDirector->GetProjectDoc()->EditSearchPaths(itsDirector);
		dlog->AddDirectories(dirList);
		}

	selMgr->DeleteData(&data, delMethod);
}

/******************************************************************************
 HandleFocusEvent (virtual protected)

 ******************************************************************************/

void
CBTreeWidget::HandleFocusEvent()
{
	JXScrollableWidget::HandleFocusEvent();
	itsKeyBuffer.Clear();
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

void
CBTreeWidget::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	CBGetDocumentManager()->SetActiveProjectDocument(itsDirector->GetProjectDoc());

	// open source

	if (c == kJReturnKey)
		{
		itsKeyBuffer.Clear();
		itsTree->ViewSelectedSources();
		}

	// open header

	else if (c == '\t' &&
			 !modifiers.GetState(kJXMetaKeyIndex)   &&
			 modifiers.GetState(kJXControlKeyIndex) &&
			 !modifiers.shift())
		{
		itsKeyBuffer.Clear();
		itsTree->ViewSelectedHeaders();
		}

	// incremental search for class name

	else if (c == ' ')
		{
		itsKeyBuffer.Clear();
		}
	else if (!itsTree->IsEmpty() && c.IsPrint() &&
			 !modifiers.meta() && !modifiers.control())
		{
		itsKeyBuffer.Append(c);

		CBClass* selClass = nullptr;
		if (itsTree->ClosestVisibleMatch(itsKeyBuffer, &selClass))
			{
			itsTree->DeselectAll();
			selClass->SetSelected(true);
			ScrollToRectCentered(selClass->GetFrame(), true);
			}
		}

	else
		{
		JXScrollableWidget::HandleKeyPress(c, keySym, modifiers);
		}
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
CBTreeWidget::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsTree && message.Is(CBTree::kBoundsChanged))
		{
		JCoordinate w,h;
		itsTree->GetBounds(&w, &h);
		SetBounds(w, h);
		Refresh();
		}
	else if (sender == itsTree && message.Is(CBTree::kPrepareForParse))
		{
		Hide();
		}
	else if (sender == itsTree && message.Is(CBTree::kParseFinished))
		{
		Show();
		}

	else if (sender == itsTree && message.Is(CBTree::kFontSizeChanged))
		{
		const auto* info =
			dynamic_cast<const CBTree::FontSizeChanged*>(&message);
		assert( info != nullptr );

		JXScrollbar *hScrollbar, *vScrollbar;
		const bool ok = GetScrollbars(&hScrollbar, &vScrollbar);
		assert( ok );
		vScrollbar->PrepareForScaledMaxValue(info->GetVertScaleFactor());

		SetVertStepSize(CBClass::GetTotalHeight(itsTree, GetFontManager()));
		}

	else if (sender == itsTree && message.Is(CBTree::kClassSelected))
		{
		const auto* selection =
			dynamic_cast<const CBTree::ClassSelected*>(&message);
		assert( selection != nullptr );
		RefreshRect((selection->GetClass())->GetFrame());
		}
	else if (sender == itsTree && message.Is(CBTree::kClassDeselected))
		{
		const auto* selection =
			dynamic_cast<const CBTree::ClassDeselected*>(&message);
		assert( selection != nullptr );
		RefreshRect((selection->GetClass())->GetFrame());
		}
	else if (sender == itsTree && message.Is(CBTree::kAllClassesDeselected))
		{
		Refresh();
		}

	else if (sender == itsTree && message.Is(CBTree::kNeedsRefresh))
		{
		Refresh();
		}

	else if (sender == itsTree && message.Is(CBTree::kChanged))
		{
		const JRect ap = GetAperture();

		JPtrArray<CBClass> sel(JPtrArrayT::kForgetAll);
		CBClass* closest = nullptr;
		JCoordinate miny = 0;
		if (itsTree->GetSelectedClasses(&sel))
			{
			bool visible  = false;
			const JSize count = sel.GetElementCount();
			for (JIndex i=1; i<=count; i++)
				{
				CBClass* c    = sel.GetElement(i);
				const JRect r = c->GetFrame();
				if (ap.Contains(r))
					{
					visible = true;
					break;
					}
				else if (ap.bottom < r.bottom &&
						 (closest == nullptr || r.top - ap.bottom < miny))
					{
					miny    = r.top - ap.bottom;
					closest = c;
					}
				else if (r.bottom < ap.top &&
						 (closest == nullptr || ap.top - r.bottom < miny))
					{
					miny    = ap.top - r.bottom;
					closest = c;
					}
				}

			if (!visible && closest != nullptr)
				{
				ScrollToRect(closest->GetFrame());
				}
			}
		}

	else
		{
		JXScrollableWidget::Receive(sender, message);
		}
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
CBTreeWidget::ReadSetup
	(
	std::istream&			setInput,
	const JFileVersion	setVers
	)
{
	if (10 <= setVers && setVers <= 21)
		{
		JPoint scrollPt;
		setInput >> scrollPt;
		ScrollTo(scrollPt);
		}

	if (setVers >= 22)
		{
		ReadScrollSetup(setInput);
		}
	else if (setVers >= 18)
		{
		JXScrollbar *hScrollbar, *vScrollbar;
		const bool ok = GetScrollbars(&hScrollbar, &vScrollbar);
		assert( ok );
		hScrollbar->ReadSetup(setInput);
		vScrollbar->ReadSetup(setInput);
		}
}

/******************************************************************************
 WriteSetup

 ******************************************************************************/

void
CBTreeWidget::WriteSetup
	(
	std::ostream& setOutput
	)
	const
{
	setOutput << ' ';
	WriteScrollSetup(setOutput);
	setOutput << ' ';
}
