/******************************************************************************
 CMLineAddressTable.cpp

	BASE CLASS = CMLineIndexTable

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "CMLineAddressTable.h"
#include "CMSourceDirector.h"
#include "CMBreakpointManager.h"
#include "CMLink.h"
#include <JXColorManager.h>
#include <JStringIterator.h>
#include <JListUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CMLineAddressTable::CMLineAddressTable
	(
	CMSourceDirector*	dir,
	CMSourceText*		text,
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
	CMLineIndexTable(CompareBreakpointAddresses, dir, text, scrollbarSet, enclosure, hSizing, vSizing, x, y, w, h)
{
	itsVisualBPIndexList = jnew JArray<JIndex>;
	assert( itsVisualBPIndexList != nullptr );

	itsLineTextList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsLineTextList != nullptr );
	itsLineTextList->SetCompareFunction(JCompareStringsCaseInsensitive);
	itsLineTextList->SetSortOrder(JListT::kSortAscending);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMLineAddressTable::~CMLineAddressTable()
{
	jdelete itsVisualBPIndexList;
	jdelete itsLineTextList;
}

/******************************************************************************
 SetLineNumbers

	We take ownership of the items in the list.

 ******************************************************************************/

void
CMLineAddressTable::SetLineNumbers
	(
	JPtrArray<JString>*	list
	)
{
	itsLineTextList->CopyPointers(*list, JPtrArrayT::kDeleteAll, false);
	list->SetCleanUpAction(JPtrArrayT::kForgetAll);

	if (itsLineTextList->IsEmpty())
		{
		return;
		}

	JSize charCount = 0;
	const JString* s1 = itsLineTextList->GetFirstElement();
	const JString* sN = itsLineTextList->GetLastElement();
	if (s1->BeginsWith("0x") && s1->GetCharacterCount() > 2 &&
		sN->BeginsWith("0x") && sN->GetCharacterCount() > 2)
		{
		charCount = 2;

		if (s1->GetCharacterCount() == sN->GetCharacterCount())
			{
			JStringIterator iter(*sN, kJIteratorStartAfter, charCount);
			JUtf8Character c;
			while (iter.Next(&c) && c == '0')
				{
				charCount++;
				}
			}
		}

	if (charCount > 0)
		{
		for (auto* s : *itsLineTextList)
			{
			JStringIterator iter(s);
			iter.RemoveNext(charCount);
			}
		}
}

/******************************************************************************
 ClearLineNumbers

 ******************************************************************************/

void
CMLineAddressTable::ClearLineNumbers()
{
	itsLineTextList->CleanOut();
}

/******************************************************************************
 FindAddressLineNumber

 ******************************************************************************/

bool
CMLineAddressTable::FindAddressLineNumber
	(
	const JString&	origAddress,
	JIndex*			index
	)
	const
{
	JString addr = GetLineTextFromAddress(origAddress);

	bool found;
	*index = itsLineTextList->SearchSorted1(&addr, JListT::kAnyMatch, &found);
	return true;
}

/******************************************************************************
 GetCurrentLineMarkerColor (virtual protected)

 ******************************************************************************/

JColorID
CMLineAddressTable::GetCurrentLineMarkerColor()
	const
{
	return JColorManager::GetMagentaColor();
}

/******************************************************************************
 GetLineText (virtual protected)

 ******************************************************************************/

JString
CMLineAddressTable::GetLineText
	(
	const JIndex lineIndex
	)
	const
{
	return itsLineTextList->IndexValid(lineIndex) ?
		*(itsLineTextList->GetElement(lineIndex)) : JString();
}

/******************************************************************************
 GetLongestLineText (virtual protected)

 ******************************************************************************/

JString
CMLineAddressTable::GetLongestLineText
	(
	const JIndex lineCount
	)
	const
{
	return !itsLineTextList->IsEmpty() ?
		*itsLineTextList->GetLastElement() : JString((JUInt64) lineCount);
}

/******************************************************************************
 GetBreakpointLineIndex (virtual protected)

 ******************************************************************************/

JIndex
CMLineAddressTable::GetBreakpointLineIndex
	(
	const JIndex		bpIndex,
	const CMBreakpoint*	bp
	)
	const
{
	return itsVisualBPIndexList->GetElement(bpIndex);
}

/******************************************************************************
 GetFirstBreakpointOnLine (virtual protected)

 ******************************************************************************/

