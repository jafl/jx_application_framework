/******************************************************************************
 ToolBar.cpp

	BASE CLASS = ContainerWidget

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "ToolBar.h"
#include "LayoutContainer.h"
#include "ToolBarPanel.h"
#include <jx-af/jx/JXWindowPainter.h>
#include <jx-af/jx/jXPainterUtil.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

const JCoordinate kButtonBarHeight = 30;

/******************************************************************************
 Constructor

 ******************************************************************************/

ToolBar::ToolBar
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
	ToolBarX();
}

ToolBar::ToolBar
	(
	const JString&		prefsMgr,
	const JString&		prefID,
	const JString&		menuBar,
	LayoutContainer*	layout,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	ContainerWidget(false, layout, hSizing, vSizing, x,y, w,h),
	itsPrefsMgr(prefsMgr),
	itsPrefID(prefID),
	itsMenuBar(menuBar)
{
	ToolBarX();
}

ToolBar::ToolBar
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
	ToolBarX();

	input >> itsPrefsMgr >> itsPrefID >> itsMenuBar;
}

// private

void
ToolBar::ToolBarX()
{
	LayoutContainer* layout = GetLayoutContainer();
	layout->Move(0, kButtonBarHeight);
	layout->AdjustSize(0, -kButtonBarHeight);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

ToolBar::~ToolBar()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
ToolBar::StreamOut
	(
	std::ostream& output
	)
	const
{
	output << JString("ToolBar") << std::endl;

	ContainerWidget::StreamOut(output);

	output << itsPrefsMgr << std::endl;
	output << itsPrefID << std::endl;
	output << itsMenuBar << std::endl;
}

/******************************************************************************
 ToString (virtual)

 ******************************************************************************/

JString
ToolBar::ToString()
	const
{
	JString s = ContainerWidget::ToString();

	s += JString::newline;
	s += JGetString("PrefsManagerLabel::ToolBar");
	s += itsPrefsMgr;

	s += JString::newline;
	s += JGetString("PrefIDLabel::ToolBar");
	s += itsPrefID;

	s += JString::newline;
	s += JGetString("MenuBarLabel::ToolBar");
	s += itsMenuBar;

	return s;
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
ToolBar::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	JRect r  = GetAperture();
	r.bottom = r.top + kButtonBarHeight;
	p.String(r, "JXToolBar", JPainter::HAlign::kCenter, JPainter::VAlign::kCenter);
}

/******************************************************************************
 GetClassName (virtual protected)

 ******************************************************************************/

JString
ToolBar::GetClassName()
	const
{
	return "JXToolBar";
}

/******************************************************************************
 GetEnclosureName (virtual protected)

 ******************************************************************************/

JString
ToolBar::GetEnclosureName
	(
	const LayoutWidget* widget
	)
	const
{
	bool b1, b2;
	return GetVarName(&b1, &b2) + "->GetWidgetEnclosure()";
}

/******************************************************************************
 PrepareToGenerateCode (virtual)

 ******************************************************************************/

void
ToolBar::PrepareToGenerateCode
	(
	std::ostream&	output,
	const JString&	indent,
	JStringManager*	stringdb
	)
	const
{
	LayoutContainer* layout = GetLayoutContainer();
	layout->Move(0, -kButtonBarHeight);
	layout->AdjustSize(0, kButtonBarHeight);
}

/******************************************************************************
 GenerateCodeFinished (virtual)

 ******************************************************************************/

void
ToolBar::GenerateCodeFinished
	(
	std::ostream&	output,
	const JString&	indent,
	JStringManager*	stringdb
	)
	const
{
	LayoutContainer* layout = GetLayoutContainer();
	layout->Move(0, kButtonBarHeight);
	layout->AdjustSize(0, -kButtonBarHeight);
}

/******************************************************************************
 PrintCtorArgsWithComma (virtual protected)

 ******************************************************************************/

void
ToolBar::PrintCtorArgsWithComma
	(
	std::ostream&	output,
	const JString&	varName,
	JStringManager* stringdb
	)
	const
{
	itsPrefsMgr.Print(output);
	output << ", ";
	itsPrefID.Print(output);
	output << ", ";
	itsMenuBar.Print(output);
	output << ", ";
}

/******************************************************************************
 AddPanels (virtual protected)

 ******************************************************************************/

void
ToolBar::AddPanels
	(
	WidgetParametersDialog* dlog
	)
{
	itsPanel = jnew ToolBarPanel(dlog, itsPrefsMgr, itsPrefID, itsMenuBar);
}

/******************************************************************************
 SavePanelData (virtual protected)

 ******************************************************************************/

void
ToolBar::SavePanelData()
{
	itsPanel->GetValues(&itsPrefsMgr, &itsPrefID, &itsMenuBar);
	itsPanel = nullptr;
}
