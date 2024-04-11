/******************************************************************************
 Menu.cpp

	BASE CLASS = CoreWidget

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "Menu.h"
#include "MenuPanel.h"
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

static const JUtf8Byte* kMenuStr = "enable menu";

/******************************************************************************
 Constructor

 ******************************************************************************/

Menu::Menu
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
	itsType(type),
	itsImageColCount(type == kImageType ? 5 : 0)
{
	MenuX(JGetString("DefaultTitle::Menu"), x,y,w,h);
}

Menu::Menu
	(
	const Type			type,
	const JString&		title,
	const JSize			colCount,
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
	itsImageColCount(colCount)
{
	MenuX(title, x,y,w,h);
}

Menu::Menu
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
	JString title;
	int type, pos, dir;
	input >> type >> title >> pos >> dir >> itsImageColCount;

	itsType = (Type) type;

	MenuX(title, x,y,w,h);
	itsMenu->SetPopupArrowPosition((JXMenu::ArrowPosition) pos);
	itsMenu->SetPopupArrowDirection((JXMenu::ArrowDirection) dir);
}

// private

void
Menu::MenuX
	(
	const JString&		title,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	itsMenu = jnew JXTextMenu(title, this, kHElastic, kVElastic, x,y,w,h);
	SetWidget(itsMenu);
	itsMenu->SetMenuItems(kMenuStr);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

Menu::~Menu()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
Menu::StreamOut
	(
	std::ostream& output
	)
	const
{
	output << JString("Menu") << std::endl;

	CoreWidget::StreamOut(output);

	output << (int) itsType << std::endl;
	output << itsMenu->GetTitleText() << std::endl;
	output << (int) itsMenu->GetPopupArrowPosition() << std::endl;
	output << (int) itsMenu->GetPopupArrowDirection() << std::endl;
	output << itsImageColCount << std::endl;
}

/******************************************************************************
 ToString (virtual)

 ******************************************************************************/

JString
Menu::ToString()
	const
{
	JString s = CoreWidget::ToString();

	if (itsType == kImageType)
	{
		s += JString::newline;
		s += JGetString("ColumnCount::Menu");
		s += JString(itsImageColCount);
	}

	return s;
}

/******************************************************************************
 GetClassName (virtual protected)

 ******************************************************************************/

JString
Menu::GetClassName()
	const
{
	return itsType == kImageType ? "JXImageMenu" : "JXTextMenu";
}

/******************************************************************************
 PrintCtorArgsWithComma (virtual protected)

 ******************************************************************************/

void
Menu::PrintCtorArgsWithComma
	(
	std::ostream&	output,
	const JString&	varName,
	JStringManager* stringdb
	)
	const
{
	PrintStringForArg(itsMenu->GetTitleText(), varName, stringdb, output);
	output << ", ";

	if (itsType == kImageType)
	{
		output << itsImageColCount << ", ";
	}
}

/******************************************************************************
 PrintConfiguration (virtual protected)

 ******************************************************************************/

void
Menu::PrintConfiguration
	(
	std::ostream&	output,
	const JString&	indent,
	const JString&	varName,
	JStringManager*	stringdb
	)
	const
{
	bool used = false;

	if (itsMenu->GetPopupArrowPosition() == JXMenu::kArrowAtLeft)
	{
		indent.Print(output);
		varName.Print(output);
		output << "->SetPopupArrowPosition(JXMenu::kArrowAtLeft);" << std::endl;
		used = true;
	}

	if (itsMenu->GetPopupArrowDirection() == JXMenu::kArrowPointsUp)
	{
		indent.Print(output);
		varName.Print(output);
		output << "->SetPopupArrowDirection(JXMenu::kArrowPointsUp);" << std::endl;
		used = true;
	}

	if (!used)
	{
		CoreWidget::PrintConfiguration(output, indent, varName, stringdb);
	}
}

/******************************************************************************
 AddPanels (virtual protected)

 ******************************************************************************/

void
Menu::AddPanels
	(
	WidgetParametersDialog* dlog
	)
{
	itsPanel =
		jnew MenuPanel(dlog, itsType, itsMenu->GetTitleText(),
			itsMenu->GetPopupArrowPosition(),
			itsMenu->GetPopupArrowDirection(),
			itsImageColCount);
}

/******************************************************************************
 SavePanelData (virtual protected)

 ******************************************************************************/

void
Menu::SavePanelData()
{
	JString title;
	JXMenu::ArrowPosition pos;
	JXMenu::ArrowDirection dir;
	itsPanel->GetValues(&title, &pos, &dir, &itsImageColCount);
	itsPanel = nullptr;

	itsMenu->SetTitleText(title);
	itsMenu->SetPopupArrowPosition(pos);
	itsMenu->SetPopupArrowDirection(dir);
}
