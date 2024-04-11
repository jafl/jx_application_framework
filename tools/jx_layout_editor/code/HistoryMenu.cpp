/******************************************************************************
 HistoryMenu.cpp

	BASE CLASS = CoreWidget

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "HistoryMenu.h"
#include "HistoryMenuPanel.h"
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

static const JUtf8Byte* kMenuStr = "enable menu";

/******************************************************************************
 Constructor

 ******************************************************************************/

HistoryMenu::HistoryMenu
	(
	const Type			type,
	LayoutContainer*	layout,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	CoreWidget(false, layout, hSizing, vSizing, x,y, w,h),
	itsType(type)
{
	HistoryMenuX(x,y,w,h);
}

HistoryMenu::HistoryMenu
	(
	const Type			type,
	const JString&		historyLength,
	LayoutContainer*	layout,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	CoreWidget(false, layout, hSizing, vSizing, x,y, w,h),
	itsType(type),
	itsHistoryLength(historyLength)
{
	HistoryMenuX(x,y,w,h);
}

HistoryMenu::HistoryMenu
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
	CoreWidget(input, vers, layout, hSizing, vSizing, x,y, w,h)
{
	int type;
	input >> type >> itsHistoryLength;

	itsType = (Type) type;

	HistoryMenuX(x,y,w,h);
}

// private

void
HistoryMenu::HistoryMenuX
	(
	const JCoordinate x,
	const JCoordinate y,
	const JCoordinate w,
	const JCoordinate h
	)
{
	itsMenu = jnew JXTextMenu(JString::empty, this, kHElastic, kVElastic, x,y,w,h);
	SetWidget(itsMenu);
	itsMenu->SetMenuItems(kMenuStr);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

HistoryMenu::~HistoryMenu()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
HistoryMenu::StreamOut
	(
	std::ostream& output
	)
	const
{
	output << JString("HistoryMenu") << std::endl;

	CoreWidget::StreamOut(output);

	output << (int) itsType << std::endl;
	output << itsHistoryLength << std::endl;
}

/******************************************************************************
 ToString (virtual)

 ******************************************************************************/

JString
HistoryMenu::ToString()
	const
{
	JString s = CoreWidget::ToString();

	s += JString::newline;
	s += JGetString("HistoryLength::HistoryMenu");
	s += itsHistoryLength;

	return s;
}

/******************************************************************************
 GetClassName (virtual protected)

 ******************************************************************************/

JString
HistoryMenu::GetClassName()
	const
{
	return (itsType == kFileType ? "JXFileHistoryMenu" :
			itsType == kPathType ? "JXPathHistoryMenu" :
			"JXStringHistoryMenu");
}

/******************************************************************************
 PrintCtorArgsWithComma (virtual protected)

 ******************************************************************************/

void
HistoryMenu::PrintCtorArgsWithComma
	(
	std::ostream&	output,
	const JString&	varName,
	JStringManager* stringdb
	)
	const
{
	itsHistoryLength.Print(output);
	output << ", ";
}

/******************************************************************************
 AddPanels (virtual protected)

 ******************************************************************************/

void
HistoryMenu::AddPanels
	(
	WidgetParametersDialog* dlog
	)
{
	itsPanel =
		jnew HistoryMenuPanel(dlog, itsHistoryLength);
}

/******************************************************************************
 SavePanelData (virtual protected)

 ******************************************************************************/

void
HistoryMenu::SavePanelData()
{
	itsPanel->GetValues(&itsHistoryLength);
	itsPanel = nullptr;
}
