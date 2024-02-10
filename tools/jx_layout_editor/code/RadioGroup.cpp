/******************************************************************************
 RadioGroup.cpp

	BASE CLASS = ContainerWidget

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "RadioGroup.h"
#include "RadioGroupPanel.h"
#include <jx-af/jx/JXWindowPainter.h>
#include <jx-af/jx/jXPainterUtil.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

RadioGroup::RadioGroup
	(
	LayoutContainer*	layout,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	ContainerWidget(false, layout, hSizing, vSizing, x,y, w,h)
{
	RadioGroupX();
}

RadioGroup::RadioGroup
	(
	std::istream&		input,
	const JFileVersion	vers,
	LayoutContainer*	layout,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	ContainerWidget(input, vers, layout, hSizing, vSizing, x,y, w,h)
{
	RadioGroupX();

	if (vers >= 8)
	{
		bool hideBorder;
		input >> hideBorder;
		if (hideBorder)
		{
			SetBorderWidth(0);
		}
	}
}

// private

void
RadioGroup::RadioGroupX()
{
	SetBorderWidth(2);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

RadioGroup::~RadioGroup()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
RadioGroup::StreamOut
	(
	std::ostream& output
	)
	const
{
	output << JString("RadioGroup") << std::endl;

	ContainerWidget::StreamOut(output);

	output << (GetBorderWidth() == 0) << std::endl;
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
RadioGroup::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
	JXDrawEngravedFrame(p, frame, 1, 0, 1);
}

/******************************************************************************
 GetClassName (virtual protected)

 ******************************************************************************/

JString
RadioGroup::GetClassName()
	const
{
	return "JXRadioGroup";
}

/******************************************************************************
 PrintConfiguration (virtual protected)

 ******************************************************************************/

void
RadioGroup::PrintConfiguration
	(
	std::ostream&	output,
	const JString&	indent,
	const JString&	varName,
	JStringManager*	stringdb
	)
	const
{
	if (GetBorderWidth() == 0)
	{
		indent.Print(output);
		varName.Print(output);
		output << "->SetBorderWidth(0);" << std::endl;
	}
	else
	{
		ContainerWidget::PrintConfiguration(output, indent, varName, stringdb);
	}
}

/******************************************************************************
 AddPanels (virtual protected)

 ******************************************************************************/

void
RadioGroup::AddPanels
	(
	WidgetParametersDialog* dlog
	)
{
	itsPanel = jnew RadioGroupPanel(dlog, GetBorderWidth() == 0);
}

/******************************************************************************
 SavePanelData (virtual protected)

 ******************************************************************************/

void
RadioGroup::SavePanelData()
{
	bool hideBorder;
	itsPanel->GetValues(&hideBorder);

	SetBorderWidth(hideBorder ? 0 : 2);

	itsPanel = nullptr;
}
