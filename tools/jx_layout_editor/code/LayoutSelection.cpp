/******************************************************************************
 LayoutSelection.cpp

	text/x-jx-layout with DELETE support for DND.

	BASE CLASS = JXSelectionData

	Copyright © 2023 by John Lindal.

 ******************************************************************************/

#include "LayoutSelection.h"
#include "LayoutContainer.h"
#include "BaseWidget.h"
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jcore/JTableSelection.h>
#include <jx-af/jcore/jAssert.h>

static const JUtf8Byte* kXAtomName = "text/x-jx-layout";

/******************************************************************************
 Constructor

 ******************************************************************************/

LayoutSelection::LayoutSelection
	(
	LayoutContainer*	layout,
	const JUtf8Byte*	id
	)
	:
	JXSelectionData(layout, id),
	itsLayout(layout)
{
	assert( itsLayout != nullptr );

	ClearWhenGoingAway(itsLayout, &itsLayout);
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
	itsXAtom = AddType(kXAtomName);
}

/******************************************************************************
 SetData

 ******************************************************************************/

void
LayoutSelection::SetData
	(
	const JPtrArray<BaseWidget>& list
	)
{
	std::ostringstream output;

	for (auto* widget : list)
	{
		output << true << ' ';
		widget->StreamOut(output);
		output << ' ';
	}

	output << false;

	itsData = output.str();
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

	if (requestType == itsXAtom)
	{
		*returnType = itsXAtom;
		*dataLength = itsData.GetByteCount();
		*data       = jnew unsigned char[ *dataLength ];
		if (*data != nullptr)
		{
			memcpy(*data, itsData.GetRawBytes(), *dataLength);
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
 GetXAtomName (static)

 ******************************************************************************/

const JUtf8Byte*
LayoutSelection::GetXAtomName()
{
	return kXAtomName;
}
