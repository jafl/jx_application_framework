/******************************************************************************
 SVNUpdateList.cc

	BASE CLASS = SVNListBase

	Copyright @ 2008 by John Lindal.  All rights reserved.

 ******************************************************************************/

#include <svnStdInc.h>
#include "SVNUpdateList.h"
#include "svnGlobals.h"
#include <JProcess.h>
#include <JRegex.h>
#include <jAssert.h>

static const JRegex revisionPattern      = "revision [0-9]+\\.$";
static const JCharacter* conflictPattern = "conflicts:";

/******************************************************************************
 Constructor

 ******************************************************************************/

SVNUpdateList::SVNUpdateList
	(
	SVNMainDirector*	director,
	JXTextMenu*			editMenu,
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption hSizing,
	const VSizingOption vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	SVNListBase(director, editMenu, "svn --non-interactive update",
				kJFalse, kJTrue, kJTrue, kJTrue,
				scrollbarSet, enclosure, hSizing, vSizing, x, y, w, h),
	itsHasRefreshedFlag(kJFalse)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SVNUpdateList::~SVNUpdateList()
{
}

/******************************************************************************
 RefreshContent (virtual)

 ******************************************************************************/

void
SVNUpdateList::RefreshContent()
{
	if (!itsHasRefreshedFlag)
	{
		SVNListBase::RefreshContent();
		itsHasRefreshedFlag = kJTrue;
	}
}

/******************************************************************************
 StyleLine (virtual protected)

 ******************************************************************************/

void
SVNUpdateList::StyleLine
	(
	const JIndex		index,
	const JString&		line,
	const JFontStyle&	errorStyle,
	const JFontStyle&	addStyle,
	const JFontStyle&	removeStyle
	)
{
	const JCharacter c1 = line.GetCharacter(1);
	const JCharacter c2 = line.GetCharacter(2);
	if (c1 == 'C' || c2 == 'C')
		{
		SetStyle(index, errorStyle);
		}
	else if (c1 == 'A' && c2 != 't')
		{
		SetStyle(index, addStyle);
		}
	else if (c1 == 'D')
		{
		SetStyle(index, removeStyle);
		}
}

/******************************************************************************
 ExtractRelativePath (virtual protected)

 ******************************************************************************/

JString
SVNUpdateList::ExtractRelativePath
	(
	const JString& line
	)
	const
{
	JString s = line.GetSubstring(6, line.GetLength());
	s.TrimWhitespace();
	return s;
}

/******************************************************************************
 ShouldDisplayLine (virtual protected)

	Return kJFalse if the line should not be displayed.

 ******************************************************************************/

JBoolean
SVNUpdateList::ShouldDisplayLine
	(
	JString* line
	)
	const
{
	return !line->Contains(conflictPattern);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
SVNUpdateList::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (message.Is(JProcess::kFinished))
		{
		JPtrArray<JString>*	list = GetLineList();
		const JSize count        = list->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			const JString* line = list->NthElement(i);
			if (revisionPattern.Match(*line))
				{
				list->MoveElementToIndex(i, 1);
				SetStyle(1, JFontStyle(kJTrue, kJFalse, 0, kJFalse));
				break;
				}
			}
		}

	SVNListBase::Receive(sender, message);
}
