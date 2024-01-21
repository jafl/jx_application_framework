/******************************************************************************
 ToolBar.cpp

	BASE CLASS = BaseWidget

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
	BaseWidget(false, layout, hSizing, vSizing, x,y, w,h)
{
	ToolBarX(layout);
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
	BaseWidget(false, layout, hSizing, vSizing, x,y, w,h),
	itsPrefsMgr(prefsMgr),
	itsPrefID(prefID),
	itsMenuBar(menuBar)
{
	ToolBarX(layout);
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
	BaseWidget(input, vers, layout, hSizing, vSizing, x,y, w,h)
{
	ToolBarX(layout);

	input >> itsPrefsMgr >> itsPrefID >> itsMenuBar;
}

// private

void
ToolBar::ToolBarX
	(
	LayoutContainer* layout
	)
{
	itsLayout = jnew LayoutContainer(layout, this, this, kHElastic, kVElastic, 0,0, 100,100);
	itsLayout->FitToEnclosure();
	itsLayout->Move(0, kButtonBarHeight);
	itsLayout->AdjustSize(0, -kButtonBarHeight);
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

	BaseWidget::StreamOut(output);

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
	JString s = BaseWidget::ToString();

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
 GetLayoutContainer (virtual)

	Some widgets can contain other widgets.

 ******************************************************************************/

bool
ToolBar::GetLayoutContainer
	(
	LayoutContainer** layout
	)
	const
{
	*layout = itsLayout;
	return true;
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
 DrawBorder (virtual protected)

 ******************************************************************************/

void
ToolBar::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
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
ToolBar::GetEnclosureName()
	const
{
	bool b;
	return GetVarName(&b) + "->GetWidgetEnclosure()";
}

/******************************************************************************
 PrepareToGenerateCode (virtual)

 ******************************************************************************/

void
ToolBar::PrepareToGenerateCode()
	const
{
	itsLayout->Move(0, -kButtonBarHeight);
	itsLayout->AdjustSize(0, kButtonBarHeight);
}

/******************************************************************************
 GenerateCodeFinished (virtual)

 ******************************************************************************/

void
ToolBar::GenerateCodeFinished()
	const
{
	itsLayout->Move(0, kButtonBarHeight);
	itsLayout->AdjustSize(0, -kButtonBarHeight);
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