bool
CMLineAddressTable::GetFirstBreakpointOnLine
	(
	const JIndex	lineIndex,
	JIndex*			bpIndex
	)
	const
{
	const JString addr = BuildAddress(*itsLineTextList->GetElement(lineIndex));
	CMBreakpoint bp(addr);
	return GetBreakpointList()->SearchSorted(&bp, JListT::kFirstMatch, bpIndex);
}

/******************************************************************************
 BreakpointsOnSameLine (virtual protected)

 ******************************************************************************/

bool
CMLineAddressTable::BreakpointsOnSameLine
	(
	const CMBreakpoint* bp1,
	const CMBreakpoint* bp2
	)
	const
{
	return bp1->GetAddress() == bp2->GetAddress();
}

/******************************************************************************
 GetBreakpoints (virtual protected)

 ******************************************************************************/

void
CMLineAddressTable::GetBreakpoints
	(
	JPtrArray<CMBreakpoint>* list
	)
{
	itsVisualBPIndexList->RemoveAll();

	const JString* fnName;
	if (!GetDirector()->GetFunctionName(&fnName))
		{
		list->CleanOut();
		return;
		}

	CMLocation loc;
	loc.SetFunctionName(*fnName);
	if (!GetLink()->GetBreakpointManager()->GetBreakpoints(loc, list))
		{
		return;
		}
	list->Sort();

	const JSize count = list->GetElementCount();
	JString target;
	for (JIndex i=1; i<=count; i++)
		{
		const CMBreakpoint* bp = list->GetElement(i);

		target = GetLineTextFromAddress(bp->GetAddress());

		bool found;
		const JIndex j = itsLineTextList->SearchSorted1(&target, JListT::kAnyMatch, &found);
		itsVisualBPIndexList->AppendElement(j);
		}
}

/******************************************************************************
 SetBreakpoint (virtual protected)

 ******************************************************************************/

void
CMLineAddressTable::SetBreakpoint
	(
	const JIndex	lineIndex,
	const bool	temporary
	)
{
	const JString addr = BuildAddress(*itsLineTextList->GetElement(lineIndex));
	GetLink()->SetBreakpoint(addr, temporary);
}

/******************************************************************************
 RemoveAllBreakpointsOnLine (virtual protected)

 ******************************************************************************/

void
CMLineAddressTable::RemoveAllBreakpointsOnLine
	(
	const JIndex lineIndex
	)
{
	const JString addr = BuildAddress(*itsLineTextList->GetElement(lineIndex));
	GetLink()->RemoveAllBreakpointsAtAddress(addr);
}

/******************************************************************************
 RunUntil (virtual protected)

 ******************************************************************************/

void
CMLineAddressTable::RunUntil
	(
	const JIndex lineIndex
	)
{
	const JString addr = BuildAddress(*itsLineTextList->GetElement(lineIndex));
	GetLink()->RunUntil(addr);
}

/******************************************************************************
 SetExecutionPoint (virtual protected)

 ******************************************************************************/

void
CMLineAddressTable::SetExecutionPoint
	(
	const JIndex lineIndex
	)
{
	const JString addr = BuildAddress(*itsLineTextList->GetElement(lineIndex));
	GetLink()->SetExecutionPoint(addr);
}

/******************************************************************************
 BuildAddress (static private)

 ******************************************************************************/

JString
CMLineAddressTable::BuildAddress
	(
	const JString& addr
	)
{
	JString s = addr;
	JStringIterator iter(&s);
	if (iter.Next(" "))
		{
		iter.SkipPrev();
		iter.RemoveAllNext();
		}

	s.Prepend("0x");
	return s;
}

/******************************************************************************
 GetLineTextFromAddress (private)

 ******************************************************************************/

JString
CMLineAddressTable::GetLineTextFromAddress
	(
	const JString& addr
	)
	const
{
	JString s = addr;
	JStringIterator iter(&s);
	if (s.BeginsWith("0x"))
		{
		iter.RemoveNext(2);
		}

	if (itsLineTextList->IsEmpty())
		{
		return s;
		}

	while (s.GetFirstCharacter() == '0' &&
		   s.GetCharacterCount() > itsLineTextList->GetLastElement()->GetCharacterCount())
		{
		iter.RemoveNext();
		}

	return s;
}

/******************************************************************************
 CompareBreakpointAddresses (static private)

 ******************************************************************************/

JListT::CompareResult
CMLineAddressTable::CompareBreakpointAddresses
	(
	CMBreakpoint* const & bp1,
	CMBreakpoint* const & bp2
	)
{
	return JCompareStringsCaseInsensitive(
		const_cast<JString*>(&(bp1->GetAddress())),
		const_cast<JString*>(&(bp2->GetAddress())));
}
