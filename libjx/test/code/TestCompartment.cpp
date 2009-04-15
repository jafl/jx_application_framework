/******************************************************************************
 TestCompartment.cpp

	Class for testing JXHorizPartition and JXVertPartition.

	BASE CLASS = JXDownRect

	Written by John Lindal.

 ******************************************************************************/

#include <JXStdInc.h>
#include "TestCompartment.h"
#include "TestPartitionDirector.h"
#include <JXHorizPartition.h>
#include <JXVertPartition.h>
#include <JXWindowPainter.h>
#include <JXColormap.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TestCompartment::TestCompartment
	(
	TestPartitionDirector*	director,
	JXContainer*			enclosure,
	const HSizingOption		hSizing,
	const VSizingOption		vSizing,
	const JCoordinate		x,
	const JCoordinate		y,
	const JCoordinate		w,
	const JCoordinate		h
	)
	:
	JXDownRect(enclosure, hSizing, vSizing, x,y, w,h)
{
	itsDirector = director;
	SetBorderWidth(kJXDefaultBorderWidth);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TestCompartment::~TestCompartment()
{
}

/******************************************************************************
 Draw (virtual protected)

	Center the message in the aperture.

 ******************************************************************************/

void
TestCompartment::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	// figure out if we are elastic

	JBoolean isElastic = kJFalse;

	JXContainer* enclosure           = GetEnclosure();
	const JXContainer* partition     = enclosure->GetEnclosure();
	JXHorizPartition* horizPartition = itsDirector->GetHorizPartition();
	JXVertPartition* vertPartition   = itsDirector->GetVertPartition();

	if (partition == horizPartition)
		{
		JIndex ourIndex;
		const JBoolean ok = horizPartition->GetCompartmentIndex(enclosure, &ourIndex);
		assert( ok );

		JIndex elasticIndex;
		horizPartition->GetElasticIndex(&elasticIndex);

		isElastic = JConvertToBoolean( ourIndex == elasticIndex || elasticIndex == 0 );
		}
	else if (partition == vertPartition)
		{
		JIndex ourIndex;
		const JBoolean ok = vertPartition->GetCompartmentIndex(enclosure, &ourIndex);
		assert( ok );

		JIndex elasticIndex;
		vertPartition->GetElasticIndex(&elasticIndex);

		isElastic = JConvertToBoolean( ourIndex == elasticIndex || elasticIndex == 0 );
		}

	// draw text

	const JRect ap         = GetAperture();
	const JSize lineHeight = p.GetLineHeight();
	JCoordinate top        = ap.ycenter() - lineHeight;
	if (isElastic)
		{
		top -= lineHeight;
		}
	else
		{
		top -= lineHeight/2;
		}

	if (!IsActive())
		{
		p.SetFontStyle((p.GetColormap())->GetInactiveLabelColor());
		}

	p.String(ap.left, top, "Left click to insert in front of",
			 ap.width(), JPainter::kHAlignCenter);
	top += lineHeight;
	p.String(ap.left, top, "Middle click to delete",
			 ap.width(), JPainter::kHAlignCenter);
	top += lineHeight;
	p.String(ap.left, top, "Right click to insert after",
			 ap.width(), JPainter::kHAlignCenter);

	if (isElastic)
		{
		top += lineHeight;
		p.String(ap.left, top, "(elastic)",
				 ap.width(), JPainter::kHAlignCenter);
		}
}

/******************************************************************************
 HandleMouseUp (virtual protected)

	Do what our message says we do.

 ******************************************************************************/

void
TestCompartment::HandleMouseUp
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JXContainer* enclosure       = GetEnclosure();
	const JXContainer* partition = enclosure->GetEnclosure();

	JXHorizPartition* horizPartition = itsDirector->GetHorizPartition();
	JXVertPartition* vertPartition   = itsDirector->GetVertPartition();

	if (partition == horizPartition)
		{
		JIndex ourIndex;
		const JBoolean ok = horizPartition->GetCompartmentIndex(enclosure, &ourIndex);
		assert( ok );

		if (button == kJXLeftButton)
			{
			itsDirector->InsertHorizCompartment(ourIndex);
			}
		else if (button == kJXMiddleButton)
			{
			itsDirector->DeleteHorizCompartment(ourIndex);	// commit suicide
			}
		else if (button == kJXRightButton)
			{
			itsDirector->InsertHorizCompartment(ourIndex+1);
			}
		}

	else if (partition == vertPartition)
		{
		JIndex ourIndex;
		const JBoolean ok = vertPartition->GetCompartmentIndex(enclosure, &ourIndex);
		assert( ok );

		if (button == kJXLeftButton)
			{
			itsDirector->InsertVertCompartment(ourIndex);
			}
		else if (button == kJXMiddleButton)
			{
			itsDirector->DeleteVertCompartment(ourIndex);	// commit suicide
			}
		else if (button == kJXRightButton)
			{
			itsDirector->InsertVertCompartment(ourIndex+1);
			}
		}
}
