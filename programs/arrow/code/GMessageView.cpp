/******************************************************************************
 GMessageView.cc

	BASE CLASS = public JXLinkText

	Copyright © 1997 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#include "GMessageView.h"
#include "GPrefsMgr.h"
#include "GMGlobals.h"

#include <JXHelpManager.h>
#include <JXDisplay.h>
#include <JXScrollbar.h>
#include <JXFontManager.h>
#include <JXColormap.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include <JRegex.h>
#include <JLatentPG.h>
#include <JFontManager.h>
#include <jASCIIConstants.h>
#include <jAssert.h>

#define LEGAL_CHAR "^][:space:]\'`;<>[\""
#define LEGAL_HOST "[" LEGAL_CHAR "()]*[" LEGAL_CHAR "().,:]"

static const JRegex kGMRegex1("(http://|https://|ftp://|mailto:)" LEGAL_HOST);
static const JRegex kGMRegex2("[" LEGAL_CHAR "@]+@" LEGAL_HOST);
static const JRegex kGMRegex3("www\\." LEGAL_HOST);

static const JRegex* kGMRegexes[]=
	{
	&kGMRegex1,
	&kGMRegex2,
	&kGMRegex3
	};

static const JSize kGMRegexCount = sizeof(kGMRegexes)/sizeof(JRegex*);

static const GMessageView::LinkType kGMLinkTypes[] =
	{
	GMessageView::kFullURL,
	GMessageView::kEMail,
	GMessageView::kWeb
	};

const JIndex kGMRemoveAllLinks = 1;

// JBroadcaster messages

const JCharacter* GMessageView::kUnderBarShortcutCaught	= "kUnderBarShortcutCaught::GMessageView";

/******************************************************************************
 Constructor

 *****************************************************************************/

