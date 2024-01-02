/******************************************************************************
 LayoutSelection.cpp

	text/x-jx-layout-data & text/x-jx-layout-meta with DELETE support for DND.

	BASE CLASS = JXSelectionData

	Copyright © 2023 by John Lindal.

 ******************************************************************************/

#include "LayoutSelection.h"
#include "LayoutDocument.h"
#include "LayoutContainer.h"
#include "BaseWidget.h"
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jcore/JTableSelection.h>
#include <sstream>
#include <jx-af/jcore/jAssert.h>

static const JUtf8Byte* kDataXAtomName = "text/x-jx-layout-data";
static const JUtf8Byte* kMetaXAtomName = "text/x-jx-layout-meta";

/******************************************************************************
 Constructor

 ******************************************************************************/

LayoutSelection::LayoutSelection
	(
	LayoutContainer*	layout,
	const JPoint&		ptG
	)
	:
	JXSelectionData(layout->GetDisplay()),
	itsLayout(layout)
{
	assert( itsLayout != nullptr );

	ClearWhenGoingAway(itsLayout, &itsLayout);

	// data

	JPtrArray<BaseWidget> widgetList(JPtrArrayT::kForgetAll);
	itsLayout->GetSelectedWidgets(&widgetList);
	assert( !widgetList.IsEmpty() );

	JPtrArray<JXWidget> widgetIndexList(JPtrArrayT::kForgetAll);
	JRect bounds;

	std::ostringstream data;
	for (auto* widget : widgetList)
	{
		LayoutDocument::WriteWidget(data, widget, &widgetIndexList);

		widget->ForEach([&data, &widgetIndexList](const JXContainer* obj)
		{
			LayoutDocument::WriteWidget(data, obj, &widgetIndexList);
		},
		true);

		const JRect r = widget->GetFrame();
		bounds        = bounds.IsEmpty() ? r : JCovering(bounds, r);
	}
	data << false;
	itsData = data.str();

	// meta

	const JPoint offset = bounds.topLeft();
	bounds.Shift(-offset);

	std::ostringstream meta;
	meta << offset;
	meta << ' ' << bounds;
	meta << ' ' << (layout->GlobalToLocal(ptG) - offset);
	meta << ' ' << widgetList.GetItemCount();
	for (auto* widget : widgetList)
	{
		JRect r = widget->GetFrame();
		r.Shift(-offset);
		meta << ' ' << r;
	}
	itsMetaData = meta.str();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LayoutSelection::~LayoutSelection()
{
}

/******************************************************************************
 AddTypes (virtual protected)

 ******************************************************************************/

void
LayoutSelection::AddTypes
	(
	const Atom selectionName
	)
{
	itsDataXAtom = AddType(kDataXAtomName);
	itsMetaXAtom = AddType(kMetaXAtomName);
}

/******************************************************************************
 ConvertData (virtual protected)

 ******************************************************************************/

bool
LayoutSelection::ConvertData
	(
	const Atom		requestType,
	Atom*			returnType,
	unsigned char**	data,
	JSize*			dataLength,
	JSize*			bitsPerBlock
	)
	const
{
	*bitsPerBlock = 8;

	JXSelectionManager* selMgr = GetSelectionManager();

	if (requestType == itsDataXAtom || requestType == itsMetaXAtom)
	{
		const JString& s = requestType == itsMetaXAtom ? itsMetaData : itsData;

		*returnType = requestType;
		*dataLength = s.GetByteCount();
		*data       = jnew_allow_null unsigned char[ *dataLength ];
		if (*data != nullptr)
		{
			memcpy(*data, s.GetRawBytes(), *dataLength);
			return true;
		}
	}

	else if (requestType == selMgr->GetDeleteSelectionXAtom() &&
			 itsLayout != nullptr && IsCurrent())
	{
		itsLayout->RemoveSelectedWidgets();

		*data       = jnew unsigned char[1];
		*dataLength = 0;
		*returnType = selMgr->GetNULLXAtom();
		return true;
	}

	*data       = nullptr;
	*dataLength = 0;
	*returnType = None;
	return false;
}

/******************************************************************************
 ReadMetaData (static)

 ******************************************************************************/

void
LayoutSelection::ReadMetaData
	(
	std::istream&	input,
	JPoint*			boundsOffset,
	JRect*			bounds,
	JPoint*			mouseOffset,
	JArray<JRect>*	rectList
	)
{
	input >> *boundsOffset >> *bounds >> *mouseOffset;

	JSize count;
	input >> count;

	JRect r;
	for (JIndex i=1; i<=count; i++)
	{
		input >> r;
		rectList->AppendItem(r);
	}
}

/******************************************************************************
 Get*XAtomName (static)

 ******************************************************************************/

const JUtf8Byte*
LayoutSelection::GetDataXAtomName()
{
	return kDataXAtomName;
}

const JUtf8Byte*
LayoutSelection::GetMetaXAtomName()
{
	return kMetaXAtomName;
}
