/******************************************************************************
 JXCursorAnimator.cpp

	This class simplifies the animation of cursors.

	During a long process, call NextFrame() regularly, and this class will
	ensure that the cursor twirls smoothly.  The default animation is the
	a beach ball.  You can change this with SetFrameSequence().

	We are owned by a particular JXWidget.  This object must activate us
	when it controls the cursor, and must deactivate us otherwise.  This
	allows an unlimited number of simultaneously animated cursors, with
	at most one visible at any one time.

	This class was not designed to be a base class.

	BASE CLASS = none

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXCursorAnimator.h>
#include <JXDisplay.h>
#include <JXWindow.h>
#include <jXGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXCursorAnimator::JXCursorAnimator
	(
	JXWindow* window
	)
{
	itsWindow     = window;
	itsActiveFlag = kJFalse;

	InitFrames(itsWindow->GetDisplay());
	ResetFrameCounter();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXCursorAnimator::~JXCursorAnimator()
{
	jdelete itsFrames;
}

/******************************************************************************
 SetFrameSequence

 ******************************************************************************/

void
JXCursorAnimator::SetFrameSequence
	(
	const JArray<JCursorIndex>& frames
	)
{
	*itsFrames = frames;
	ResetFrameCounter();
}

/******************************************************************************
 NextFrame

	Show the next frame of the cursor -if enough time has elapsed-.

 ******************************************************************************/

void
JXCursorAnimator::NextFrame()
{
	const Time currentTime = (JXGetApplication())->GetCurrentTime();
	if (itsActiveFlag && currentTime - itsLastUpdateTime >= kmsecPerFrame)
		{
		itsFrameCounter++;
		if (itsFrameCounter > itsFrames->GetElementCount())
			{
			ResetFrameCounter();
			}

		itsWindow->DisplayXCursor(itsFrames->GetElement(itsFrameCounter));
		itsLastUpdateTime = currentTime;
		}
}

/******************************************************************************
 InitFrames (private)

	We start with the Macintosh spinning beachball.

 ******************************************************************************/

static const JUtf8Byte* kBeachballFrameName[] =
{
	"JXBeachballFrame1", "JXBeachballFrame2",
	"JXBeachballFrame3", "JXBeachballFrame4"
};

#include <jx_beachball_1_cursor.xbm>
#include <jx_beachball_2_cursor.xbm>
#include <jx_beachball_3_cursor.xbm>
#include <jx_beachball_4_cursor.xbm>
#include <jx_beachball_cursor_mask.xbm>

static const JXCursor kBeachBallFrames[] =
{
	{ jx_beachball_cursor_mask_width, jx_beachball_cursor_mask_height, 8,8, jx_beachball_1_cursor_bits, jx_beachball_cursor_mask_bits },
	{ jx_beachball_cursor_mask_width, jx_beachball_cursor_mask_height, 8,8, jx_beachball_2_cursor_bits, jx_beachball_cursor_mask_bits },
	{ jx_beachball_cursor_mask_width, jx_beachball_cursor_mask_height, 8,8, jx_beachball_3_cursor_bits, jx_beachball_cursor_mask_bits },
	{ jx_beachball_cursor_mask_width, jx_beachball_cursor_mask_height, 8,8, jx_beachball_4_cursor_bits, jx_beachball_cursor_mask_bits }
};	

const JSize kBeachBallFrameCount = sizeof(kBeachBallFrames) / sizeof(JXCursor);

void
JXCursorAnimator::InitFrames
	(
	JXDisplay* display
	)
{
	itsFrames = jnew JArray<JCursorIndex>(kBeachBallFrameCount);
	assert( itsFrames != NULL );

	for (JIndex i=0; i<kBeachBallFrameCount; i++)
		{
		const JCursorIndex cursIndex =
			display->CreateCustomCursor(kBeachballFrameName[i],
										kBeachBallFrames[i]);
		itsFrames->AppendElement(cursIndex);
		}
}
