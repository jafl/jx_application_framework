/******************************************************************************
 SVNUpdateList.cpp

	BASE CLASS = SVNListBase

	Copyright @ 2008 by John Lindal.

 ******************************************************************************/

#include "SVNUpdateList.h"
#include "svnGlobals.h"
#include <JProcess.h>
#include <JRegex.h>
#include <JStringIterator.h>
#include <jAssert.h>

static const JRegex revisionPattern = "revision [0-9]+\\.$";
static const JString conflictPattern("conflicts:", JString::kNoCopy);

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
	SVNListBase(director, editMenu, JString("svn --non-interactive update", JString::kNoCopy),
				false, true, true, true,
				scrollbarSet, enclosure, hSizing, vSizing, x, y, w, h),
	itsHasRefreshedFlag(false)
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
		itsHasRefreshedFlag = true;
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
	JStringIterator iter(line);
	JUtf8Character c1, c2;
	iter.Next(&c1);
	iter.Next(&c2);
	iter.Invalidate();

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
	return SVNListBase::ExtractRelativePath(line, 6);
}

/******************************************************************************
 ShouldDisplayLine (virtual protected)

	Return false if the line should not be displayed.

 ******************************************************************************/

bool
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
			const JString* line = list->GetElement(i);
			if (revisionPattern.Match(*line))
				{
				list->MoveElementToIndex(i, 1);
				SetStyle(1, JFontStyle(true, false, 0, false));
				break;
				}
			}
		}

	SVNListBase::Receive(sender, message);
}
