/******************************************************************************
 Partition.cpp

	BASE CLASS = MultiContainerWidget

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "Partition.h"
#include "PartitionPanel.h"
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
	if (itsType == kHorizType)
	{
		JArray<JCoordinate> widths;
		widths.AppendItem(w/2);
		widths.AppendItem(0);

		JArray<JCoordinate> minWidths;
		minWidths.AppendItem(10);
		minWidths.AppendItem(10);

		itsPartition = jnew JXHorizPartition(widths, 2, minWidths, this, kHElastic, kVElastic, 0,0, w,h);
	}
	else
	{
		JArray<JCoordinate> heights;
		heights.AppendItem(h/2);
		heights.AppendItem(0);

		JArray<JCoordinate> minHeights;
		minHeights.AppendItem(10);
		minHeights.AppendItem(10);

		itsPartition = jnew JXVertPartition(heights, 2, minHeights, this, kHElastic, kVElastic, 0,0, w,h);
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
	return GetVarName(&b1, &b2) + "->GetCompartment(" + JString((JUInt64) i) + ")";
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

	while (itsPartition->GetCompartmentCount() < minSizes.GetItemCount())
	{
		const JIndex i = itsPartition->GetCompartmentCount()+1;
		const JSize v  = minSizes.GetItem(i);

		JXContainer* compartment;
		itsPartition->AppendCompartment(v, v, &compartment);
		InsertLayoutContainer(i, compartment);
	}

	while (itsPartition->GetCompartmentCount() > minSizes.GetItemCount())
	{
		itsPartition->DeleteCompartment(itsPartition->GetCompartmentCount());
	}

	itsPartition->SetElasticIndex(elasticIndex);
}
