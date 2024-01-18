/******************************************************************************
 CustomWidget.cpp

	BASE CLASS = BaseWidget

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "CustomWidget.h"
#include "WidgetParametersDialog.h"
#include "CustomWidgetPanel.h"
#include <jx-af/jx/JXWindowPainter.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CustomWidget::CustomWidget
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
	BaseWidget(false, layout, hSizing, vSizing, x,y, w,h),
	itsCreateFlag(false)
{
	CustomWidgetX();
}

CustomWidget::CustomWidget
	(
	const JString&		className,
	const JString&		args,
	const bool			create,
	LayoutContainer*	layout,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	BaseWidget(false, layout, hSizing, vSizing, x,y, w,h),
	itsClassName(className),
	itsCtorArgs(args),
	itsCreateFlag(create)
{
	CustomWidgetX();
}

CustomWidget::CustomWidget
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
	BaseWidget(input, vers, layout, hSizing, vSizing, x,y, w,h)
{
	input >> itsClassName >> itsCtorArgs >> itsCreateFlag;

	CustomWidgetX();
}

// private

void
CustomWidget::CustomWidgetX()
{
	SetBorderWidth(1);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CustomWidget::~CustomWidget()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
CustomWidget::StreamOut
	(
	std::ostream& output
	)
	const
{
	output << JString("CustomWidget") << std::endl;

	BaseWidget::StreamOut(output);

	output << itsClassName << std::endl;
	output << itsCtorArgs << std::endl;
	output << itsCreateFlag << std::endl;
}

/******************************************************************************
 ToString (virtual)

 ******************************************************************************/

JString
CustomWidget::ToString()
	const
{
	JString s = BaseWidget::ToString();

	if (!itsCtorArgs.IsEmpty() && !itsCreateFlag)
	{
		s += JString::newline;
		s += "jnew ";
		s += itsClassName;
		s += "(";
		s += itsCtorArgs;
		s += " ...);";
	}
	else if (!itsCtorArgs.IsEmpty())
	{
		s += JString::newline;
		s += itsClassName;
		s += "::Create(";
		s += itsCtorArgs;
		s += " ...);";
	}

	return s;
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
CustomWidget::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	p.String(GetFrameLocal(), itsClassName, JPainter::HAlign::kCenter, JPainter::VAlign::kCenter);
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
CustomWidget::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
	p.Rect(frame);
}

/******************************************************************************
 GetClassName (virtual protected)

 ******************************************************************************/

JString
CustomWidget::GetClassName()
	const
{
	return itsClassName;
}

/******************************************************************************
 GetCtor (virtual protected)

 ******************************************************************************/

JString
CustomWidget::GetCtor()
	const
{
	return itsCreateFlag ? (itsClassName + "::Create") : ("jnew " + itsClassName);
}

/******************************************************************************
 PrintCtorArgsWithComma (virtual protected)

 ******************************************************************************/

void
CustomWidget::PrintCtorArgsWithComma
	(
	std::ostream&	output,
	const JString&	varName,
	JStringManager* stringdb
	)
	const
{
	if (!itsCtorArgs.IsEmpty())
	{
		itsCtorArgs.Print(output);
		if (itsCtorArgs.GetLastCharacter() != ',')
		{
			output << ", ";
		}
	}
}

/******************************************************************************
 AddPanels (virtual protected)

 ******************************************************************************/

void
CustomWidget::AddPanels
	(
	WidgetParametersDialog* dlog
	)
{
	itsPanel =
		jnew CustomWidgetPanel(dlog, itsClassName, itsCtorArgs, itsCreateFlag,
			WantsInput());
}

/******************************************************************************
 SavePanelData (virtual protected)

 ******************************************************************************/

void
CustomWidget::SavePanelData()
{
	bool wantsInput;
	itsPanel->GetValues(&itsClassName, &itsCtorArgs, &itsCreateFlag, &wantsInput);
	SetWantsInput(wantsInput);
	itsPanel = nullptr;
}
