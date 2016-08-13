/******************************************************************************
 SVNTabGroup.cpp

	Displays a spinner next to the title of the busy tab.

	BASE CLASS = JXTabGroup

	Copyright © 2008 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "SVNTabGroup.h"
#include "SVNBusyTabTask.h"
#include <JXWindowPainter.h>
#include <JXImage.h>
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
	itsImageList = new JPtrArray<JXImage>(JPtrArrayT::kDeleteAll, kBusyIconCount);
	assert( itsImageList != NULL );

	JXDisplay* display   = enclosure->GetDisplay();
	JXColormap* colormap = enclosure->GetColormap();
	for (JIndex i=1; i<=kBusyIconCount; i++)
		{
		JXImage* icon = new JXImage(display, kBusyIcon[i-1]);
		assert( icon != NULL );
		itsImageList->Append(icon);
		}

	itsAnimationTask = new SVNBusyTabTask(this);
	assert( itsAnimationTask != NULL );

	ListenTo(GetCardEnclosure());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SVNTabGroup::~SVNTabGroup()
{
	delete itsAnimationTask;
	delete itsImageList;
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
		JXImage* image = itsImageList->NthElement(itsSpinnerIndex);
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
		const JXCardFile::CardRemoved* info =
			dynamic_cast<const JXCardFile::CardRemoved*>(&message);
		assert( info != NULL );

		JIndex i;
		const JBoolean hasIndex = info->GetCardIndex(&i);
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
