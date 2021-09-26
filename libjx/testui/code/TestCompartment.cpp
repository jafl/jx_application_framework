/******************************************************************************
 TestCompartment.cpp

	Class for testing JXHorizPartition and JXVertPartition.

	BASE CLASS = JXWidget

	Written by John Lindal.

 ******************************************************************************/

#include "TestCompartment.h"
#include "TestPartitionDirector.h"
#include <jx-af/jx/JXHorizPartition.h>
#include <jx-af/jx/JXVertPartition.h>
#include <jx-af/jx/JXWindowPainter.h>
#include <jx-af/jx/jXPainterUtil.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

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
	JXWidget(enclosure, hSizing, vSizing, x,y, w,h)
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
 IsElastic (private)

 ******************************************************************************/

bool
TestCompartment::IsElastic()
	const
{
	bool isElastic = false;

	JXContainer* enclosure           = GetEnclosure();
	const JXContainer* partition     = enclosure->GetEnclosure();
	JXHorizPartition* horizPartition = itsDirector->GetHorizPartition();
	JXVertPartition* vertPartition   = itsDirector->GetVertPartition();

	if (partition == horizPartition)
	{
		JIndex ourIndex;
		const bool ok = horizPartition->GetCompartmentIndex(enclosure, &ourIndex);
		assert( ok );

		JIndex elasticIndex;
		horizPartition->GetElasticIndex(&elasticIndex);

		isElastic = ourIndex == elasticIndex || elasticIndex == 0;
	}
	else if (partition == vertPartition)
	{
		JIndex ourIndex;
		const bool ok = vertPartition->GetCompartmentIndex(enclosure, &ourIndex);
		assert( ok );

		JIndex elasticIndex;
		vertPartition->GetElasticIndex(&elasticIndex);

		isElastic = ourIndex == elasticIndex || elasticIndex == 0;
	}

	return isElastic;
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
TestCompartment::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
	JXDrawDownFrame(p, frame, GetBorderWidth());
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
	const bool isElastic = IsElastic();

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
		p.SetFontStyle(JColorManager::GetInactiveLabelColor());
	}

	p.String(ap.left, top, JGetString("LeftClickInstr::TestCompartment"),
			 ap.width(), JPainter::kHAlignCenter);
	top += lineHeight;
	p.String(ap.left, top, JGetString("MiddleClickInstr::TestCompartment"),
			 ap.width(), JPainter::kHAlignCenter);
	top += lineHeight;
	p.String(ap.left, top, JGetString("RightClickInstr::TestCompartment"),
			 ap.width(), JPainter::kHAlignCenter);

	if (isElastic)
	{
		top += lineHeight;
		p.String(ap.left, top, JGetString("ElasticLabel::TestCompartment"),
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
		const bool ok = horizPartition->GetCompartmentIndex(enclosure, &ourIndex);
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
		const bool ok = vertPartition->GetCompartmentIndex(enclosure, &ourIndex);
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

/******************************************************************************
 GetFTCMinContentSize (virtual protected)

 ******************************************************************************/

JCoordinate
TestCompartment::GetFTCMinContentSize
	(
	const bool horizontal
	)
	const
{
	const JFont& f = JFontManager::GetDefaultFont();

	return (horizontal ?
			(f.GetStringWidth(GetFontManager(), JGetString("LeftClickInstr::TestCompartment")) + 10) :
			(f.GetLineHeight(GetFontManager()) * (IsElastic() ? 4 : 3) + 10));
}
