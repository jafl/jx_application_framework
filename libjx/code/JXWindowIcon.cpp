/******************************************************************************
 JXWindowIcon.cpp

	Draws the image for an iconified window.

	BASE CLASS = JXWidget

	Copyright © 2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXWindowIcon.h>
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXWindowPainter.h>
#include <JXImageMask.h>
#include <X11/extensions/shape.h>
#include <jAssert.h>

// JBroadcaster message types

const JCharacter* JXWindowIcon::kAcceptDrop  = "AcceptDrop::JXWindowIcon";
const JCharacter* JXWindowIcon::kHandleEnter = "HandleEnter::JXWindowIcon";
const JCharacter* JXWindowIcon::kHandleLeave = "HandleLeave::JXWindowIcon";
const JCharacter* JXWindowIcon::kHandleDrop  = "HandleDrop::JXWindowIcon";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXWindowIcon::JXWindowIcon
	(
	JXImage*			normalImage,
	JXImage*			dropImage,
	JXWindow*			mainWindow,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXWidget(enclosure, hSizing, vSizing, x,y, w,h),
	itsMainWindow(mainWindow)
{
	itsNormalImage = itsDropImage = NULL;	// makes SetIcons() safe
	SetIcons(normalImage, dropImage);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXWindowIcon::~JXWindowIcon()
{
	delete itsNormalImage;
	delete itsDropImage;
}

/******************************************************************************
 SetIcons

 ******************************************************************************/

void
JXWindowIcon::SetIcons
	(
	JXImage* normalImage,
	JXImage* dropImage
	)
{
	delete itsNormalImage;
	itsNormalImage = normalImage;
	itsNormalImage->ConvertToRemoteStorage();

	delete itsDropImage;
	itsDropImage = dropImage;
	itsDropImage->ConvertToRemoteStorage();

//	Can't change size because request is forwarded to main window!
//	(GetWindow())->SetSize(itsNormalImage->GetWidth(), itsNormalImage->GetHeight());
	Refresh();

	JXImageMask* mask;
	if (itsNormalImage->GetMask(&mask))
		{
		JXDisplay* display = GetDisplay();
		Window xWindow     = (GetWindow())->GetXWindow();
		int major, minor;
		if (XShapeQueryVersion(*display, &major, &minor))
			{
			mask->ConvertToPixmap();
			XShapeCombineMask(*display, xWindow, ShapeBounding, 0,0, mask->GetPixmap(), ShapeSet);
			XShapeCombineMask(*display, xWindow, ShapeClip,     0,0, mask->GetPixmap(), ShapeSet);
			}
		}
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
JXWindowIcon::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	JXImage* image = (IsDNDTarget() ? itsDropImage : itsNormalImage);
	p.Image(*image, image->GetBounds(), GetBounds());
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
JXWindowIcon::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
}

/******************************************************************************
 HandleMouseEnter (virtual protected)

	Request focus when mouse enters icon.

 ******************************************************************************/

void
JXWindowIcon::HandleMouseEnter()
{
//	(GetWindow())->RequestFocus();
}

/******************************************************************************
 WillAcceptDrop (virtual protected)

 ******************************************************************************/

JBoolean
JXWindowIcon::WillAcceptDrop
	(
	const JArray<Atom>&	typeList,
	Atom*				action,
	const Time			time,
	const JXWidget*		source
	)
{
	AcceptDrop msg(typeList, action, time, source);
	BroadcastWithFeedback(&msg);
	return msg.WillAcceptDrop();
}

/******************************************************************************
 HandleDNDEnter (virtual protected)

 ******************************************************************************/

void
JXWindowIcon::HandleDNDEnter()
{
	Broadcast(HandleEnter());
}

/******************************************************************************
 HandleDNDLeave (virtual protected)

 ******************************************************************************/

void
JXWindowIcon::HandleDNDLeave()
{
	Broadcast(HandleLeave());
}

/******************************************************************************
 HandleDNDDrop (virtual protected)

 ******************************************************************************/

void
JXWindowIcon::HandleDNDDrop
	(
	const JPoint&		pt,
	const JArray<Atom>&	typeList,
	const Atom			action,
	const Time			time,
	const JXWidget*		source
	)
{
	Atom a = action;
	Broadcast(HandleDrop(typeList, &a, time, source));
}
