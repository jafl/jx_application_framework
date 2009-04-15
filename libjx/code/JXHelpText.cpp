/******************************************************************************
 JXHelpText.cpp

	Displays hypertext for JXHelp system.

	BASE CLASS = JXLinkText

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXHelpText.h>
#include <JXHelpDirector.h>
#include <JXHelpManager.h>
#include <JXWindow.h>
#include <JXColormap.h>
#include <jXGlobals.h>
#include <jXKeysym.h>
#include <JPagePrinter.h>
#include <jASCIIConstants.h>
#include <strstream>
#include <ctype.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXHelpText::JXHelpText
	(
	const JCharacter*	title,
	const JCharacter*	text,
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
	JXLinkText(scrollbarSet, enclosure, hSizing, vSizing, x,y, w,h),
	itsTitle(title)
{
	itsMarks = new JArray<MarkInfo>;
	assert( itsMarks != NULL );
	itsMarks->SetCompareFunction(CompareMarkNames);

	itsLinks = new JArray<LinkInfo>;
	assert( itsLinks != NULL );

	itsAnchorText = NULL;

	// set text

	TESetLeftMarginWidth(kMinLeftMarginWidth);

	std::istrstream input(text, strlen(text));
	ReadHTML(input);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXHelpText::~JXHelpText()
{
JIndex i;

	const JSize markCount = itsMarks->GetElementCount();
	for (i=1; i<=markCount; i++)
		{
		MarkInfo info = itsMarks->GetElement(i);
		delete info.name;
		}
	delete itsMarks;

	const JSize linkCount = itsLinks->GetElementCount();
	for (i=1; i<=linkCount; i++)
		{
		LinkInfo info = itsLinks->GetElement(i);
		delete info.url;
		}
	delete itsLinks;

	delete itsAnchorText;
}

/******************************************************************************
 ShowSubsection

 ******************************************************************************/

void
JXHelpText::ShowSubsection
	(
	const JCharacter* name
	)
{
	if (JStringEmpty(name))
		{
		ScrollTo(0,0);
		}
	else
		{
		JString s = name;
		MarkInfo info(&s, 0);
		JIndex index;
		if (itsMarks->SearchSorted(info, JOrderedSetT::kAnyMatch, &index))
			{
			info = itsMarks->GetElement(index);
			ScrollTo(0, GetLineTop(GetLineForChar(info.index)));
			}
		}
}

/******************************************************************************
 Print header & footer (virtual protected)

 ******************************************************************************/

JCoordinate
JXHelpText::GetPrintHeaderHeight
	(
	JPagePrinter& p
	)
	const
{
	return (itsTitle.IsEmpty() ? 0 : 3 * p.GetLineHeight());
}

void
JXHelpText::DrawPrintHeader
	(
	JPagePrinter&		p,
	const JCoordinate	footerHeight
	)
{
	if (!itsTitle.IsEmpty())
		{
		p.SetFontStyle(JFontStyle(kJTrue, kJFalse, 1, kJFalse));
		p.String(p.GetPageRect(), itsTitle, JPainter::kHAlignCenter);
		p.SetFontStyle(JFontStyle());
		}
}

/******************************************************************************
 GetLinkCount (virtual protected)

 ******************************************************************************/

JSize
JXHelpText::GetLinkCount()
	const
{
	return itsLinks->GetElementCount();
}

/******************************************************************************
 GetLinkRange (virtual protected)

 ******************************************************************************/

JIndexRange
JXHelpText::GetLinkRange
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
JXHelpText::LinkClicked
	(
	const JIndex index
	)
{
	JXHelpDirector* helpDir =
		dynamic_cast(JXHelpDirector*, (GetWindow())->GetDirector());
	assert( helpDir != NULL );

	const LinkInfo info = itsLinks->GetElement(index);
	(JXGetHelpManager())->ShowURL(*(info.url), helpDir);
}

/******************************************************************************
 PrepareToPasteHTML (virtual protected)

 ******************************************************************************/

void
JXHelpText::PrepareToPasteHTML()
{
	assert( 0 /* JXHelpText doesn't support PasteHTML() */ );
}

/******************************************************************************
 PrepareToReadHTML (virtual protected)

 ******************************************************************************/

void
JXHelpText::PrepareToReadHTML()
{
	JXLinkText::PrepareToReadHTML();

	itsAnchorText = new JString;
	assert( itsAnchorText != NULL );

	ClearAnchorInfo();
}

/******************************************************************************
 ReadHTMLFinished (virtual protected)

 ******************************************************************************/

