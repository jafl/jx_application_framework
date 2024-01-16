/******************************************************************************
 StaticText.cpp

	BASE CLASS = CoreWidget

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "StaticText.h"
#include "StaticTextPanel.h"
#include "LayoutContainer.h"
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

StaticText::StaticText
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
	CoreWidget(false, layout, hSizing, vSizing, x,y, w,h),
	itsIsLabelFlag(true),
	itsCenterHorizFlag(false),
	itsWordWrapFlag(false),
	itsSelectableFlag(false),
	itsStyleableFlag(false)
{
	StaticTextX(JGetString("DefaultText::StaticText"), x,y,w,h);
}

StaticText::StaticText
	(
	const JString&		text,
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
	itsIsLabelFlag(true),
	itsCenterHorizFlag(false),
	itsWordWrapFlag(false),
	itsSelectableFlag(false),
	itsStyleableFlag(false)
{
	StaticTextX(text, x,y,w,h);
}

StaticText::StaticText
	(
	std::istream&		input,
	LayoutContainer*	layout,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	CoreWidget(input, layout, hSizing, vSizing, x,y, w,h)
{
	JString text;
	input >> text >> itsIsLabelFlag >> itsCenterHorizFlag;
	input >> itsWordWrapFlag >> itsSelectableFlag >> itsStyleableFlag;

	StaticTextX(text, x,y,w,h);
}

// private

void
StaticText::StaticTextX
	(
	const JString&		text,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	itsText = jnew JXStaticText(text, itsWordWrapFlag, false, false, nullptr,
								this, kHElastic, kVElastic, x,y,w,h);
	itsText->SetBorderWidth(0);
	SetWidget(itsText);

	if (itsIsLabelFlag)
	{
		itsText->SetToLabel(itsCenterHorizFlag);
	}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

StaticText::~StaticText()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
StaticText::StreamOut
	(
	std::ostream& output
	)
	const
{
	output << JString("StaticText") << std::endl;

	CoreWidget::StreamOut(output);

	output << itsText->GetText()->GetText() << std::endl;
	output << itsIsLabelFlag << std::endl;
	output << itsCenterHorizFlag << std::endl;
	output << itsWordWrapFlag << std::endl;
	output << itsSelectableFlag << std::endl;
	output << itsStyleableFlag << std::endl;
}

/******************************************************************************
 ToString (virtual)

 ******************************************************************************/

JString
StaticText::ToString()
	const
{
	JString s = CoreWidget::ToString();

	if (itsIsLabelFlag)
	{
		s += JString::newline;
		s += JGetString("IsLabel::StaticText");

		if (itsCenterHorizFlag)
		{
			s += JGetString("Centered::StaticText");
		}
	}

	if (itsWordWrapFlag)
	{
		s += JString::newline;
		s += JGetString("WordWrap::StaticText");
	}

	if (itsSelectableFlag)
	{
		s += JString::newline;
		s += JGetString("Selectable::InputField");
	}

	if (itsStyleableFlag)
	{
		s += JString::newline;
		s += JGetString("Styleable::InputField");
	}

	return s;
}

/******************************************************************************
 GetClassName (virtual protected)

 ******************************************************************************/

JString
StaticText::GetClassName()
	const
{
	return "JXStaticText";
}

/******************************************************************************
 PrintCtorArgsWithComma (virtual protected)

 ******************************************************************************/

void
StaticText::PrintCtorArgsWithComma
	(
	std::ostream&	output,
	const JString&	varName,
	JStringManager* stringdb
	)
	const
{
	const JString id = varName + GetParentContainer()->GetStringNamespace();
	stringdb->SetItem(id, itsText->GetText()->GetText(), JPtrArrayT::kDelete);

	output << "JGetString(" << id << "),";

	if (itsWordWrapFlag || itsSelectableFlag || itsStyleableFlag)
	{
		output << itsWordWrapFlag << ", ";
		output << itsSelectableFlag << ',';
		output << itsStyleableFlag << ',';
		output << "nullptr, ";		// JXScrollbarSet
	}
}

/******************************************************************************
 PrintConfiguration (virtual protected)

 ******************************************************************************/

void
StaticText::PrintConfiguration
	(
	std::ostream&	output,
	const JString&	indent,
	const JString&	varName,
	JStringManager*	stringdb
	)
	const
{
	if (itsIsLabelFlag)
	{
		indent.Print(output);
		varName.Print(output);
		output << "->SetToLabel(" << itsCenterHorizFlag << ");" << std::endl;
	}
	else
	{
		CoreWidget::PrintConfiguration(output, indent, varName, stringdb);
	}
}

/******************************************************************************
 AddPanels (virtual protected)

 ******************************************************************************/

void
StaticText::AddPanels
	(
	WidgetParametersDialog* dlog
	)
{
	itsPanel =
		jnew StaticTextPanel(dlog, itsText->GetText()->GetText(),
			itsIsLabelFlag, itsCenterHorizFlag,
			itsWordWrapFlag, itsSelectableFlag, itsStyleableFlag);
}

/******************************************************************************
 SavePanelData (virtual protected)

 ******************************************************************************/

void
StaticText::SavePanelData()
{
	const bool label  = itsIsLabelFlag,
			   center = itsCenterHorizFlag,
			   wrap   = itsWordWrapFlag;

	JString text;
	itsPanel->GetValues(&text, &itsIsLabelFlag, &itsCenterHorizFlag,
						&itsWordWrapFlag, &itsSelectableFlag, &itsStyleableFlag);
	itsPanel = nullptr;

	if (itsIsLabelFlag != label || itsCenterHorizFlag != center)
	{
		jdelete itsText;

		const JRect r = GetFrame();
		StaticTextX(text, r.left, r.top, r.width(), r.height());
	}
	else if (itsWordWrapFlag != wrap)
	{
		itsText->SetBreakCROnly(!itsWordWrapFlag);
		itsText->GetText()->SetText(text);
	}
	else
	{
		itsText->GetText()->SetText(text);
	}
}
