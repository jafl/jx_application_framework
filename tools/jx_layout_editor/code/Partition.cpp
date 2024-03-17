/******************************************************************************
 Partition.cpp

	BASE CLASS = MultiContainerWidget

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "Partition.h"
#include "PartitionPanel.h"
#include "LayoutContainer.h"
#include <jx-af/jx/JXHorizPartition.h>
#include <jx-af/jx/JXVertPartition.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

Partition::Partition
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
	MultiContainerWidget(false, layout, hSizing, vSizing, x,y, w,h),
	itsType(type)
{
	JArray<JCoordinate> sizes;
	sizes.AppendItem((itsType == kHorizType ? w : h)/2);
	sizes.AppendItem(0);

	JArray<JCoordinate> minSizes;
	minSizes.AppendItem(10);
	minSizes.AppendItem(10);

	if (itsType == kHorizType)
	{
		itsPartition = jnew JXHorizPartition(sizes, 2, minSizes, this, kHElastic, kVElastic, 0,0, w,h);
	}
	else
	{
		itsPartition = jnew JXVertPartition(sizes, 2, minSizes, this, kHElastic, kVElastic, 0,0, w,h);
	}

	InsertLayoutContainer(1, itsPartition->GetCompartment(1));
	InsertLayoutContainer(2, itsPartition->GetCompartment(2));
}

Partition::Partition
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
	int type;
	input >> type;

	itsType = (Type) type;

	JSize count;
	input >> count;

	JArray<JCoordinate> sizes, minSizes;
	for (JIndex i=1; i<=count; i++)
	{
		JCoordinate size, min;
		input >> size >> min;
		sizes.AppendItem(size);
		minSizes.AppendItem(min);
	}

	JIndex elasticIndex;
	input >> elasticIndex;

	if (itsType == kHorizType)
	{
		itsPartition = jnew JXHorizPartition(sizes, elasticIndex, minSizes, this, kHElastic, kVElastic, 0,0, w,h);
	}
	else
	{
		itsPartition = jnew JXVertPartition(sizes, elasticIndex, minSizes, this, kHElastic, kVElastic, 0,0, w,h);
	}

	for (JIndex i=1; i<=count; i++)
	{
		InsertLayoutContainer(i, itsPartition->GetCompartment(i));
	}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

Partition::~Partition()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
Partition::StreamOut
	(
	std::ostream& output
	)
	const
{
	output << JString("Partition") << std::endl;

	MultiContainerWidget::StreamOut(output);

	output << (int) itsType << std::endl;

	const JSize count = itsPartition->GetCompartmentCount();
	output << count << std::endl;

	for (JIndex i=1; i<=count; i++)
	{
		output << itsPartition->GetCompartmentSize(i) << std::endl;
		output << itsPartition->GetMinCompartmentSize(i) << std::endl;
	}

	JIndex elasticIndex;
	itsPartition->GetElasticIndex(&elasticIndex);
	output << elasticIndex << std::endl;
}

/******************************************************************************
 StealMouse (virtual protected)

 ******************************************************************************/

bool
Partition::StealMouse
	(
	const int			eventType,
	const JPoint&		ptG,
	const JXMouseButton	button,
	const unsigned int	state
	)
{
	itsPartition->SetHint(ToString());

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
Partition::GetClassName()
	const
{
	return itsType == kHorizType ? "JXHorizPartition" : "JXVertPartition";
}

/******************************************************************************
 GetEnclosureName (virtual protected)

 ******************************************************************************/

JString
Partition::GetEnclosureName
	(
	const LayoutWidget* widget
	)
	const
{
	JIndex i;
	const bool ok = GetLayoutContainerIndex(widget, &i);
	assert( ok );

	bool b1, b2;
	return GetVarName(&b1, &b2) + "->GetCompartment(" + JString(i) + ")";
}

/******************************************************************************
 PrepareToGenerateCode (virtual)

 ******************************************************************************/

void
Partition::PrepareToGenerateCode
	(
	std::ostream&	output,
	const JString&	indent,
	JStringManager*	stringdb
	)
	const
{
	bool b1, b2;
	const JString varName= GetVarName(&b1, &b2);

	indent.Print(output);
	output << "JArray<JCoordinate> ";
	varName.Print(output);
	output << "_sizes, ";
	varName.Print(output);
	output << "_minSizes;" << std::endl;

	const JSize count = itsPartition->GetCompartmentCount();
	for (JIndex i=1; i<=count; i++)
	{
		indent.Print(output);
		varName.Print(output);
		output << "_sizes.AppendItem(" << itsPartition->GetCompartmentSize(i) << ");" << std::endl;

		indent.Print(output);
		varName.Print(output);
		output << "_minSizes.AppendItem(" << itsPartition->GetMinCompartmentSize(i) << ");" << std::endl;
	}

	output << std::endl;
}

/******************************************************************************
 PrintCtorArgsWithComma (virtual protected)

 ******************************************************************************/

void
Partition::PrintCtorArgsWithComma
	(
	std::ostream&	output,
	const JString&	varName,
	JStringManager* stringdb
	)
	const
{
	JIndex elasticIndex;
	itsPartition->GetElasticIndex(&elasticIndex);

	varName.Print(output);
	output << "_sizes, " << elasticIndex << ", ";
	varName.Print(output);
	output << "_minSizes, ";
}

/******************************************************************************
 AddPanels (virtual protected)

 ******************************************************************************/

void
Partition::AddPanels
	(
	WidgetParametersDialog* dlog
	)
{
	JArray<JCoordinate> minSizes;
	const JSize count = itsPartition->GetCompartmentCount();
	for (JIndex i=1; i<=count; i++)
	{
		minSizes.AppendItem(itsPartition->GetMinCompartmentSize(i));
	}

	JIndex elasticIndex;
	itsPartition->GetElasticIndex(&elasticIndex);

	itsPanel = jnew PartitionPanel(dlog, minSizes, elasticIndex);
}

/******************************************************************************
 SavePanelData (virtual protected)

 ******************************************************************************/

void
Partition::SavePanelData()
{
	JArray<JCoordinate> minSizes;
	JIndex elasticIndex;
	itsPanel->GetValues(&minSizes, &elasticIndex);
	itsPanel = nullptr;

	JSize total = 0;
	for (const auto v : minSizes)
	{
		total += v + JPartition::kDragRegionSize;
	}

	if (itsType == kHorizType && GetFrameWidth() < total)
	{
		AdjustSize(total - GetFrameWidth(), 0);
	}
	else if (GetFrameHeight() < total)
	{
		AdjustSize(0, total - GetFrameHeight());
	}

	while (itsPartition->GetCompartmentCount() < minSizes.GetItemCount())
	{
		const JIndex i = itsPartition->GetCompartmentCount()+1;
		const JSize v  = minSizes.GetItem(i);

		JXContainer* compartment;
		const bool ok = itsPartition->AppendCompartment(v, v, &compartment);
		assert( ok );
		InsertLayoutContainer(i, compartment);
	}

	while (itsPartition->GetCompartmentCount() > minSizes.GetItemCount())
	{
		itsPartition->DeleteCompartment(itsPartition->GetCompartmentCount());
	}

	itsPartition->SetElasticIndex(elasticIndex);

	const JSize count = minSizes.GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		const auto v = minSizes.GetItem(i);
		if (itsPartition->GetCompartmentSize(i) < v)
		{
			const bool ok = itsPartition->SetCompartmentSize(i, v);
			assert( ok );
		}
		itsPartition->SetMinCompartmentSize(i, v);
	}
}
