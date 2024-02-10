/******************************************************************************
 TabGroup.cpp

	BASE CLASS = MultiContainerWidget

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "TabGroup.h"
#include "TabGroupPanel.h"
#include "LayoutContainer.h"
#include <jx-af/jx/JXTabGroup.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TabGroup::TabGroup
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
	MultiContainerWidget(false, layout, hSizing, vSizing, x,y, w,h)
{
	itsTabGroup = jnew JXTabGroup(this, kHElastic, kVElastic, 0,0, w,h);

	InsertLayoutContainer(1,
		itsTabGroup->AppendTab(JGetString("DefaultLabel::TabGroup")));

	itsTabGroup->ShowTab(1);
}

TabGroup::TabGroup
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
	MultiContainerWidget(input, vers, layout, hSizing, vSizing, x,y, w,h)
{
	itsTabGroup = jnew JXTabGroup(this, kHElastic, kVElastic, 0,0, w,h);

	int edge;
	input >> edge;
	itsTabGroup->SetTabEdge((JXTabGroup::Edge) edge);

	JSize count;
	input >> count;

	JString title;
	for (JIndex i=1; i<=count; i++)
	{
		input >> title;

		InsertLayoutContainer(i, itsTabGroup->AppendTab(title));
	}

	itsTabGroup->ShowTab(1);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TabGroup::~TabGroup()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
TabGroup::StreamOut
	(
	std::ostream& output
	)
	const
{
	output << JString("TabGroup") << std::endl;

	MultiContainerWidget::StreamOut(output);

	output << (int) itsTabGroup->GetTabEdge() << std::endl;

	const JSize count = itsTabGroup->GetTabCount();
	output << count << std::endl;

	for (JIndex i=1; i<=count; i++)
	{
		output << itsTabGroup->GetTabTitle(i) << std::endl;
	}
}

/******************************************************************************
 StealMouse (virtual protected)

 ******************************************************************************/

bool
TabGroup::StealMouse
	(
	const int			eventType,
	const JPoint&		ptG,
	const JXMouseButton	button,
	const unsigned int	state
	)
{
	itsTabGroup->SetHint(ToString());

	const JXKeyModifiers modifiers(GetDisplay(), state);
	if (!modifiers.control())
	{
		return MultiContainerWidget::StealMouse(eventType, ptG, button, state);
	}

	return !AnyOf([this, &ptG](const JXContainer* obj)
	{
		auto* layout = dynamic_cast<const LayoutContainer*>(obj);
		return (layout != nullptr && !OwnsLayoutContainer(layout) &&
				layout->GetFrameGlobal().Contains(ptG));
	},
	true);
}

/******************************************************************************
 GetClassName (virtual protected)

 ******************************************************************************/

JString
TabGroup::GetClassName()
	const
{
	return "JXTabGroup";
}

/******************************************************************************
 GetEnclosureName (virtual protected)

 ******************************************************************************/

JString
TabGroup::GetEnclosureName
	(
	const LayoutWidget* widget
	)
	const
{
	JIndex i;
	const bool ok = GetLayoutContainerIndex(widget, &i);
	assert( ok );

	bool b1, b2;
	return GetVarName(&b1, &b2) + "->GetTabEnclosure(" + JString((JUInt64) i) + ")";
}

/******************************************************************************
 PrintConfiguration (virtual protected)

 ******************************************************************************/

void
TabGroup::PrintConfiguration
	(
	std::ostream&	output,
	const JString&	indent,
	const JString&	varName,
	JStringManager*	stringdb
	)
	const
{
	const JSize count = itsTabGroup->GetTabCount();
	for (JIndex i=1; i<=count; i++)
	{
		indent.Print(output);
		varName.Print(output);
		output << "->AppendTab(";
		PrintStringForArg(itsTabGroup->GetTabTitle(i), varName + "::tab" + JString((JUInt64) i), stringdb, output);
		output << ");" << std::endl;
	}
}

/******************************************************************************
 AddPanels (virtual protected)

 ******************************************************************************/

void
TabGroup::AddPanels
	(
	WidgetParametersDialog* dlog
	)
{
	JPtrArray<JString> titles(JPtrArrayT::kDeleteAll);
	const JSize count = itsTabGroup->GetTabCount();
	for (JIndex i=1; i<=count; i++)
	{
		titles.Append(itsTabGroup->GetTabTitle(i));
	}

	itsPanel = jnew TabGroupPanel(dlog, titles, itsTabGroup->GetTabEdge());
}

/******************************************************************************
 SavePanelData (virtual protected)

 ******************************************************************************/

void
TabGroup::SavePanelData()
{
	JPtrArray<JString> titles(JPtrArrayT::kDeleteAll);
	JXTabGroup::Edge edge;
	itsPanel->GetValues(&titles, &edge);
	itsPanel = nullptr;

	while (itsTabGroup->GetTabCount() < titles.GetItemCount())
	{
		const JIndex i = itsTabGroup->GetTabCount()+1;

		InsertLayoutContainer(i,
			itsTabGroup->AppendTab(*titles.GetItem(i)));
	}

	while (itsTabGroup->GetTabCount() > titles.GetItemCount())
	{
		itsTabGroup->DeleteTab(itsTabGroup->GetTabCount());
	}

	itsTabGroup->SetTabEdge(edge);

	const JSize count = titles.GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		itsTabGroup->SetTabTitle(i, *titles.GetItem(i));
	}
}