void
JXHelpText::ReadHTMLFinished()
{
JIndex i,j;

	JXLinkText::ReadHTMLFinished();

	delete itsAnchorText;
	itsAnchorText = NULL;

	// shift each mark to nearest non-blank line

	const JString& text    = GetText();
	const JSize textLength = GetTextLength();

	const JSize markCount = itsMarks->GetElementCount();
	for (i=1; i<=markCount; i++)
		{
		MarkInfo info = itsMarks->GetElement(i);
		while (info.index < textLength &&
			   text.GetCharacter(info.index) == '\n')
			{
			(info.index)++;
			}
		itsMarks->SetElement(i, info);
		}

	// highlight each link and display URLs for non-local links

	const JColorIndex blueColor = (GetColormap())->GetBlueColor();

	const JSize linkCount = itsLinks->GetElementCount();
	for (i=1; i<=linkCount; i++)
		{
		const LinkInfo info = itsLinks->GetElement(i);
		SetFontColor(info.range.first, info.range.last, blueColor, kJFalse);
		SetFontUnderline(info.range.first, info.range.last, 1, kJFalse);

		if (!JXHelpManager::IsLocalURL(*(info.url)))
			{
			JBoolean showURL = kJTrue;

			JString url = *(info.url);
			if (url.BeginsWith("mailto:", kJFalse))
				{
				url.RemoveSubstring(1, 7);

				// don't display address if it's already there
				if (text.GetSubstring(info.range) == url)
					{
					showURL = kJFalse;
					}
				}

			if (showURL)
				{
				const JIndex pasteIndex = info.range.last+1;
				SetCaretLocation(pasteIndex);
				const JString s = " (" + url + ")";
				Paste(s);
				JIndexRange r(pasteIndex, pasteIndex + s.GetLength()-1);
				SetFontName(r.first, r.last, JGetMonospaceFontName(), kJFalse);
				SetFontStyle(r.first, r.last, GetDefaultFontStyle(), kJFalse);

				const JSize delta = s.GetLength();

				for (j=i+1; j<=linkCount; j++)
					{
					LinkInfo link = itsLinks->GetElement(j);
					link.range   += delta;
					itsLinks->SetElement(j, link);
					}

				for (j=1; j<=markCount; j++)
					{
					MarkInfo mark = itsMarks->GetElement(j);
					if (mark.index > info.range.last)
						{
						mark.index += delta;
						itsMarks->SetElement(j, mark);
						}
					}
				}
			}
		}

	SetCaretLocation(1);
	ClearUndo();
}

/******************************************************************************
 HandleHTMLTag (virtual protected)

 ******************************************************************************/

void
JXHelpText::HandleHTMLTag
	(
	const JString&					name,
	const JStringPtrMap<JString>&	attr
	)
{
	if (name == "a")
		{
		BeginAnchor(attr);
		}
	else if (name == "/a" && itsAnchorRange.first > 0)
		{
		EndAnchor();
		}
	else
		{
		JXLinkText::HandleHTMLTag(name, attr);
		}
}

/******************************************************************************
 BeginAnchor (private)

 ******************************************************************************/

void
JXHelpText::BeginAnchor
	(
	const JStringPtrMap<JString>& attr
	)
{
	const JString* valueStr;
	if (attr.GetElement("href", &valueStr) && valueStr != NULL)
		{
		if (!itsAnchorText->IsEmpty())
			{
			EndAnchor();
			}

		itsAnchorRange.first = GetHTMLBufferLength()+1;
		*itsAnchorText       = *valueStr;
		}
	else if (attr.GetElement("name", &valueStr) && valueStr != NULL)
		{
		MarkInfo info(new JString(*valueStr), GetHTMLBufferLength()+1);
		assert( info.name != NULL );

		if (!itsMarks->InsertSorted(info, kJFalse))
			{
			delete info.name;
			}
		}
}

/******************************************************************************
 EndAnchor (private)

 ******************************************************************************/

void
JXHelpText::EndAnchor()
{
	assert( itsAnchorRange.first > 0 );

	itsAnchorRange.last = GetHTMLBufferLength();

	const JString& text = GetText();
	while (itsAnchorRange.first <= itsAnchorRange.last &&
		   isspace(text.GetCharacter(itsAnchorRange.first)))
		{
		(itsAnchorRange.first)++;
		}
	while (itsAnchorRange.first <= itsAnchorRange.last &&
		   isspace(text.GetCharacter(itsAnchorRange.last)))
		{
		(itsAnchorRange.last)--;
		}

	if (!itsAnchorRange.IsEmpty() && !itsAnchorText->IsEmpty())
		{
		LinkInfo info(itsAnchorRange, new JString(*itsAnchorText));
		assert( info.url != NULL );
		itsLinks->AppendElement(info);
		}

	ClearAnchorInfo();
}

/******************************************************************************
 ClearAnchorInfo (private)

 ******************************************************************************/

void
JXHelpText::ClearAnchorInfo()
{
	itsAnchorRange.SetToNothing();
	itsAnchorText->Clear();
}

/******************************************************************************
 CompareMarkNames (static private)

 ******************************************************************************/

JOrderedSetT::CompareResult
JXHelpText::CompareMarkNames
	(
	const MarkInfo& m1,
	const MarkInfo& m2
	)
{
	return JCompareStringsCaseInsensitive(m1.name, m2.name);
}

#define JTemplateType JXHelpText::MarkInfo
#include <JArray.tmpls>
#undef JTemplateType

#define JTemplateType JXHelpText::LinkInfo
#include <JArray.tmpls>
#undef JTemplateType
