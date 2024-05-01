/******************************************************************************
 DNDData.cpp

	BASE CLASS = public JXSelectionData

	Copyright (C) 2000 by Glenn W. Bach.
	
 *****************************************************************************/

#include "DNDData.h"
#include <sstream>
#include <jx-af/jcore/jAssert.h>

// This is the string that will be used to define the X Atom for our data.
static const JUtf8Byte* kDragLinesXAtomName	= "DNDTutorialWidget";

/******************************************************************************
 Constructor

  We need to initialize all of our pointers to nullptr.

 *****************************************************************************/

DNDData::DNDData
	(
	JXWidget* 			widget, 
	const JUtf8Byte*	id
	)
	:
	JXSelectionData(widget, id),
	itsPoints(nullptr),
	itsBuffer(nullptr)
{
}

/******************************************************************************
 Destructor

  We delete our data and buffer.

 *****************************************************************************/

DNDData::~DNDData()
{
	jdelete itsPoints;
	jdelete itsBuffer;
}

/******************************************************************************
 SetData (public)

  This is called by the widget when the widget's GetSelectionData function
  is called during a drag-and-drop, or when the user performs a Copy.

 ******************************************************************************/

void
DNDData::SetData
	(
	const JArray<JPoint>& points
	)
{
	// create the data array if it doesn't already exist
	if (itsPoints == nullptr)
	{
		itsPoints = jnew JArray<JPoint>;
	}

	// if the data had been set previously, we want to remove all of
	// the old points.
	else
	{
		itsPoints->RemoveAll();
	}

	// we now loop through all of the points and add them to our list.
	const JSize count	= points.GetItemCount();
	for (JIndex i = 1; i <= count; i++)
	{
		itsPoints->AppendItem(points.GetItem(i));
	}

	// since the data has changed, the data buffer is no longer valid. It
	// is safe to delete it if it is still nullptr.
	jdelete itsBuffer;
	itsBuffer = nullptr;
}

/******************************************************************************
 AddTypes (virtual protected)

  We need to call AddType for each XAtom that we support.

 ******************************************************************************/

void
DNDData::AddTypes
	(
	const Atom selectionName
	)
{
	itsLinesXAtom	= AddType(kDragLinesXAtomName);
}

/******************************************************************************
 ConvertData (virtual protected)

  This is called when the user performs a Paste. We buffer the data, so that
  we don't need to recalculate it if the user performs more than one Paste.

 ******************************************************************************/

bool
DNDData::ConvertData
	(
	const Atom 		requestType, 
	Atom* 			returnType,
	unsigned char** data, 
	JSize* 			dataLength,
	JSize* 			bitsPerBlock
	)
	const
{
	*bitsPerBlock = 8;

	if (requestType == itsLinesXAtom && itsPoints != nullptr && !itsPoints->IsEmpty())
	{
		// This initializes and builds our data buffer.
		CreateBuffer();

		// The following code gives X the data in the form that it needs.
		*returnType = itsLinesXAtom;
		*dataLength = itsBuffer->GetByteCount();
		*data = jnew_allow_null unsigned char[ *dataLength ];
		if (*data != nullptr)
		{
			memcpy(*data, itsBuffer->GetRawBytes(), *dataLength);
			return true;
		}
	}

	// If we reached this point, than it was either not our data type, or
	// we had no data.
	*data       = nullptr;
	*dataLength = 0;
	*returnType = None;
	return false;
}

/******************************************************************************
 GetDNDAtomName (public static)

  The widget that uses this object will need to register the X atom with 
  its JXDisplay object, so we need to give the widget access to the string
  that represents it.  We cannot return the actual X atom because that value
  is different on each display.

 ******************************************************************************/

const JUtf8Byte*
DNDData::GetDNDAtomName()
{
	return kDragLinesXAtomName;
}

/******************************************************************************
 CreateBuffer (private)

  We fill the buffer with our data. The format is first the point count, then
  the point data.

 ******************************************************************************/

void
DNDData::CreateBuffer()
	const
{
	// This function must be const, since ConvertData is const, so we
	// need to do a const_cast on ourselves in order to fill the buffer.
	DNDData* th = const_cast<DNDData*>(this);
	if (itsBuffer == nullptr)
	{
		std::ostringstream os;

		// Write out the point count.
		const JSize count	= itsPoints->GetItemCount();
		os << count << ' ';

		// Loop through the points and write out their data.
		for (JIndex i = 1; i <= count; i++)
		{
			JPoint point	= itsPoints->GetItem(i);
			os << point << ' ';
		}

		// Create our buffer with the character pointer.
		th->itsBuffer = jnew JString(os.str());
	}
}