GMessageView::GMessageView
	(
	JXMenuBar*			menuBar,
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
   JXLinkText(scrollbarSet, enclosure, hSizing, vSizing, x, y, w, h)
{
	GMessageViewX(scrollbarSet, enclosure, hSizing, vSizing, x, y, w, h);
	AppendEditMenu(menuBar);
	AppendSearchMenu(menuBar);
}

GMessageView::GMessageView
	(
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
   JXLinkText(scrollbarSet, enclosure, hSizing, vSizing, x, y, w, h)
{
	GMessageViewX(scrollbarSet, enclosure, hSizing, vSizing, x, y, w, h);
}

// private

void
GMessageView::GMessageViewX
	(
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	SetPTPrinter(GMGetPTPrinter());
	SetBackColor(GetColormap()->GetWhiteColor());
	ShouldAllowDragAndDrop(kJTrue);
	SetDefaultFont(GGetPrefsMgr()->GetDefaultMonoFont(), GGetPrefsMgr()->GetDefaultFontSize());
	JSize tabChars = GGetPrefsMgr()->GetTabCharWidth();
	SetCRMTabCharCount(tabChars);
	JCoordinate charWidth =
		TEGetFontManager()->GetCharWidth(GGetPrefsMgr()->GetDefaultMonoFont(),
			GGetPrefsMgr()->GetDefaultFontSize(), JFontStyle(), ' ');
	SetDefaultTabWidth(charWidth * tabChars);

	itsLinks = new JArray<LinkInfo>;
	assert(itsLinks != NULL);

	ListenTo(this);
	ListenTo(GGetPrefsMgr());
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GMessageView::~GMessageView()
{
	RemoveLinks(kGMRemoveAllLinks);
	delete itsLinks;
}

/******************************************************************************
 GetLinkCount (virtual protected)

 ******************************************************************************/

JSize
GMessageView::GetLinkCount()
	const
{
	return itsLinks->GetElementCount();
}

/******************************************************************************
 GetLinkRange (virtual protected)

 ******************************************************************************/

JIndexRange
GMessageView::GetLinkRange
	(
	const JIndex index
	)
	const
{
	return (itsLinks->GetElement(index)).range;
}

/******************************************************************************
 LinkClicked (virtual protected)

 ******************************************************************************/

void
GMessageView::LinkClicked
	(
	const JIndex index
	)
{
	const LinkInfo info = itsLinks->GetElement(index);
	JString url = *(info.url);
	if (info.type == kEMail)
		{
		url.Prepend("mailto:");
		}
	else if (info.type == kWeb)
		{
		url.Prepend("http://");
		}
	(JXGetHelpManager())->ShowURL(url, NULL);
}

/******************************************************************************
 ParseURLs

 ******************************************************************************/

void
GMessageView::ParseURLs
	(
	const JBoolean deleteFirst
	)
{
	SetCaretLocation(1);
	if (deleteFirst)
		{
		RemoveLinks(kGMRemoveAllLinks);
		}

	JLatentPG pg(50);
	JSize length = GetTextLength();
	pg.FixedLengthProcessBeginning((kGMRegexCount + 1)*length, "Parsing URLs...", kJTrue, kJFalse);

	JIndex lastindex = 0;
	JBoolean cancel  = kJFalse;
	if (length > 0)
		{
		for (JIndex i=0; i<kGMRegexCount; i++)
			{
			if (!ParseForURLs(i, &pg, length, &lastindex))
				{
				cancel = kJTrue;
				break;
				}
			}
		}

	const JColorIndex blueColor = JGetCurrColormap()->GetBlueColor();

	JSize linkCount	= itsLinks->GetElementCount();
	JSize lastLink	= linkCount;
	if (cancel)
		{
		RemoveLinks(kGMRemoveAllLinks);
		}
	else if (linkCount > 0)
		{
		for (JIndex i=1; i<=linkCount; i++)
			{
			const LinkInfo info = itsLinks->GetElement(i);

			SetSelection(info.range);
			SetCurrentFontColor(blueColor);
			SetCurrentFontUnderline(1);
			if (!pg.IncrementProgress((kGMRegexCount*length + info.range.last) - lastindex))
				{
				cancel   = kJTrue;
				lastLink = i;
				break;
				}
			lastindex = kGMRegexCount*length + info.range.last;
			}

		if (lastLink != linkCount)
			{
			RemoveLinks(lastLink + 1);
			}

		if (GetTextLength() > 0)
			{
			SetCaretLocation(1);
			}
		}

	pg.ProcessFinished();
}

/******************************************************************************
 ParseForURLs (private)

	index is zero based because it refers to a c-array.

 ******************************************************************************/

JBoolean
GMessageView::ParseForURLs
	(
	const JIndex	index,
	JLatentPG*		pg,
	const JSize		length,
	JIndex*			lastindex
	)
{
	JBoolean ok = kJTrue;
	JBoolean wrapped;
	JArray<JIndexRange> submatchList;
	while (ok && JTextEditor::SearchForward(*kGMRegexes[index], kJFalse,
											kJFalse, &wrapped, &submatchList))
		{
		JIndexRange range = submatchList.GetElement(1);

		JString* str = new JString(GetText().GetSubstring(range));
		assert(str != NULL);

		LinkInfo info(range, str, kGMLinkTypes[index]);
		itsLinks->AppendElement(info);

		if (!pg->IncrementProgress((index * length) + range.last - *lastindex))
			{
			ok = kJFalse;
			}
		*lastindex = (index * length) + range.last;
		}

	SetCaretLocation(1);
	JSize delta = ((index + 1) * length) - *lastindex;
	*lastindex  = (index + 1) * length;
	if (delta != 0 && !pg->IncrementProgress(delta))
		{
		ok = kJFalse;
		}

	return ok;
}

/******************************************************************************
 AddLink (private)

 ******************************************************************************/

void
GMessageView::AddLink
	(
	const JIndexRange range, 
	const JCharacter* url
	)
{
	for (JIndex i=0; i<kGMRegexCount; i++)
		{
		if (kGMRegexes[i]->Match(url))
			{
			JString* str = new JString(url);
			assert(str != NULL);

			LinkInfo info(range, str, kGMLinkTypes[i]);
			itsLinks->AppendElement(info);
			break;
			}
		}
}

/******************************************************************************
 RemoveLinks

 ******************************************************************************/

void
GMessageView::RemoveLinks()
{
	RemoveLinks(kGMRemoveAllLinks);
}

/******************************************************************************
 RemoveLinks (private)

 ******************************************************************************/

void
GMessageView::RemoveLinks
	(
	const JIndex startIndex
	)
{
	const JBoolean removeAll = JI2B(startIndex == kGMRemoveAllLinks);

	const JSize linkCount = itsLinks->GetElementCount();
	for (JIndex i=linkCount; i>=startIndex; i--)
		{
		LinkInfo info = itsLinks->GetElement(i);
		delete info.url;
		if (!removeAll)
			{
			itsLinks->RemoveElement(i);
			}
		}

	if (removeAll)
		{
		itsLinks->RemoveAll();
		}
}

/******************************************************************************
 HandleHTMLTag (virtual protected)

 ******************************************************************************/

void
GMessageView::HandleHTMLTag
	(
	const JString&					name,
	const JStringPtrMap<JString>&	attr
	)
{
	if (name == "a")
		{
		const JString* valueStr;
		if (attr.GetElement("href", &valueStr) && valueStr != NULL)
			{
			itsLinkUrl         = *valueStr;
			itsLinkRange.first = GetHTMLBufferLength()+1;
			}
		else
			{
			itsLinkUrl.Clear();
			}
		}
	else if (name == "/a")
		{
		itsLinkRange.last = GetHTMLBufferLength();
		if (!itsLinkRange.IsEmpty() && !itsLinkUrl.IsEmpty())
			{
			AddLink(itsLinkRange, itsLinkUrl);
			}
		}
	else if (name == "img")
		{
		// don't display anything
		}
	else
		{
		JXLinkText::HandleHTMLTag(name, attr);
		}
}

/******************************************************************************
 HandleHTMLError (virtual protected)

 ******************************************************************************/

void
GMessageView::HandleHTMLError
	(
	const JCharacter* errStr
	)
{
	// don't report errors
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
GMessageView::Receive
	(
	JBroadcaster* sender,
	const JBroadcaster::Message& message
	)
{
	if (sender == GGetPrefsMgr() && message.Is(GPrefsMgr::kFontChanged))
		{
		JString fontName	= GGetPrefsMgr()->GetDefaultMonoFont();
		JSize fontSize		= GGetPrefsMgr()->GetDefaultFontSize();
		JBoolean changed =
			JConvertToBoolean(
				(fontName != GetDefaultFontName()) ||
				(fontSize != GetDefaultFontSize()));
		if (changed)
			{
			const JFontManager* fontMgr = GetFontManager();
			const JFloat h1 = fontMgr->GetLineHeight(GetDefaultFontName(),
													 GetDefaultFontSize(), JFontStyle());
			const JFloat h2 = fontMgr->GetLineHeight(fontName, fontSize, JFontStyle());
			JFloat vScrollScale    = h2 / h1;

			JXScrollbar *hScrollbar, *vScrollbar;
			const JBoolean ok = GetScrollbars(&hScrollbar, &vScrollbar);
			assert( ok );
			vScrollbar->PrepareForScaledMaxValue(vScrollScale);

			JSize tabCharCount = GGetPrefsMgr()->GetTabCharWidth();

			SetAllFontNameAndSize(fontName, fontSize, tabCharCount, kJFalse, kJFalse);
			}
		}
	else
		{
		if (sender == this && message.Is(JTextEditor::kTextSet))
			{
			ParseURLs();
			}

		JXLinkText::Receive(sender, message);
		}
}

/******************************************************************************
 HandleShortcut (virtual public)

 ******************************************************************************/

void
GMessageView::HandleShortcut
	(
	const int key,
	const JXKeyModifiers& modifiers
	)
{
	if (key == '_' && modifiers.meta())
		{
		Broadcast(UnderBarShortcutCaught());
		}

	JXLinkText::HandleShortcut(key, modifiers);
}

#define JTemplateType GMessageView::LinkInfo
#include <JArray.tmpls>
#undef JTemplateType
