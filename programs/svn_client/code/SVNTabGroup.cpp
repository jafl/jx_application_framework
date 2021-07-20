/******************************************************************************
 SVNTabGroup.cpp

	Displays a spinner next to the title of the busy tab.

	BASE CLASS = JXTabGroup

	Copyright (C) 2008 by John Lindal.

 ******************************************************************************/

#include "SVNTabGroup.h"
#include "SVNBusyTabTask.h"
#include <JXWindowPainter.h>
#include <JXImage.h>
#include <JXImageCache.h>
#include <jXGlobals.h>
#include <jAssert.h>

#include <jx_busy_1_small.xpm>
#include <jx_busy_2_small.xpm>
#include <jx_busy_3_small.xpm>
#include <jx_busy_4_small.xpm>
#include <jx_busy_5_small.xpm>
#include <jx_busy_6_small.xpm>
#include <jx_busy_7_small.xpm>
#include <jx_busy_8_small.xpm>

static JXPM kBusyIcon[] =
{
	jx_busy_1_small, jx_busy_2_small, jx_busy_3_small, jx_busy_4_small,
	jx_busy_5_small, jx_busy_6_small, jx_busy_7_small, jx_busy_8_small
};

const JSize kBusyIconCount = sizeof(kBusyIcon)/sizeof(JXPM);

const JCoordinate kImageWidth  = 16;
const JCoordinate kMarginWidth = 2;

/******************************************************************************
 Constructor

 ******************************************************************************/

SVNTabGroup::SVNTabGroup
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
	JXTabGroup(enclosure, hSizing, vSizing, x,y, w,h),
	itsBusyIndex(0)
{
	itsImageList = jnew JPtrArray<JXImage>(JPtrArrayT::kForgetAll, kBusyIconCount);
	assert( itsImageList != nullptr );

	JXImageCache* cache = enclosure->GetDisplay()->GetImageCache();
	for (JIndex i=1; i<=kBusyIconCount; i++)
		{
		itsImageList->Append(cache->GetImage(kBusyIcon[i-1]));
		}

	itsAnimationTask = jnew SVNBusyTabTask(this);
	assert( itsAnimationTask != nullptr );

	ListenTo(GetCardEnclosure());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SVNTabGroup::~SVNTabGroup()
{
	jdelete itsAnimationTask;
	jdelete itsImageList;
}

/******************************************************************************
 SetBusyIndex

 ******************************************************************************/

void
SVNTabGroup::SetBusyIndex
	(
	const JIndex index
	)
{
	ClearBusyIndex();

	itsBusyIndex    = index;
	itsSpinnerIndex = 1;

	if (itsBusyIndex > 0)
		{
		SetTabTitlePreMargin(itsBusyIndex, kImageWidth + 2*kMarginWidth);
		}

	itsAnimationTask->Start();
}

/******************************************************************************
 ClearBusyIndex

 ******************************************************************************/

void
SVNTabGroup::ClearBusyIndex()
{
	if (itsBusyIndex > 0)
		{
		SetTabTitlePreMargin(itsBusyIndex, 0);
		}

	itsBusyIndex = 0;
	itsAnimationTask->Stop();
}

/******************************************************************************
 IncrementSpinnerIndex

 ******************************************************************************/

void
SVNTabGroup::IncrementSpinnerIndex()
{
	itsSpinnerIndex++;
	if (itsSpinnerIndex > kBusyIconCount)
		{
		itsSpinnerIndex = 1;
		}

	Refresh();
}

/******************************************************************************
 DrawTab (virtual protected)

 ******************************************************************************/

void
SVNTabGroup::DrawTab
	(
	const JIndex		index,
	JXWindowPainter&	p,
	const JRect&		rect,
	const Edge			edge
	)
{
	if (index == itsBusyIndex)
		{
		JXImage* image = itsImageList->GetElement(itsSpinnerIndex);
		p.JPainter::Image(*image, image->GetBounds(),
						  rect.left + kMarginWidth, rect.ycenter() - (image->GetHeight()/2));
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
SVNTabGroup::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (message.Is(JXCardFile::kCardRemoved))
		{
		const auto* info =
			dynamic_cast<const JXCardFile::CardRemoved*>(&message);
		assert( info != nullptr );

		JIndex i;
		const bool hasIndex = info->GetCardIndex(&i);
		if (hasIndex && i < itsBusyIndex)
			{
			itsBusyIndex--;
			}
		else if (hasIndex && i == itsBusyIndex)
			{
			itsBusyIndex = 0;
			ClearBusyIndex();
			}
		}

	JXTabGroup::Receive(sender, message);
}
