/******************************************************************************
 JXSelectionData.cpp

	This defines the interface for all objects that encapsulate data
	placed in an X selection.  Each object is restricted to work on a
	single X display since the atom id's are different on each one.

	Remember to always provide access to the data so derived classes
	can extend your class by providing additional types.

	BASE CLASS = JBroadcaster

	Copyright (C) 1996-2005 by John Lindal.

 ******************************************************************************/

#include "jx-af/jx/JXSelectionData.h"
#include "jx-af/jx/JXDisplay.h"
#include "jx-af/jx/JXWidget.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

	The second form is used for delayed evaluation.  The id must be something
	unique to a particular *class* so each class in the inheritance line
	that implements GetSelectionData() can either do the work or pass it to
	its base class.  In most cases, the class name is sufficient.

 ******************************************************************************/

JXSelectionData::JXSelectionData
	(
	JXDisplay* display
	)
	:
	itsDisplay(display),
	itsDataSource(nullptr)
{
	JXSelectionDataX();
}

JXSelectionData::JXSelectionData
	(
	JXWidget*			widget,
	const JUtf8Byte*	id
	)
	:
	itsDisplay(widget->GetDisplay()),
	itsDataSource(widget)
{
	assert( widget != nullptr && !JString::IsEmpty(id) );

	JXSelectionDataX();

	ListenTo(itsDataSource);	// need to know if it is deleted

	itsDataSourceID = jnew JString(id);
	assert( itsDataSourceID != nullptr );
}

// private

void
JXSelectionData::JXSelectionDataX()
{
	itsSelectionName = None;
	itsStartTime     = CurrentTime;
	itsEndTime       = CurrentTime;

	itsTypeList = jnew JArray<Atom>;
	assert( itsTypeList != nullptr );
	itsTypeList->SetCompareFunction(CompareAtoms);

	itsDataSourceID = nullptr;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXSelectionData::~JXSelectionData()
{
	jdelete itsTypeList;
	jdelete itsDataSourceID;
}

/******************************************************************************
 GetSelectionManager

 ******************************************************************************/

JXSelectionManager*
JXSelectionData::GetSelectionManager()
	const
{
	return itsDisplay->GetSelectionManager();
}

/******************************************************************************
 GetDNDManager

 ******************************************************************************/

JXDNDManager*
JXSelectionData::GetDNDManager()
	const
{
	return itsDisplay->GetDNDManager();
}

/******************************************************************************
 GetDNDSelectionName

 ******************************************************************************/

Atom
JXSelectionData::GetDNDSelectionName()
	const
{
	return (itsDisplay->GetDNDManager())->GetDNDSelectionName();
}

/******************************************************************************
 SetSelectionInfo

 ******************************************************************************/

void
JXSelectionData::SetSelectionInfo
	(
	const Atom	selectionName,
	const Time	startTime
	)
{
	assert( selectionName != None );

	itsSelectionName = selectionName;
	itsStartTime     = startTime;

	if (selectionName != GetDNDSelectionName())
	{
		JXSelectionManager* selMgr = GetSelectionManager();
		AddType(selMgr->GetTargetsXAtom());
		AddType(selMgr->GetTimeStampXAtom());
	}

	AddTypes(selectionName);
}

/******************************************************************************
 AddType (protected)

	Create the atom and add it to the list.

 ******************************************************************************/

Atom
JXSelectionData::AddType
	(
	const JUtf8Byte* name
	)
{
	const Atom atom = itsDisplay->RegisterXAtom(name);
	AddType(atom);
	return atom;
}

/******************************************************************************
 RemoveType (protected)

	Remove the type from the list.

 ******************************************************************************/

void
JXSelectionData::RemoveType
	(
	const Atom type
	)
{
	JIndex index;
	if (itsTypeList->SearchSorted(type, JListT::kAnyMatch, &index))
	{
		itsTypeList->RemoveElement(index);
	}
}

/******************************************************************************
 Resolve

	Asks its data source to set the data.  This is required for DND where
	the data should not be converted until it is needed, allowing the mouse
	drag to begin immediately.

	This cannot be called by JXSelectionManager because it doesn't know
	how the data is being used.

 ******************************************************************************/

void
JXSelectionData::Resolve()
	const
{
	if (itsDataSource != nullptr && itsDataSourceID != nullptr)
	{
		auto* me = const_cast<JXSelectionData*>(this);

		itsDataSource->GetSelectionData(me, *itsDataSourceID);

		jdelete me->itsDataSourceID;
		me->itsDataSource   = nullptr;
		me->itsDataSourceID = nullptr;
	}
}

/******************************************************************************
 Convert

	Handles certain types and passes everything else off to ConvertData().

	When adding special types to this function, remember to update
	SetSelectionInfo() to add the jnew types.

 ******************************************************************************/

bool
JXSelectionData::Convert
	(
	const Atom		requestType,
	Atom*			returnType,
	unsigned char**	data,
	JSize*			dataLength,
	JSize*			bitsPerBlock
	)
	const
{
	JXSelectionManager* selMgr = GetSelectionManager();

	// TARGETS

	if (requestType == selMgr->GetTargetsXAtom())
	{
		const JSize atomCount = itsTypeList->GetElementCount();
		assert( atomCount > 0 );

		*returnType   = XA_ATOM;
		*bitsPerBlock = 32;		// XXXATOM: sizeof(Atom)*8; -- fails on 64-bit systems
		*dataLength   = sizeof(Atom)*atomCount;

		*data = jnew unsigned char [ *dataLength ];
		if (*data == nullptr)
		{
			return false;
		}

		Atom* atomData = reinterpret_cast<Atom*>(*data);
		for (JIndex i=1; i<=atomCount; i++)
		{
			atomData[i-1] = itsTypeList->GetElement(i);
		}

		return true;
	}

	// TIMESTAMP

	else if (requestType == selMgr->GetTimeStampXAtom())
	{
		*returnType   = XA_INTEGER;
		*bitsPerBlock = 32;	// sizeof(Time)*8; -- fails on 64-bit systems
		*dataLength   = sizeof(Time);

		*data = jnew unsigned char [ *dataLength ];
		if (*data == nullptr)
		{
			return false;
		}

		*(reinterpret_cast<Time*>(*data)) = itsStartTime;

		return true;
	}

	// everything else

	else
	{
		Resolve();
		return ConvertData(requestType, returnType,
						   data, dataLength, bitsPerBlock);
	}
}

/******************************************************************************
 AddTypes (virtual protected)

	Call AddType() for whatever types are appropriate for the given selection.

 ******************************************************************************/

/******************************************************************************
 ConvertData (virtual protected)

	Derived class must convert data to the specified type and return
	true, or return false if the conversion cannot be accomplished.

	*returnType must be actual data type.  For example, when "TEXT" is
	requested, one often returns XA_STRING.

	*data must be allocated with "new unsigned char[]" and will be deleted
	by the caller.  *dataLength must be set to the length of *data.

	*bitsPerBlock must be set to the number of bits per element of data.
	e.g.	If data is text, *bitsPerBlock=8.
			If data is an int, *bitsPerBlock=sizeof(int)*8

	Since X performs byte swapping when *bitsPerBlock > 8, mixed data should
	be packed one byte at a time to ensure that it can be correctly decoded.

 ******************************************************************************/

/******************************************************************************
 ReceiveGoingAway (virtual protected)

	The given sender has been deleted.

 ******************************************************************************/

void
JXSelectionData::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (sender == itsDataSource)
	{
		jdelete itsDataSourceID;
		itsDataSource   = nullptr;
		itsDataSourceID = nullptr;
	}
	else
	{
		JBroadcaster::ReceiveGoingAway(sender);
	}
}

/******************************************************************************
 CompareAtoms (static private)

 ******************************************************************************/

JListT::CompareResult
JXSelectionData::CompareAtoms
	(
	const Atom& atom1,
	const Atom& atom2
	)
{
	if (atom1 < atom2)
	{
		return JListT::kFirstLessSecond;
	}
	else if (atom1 == atom2)
	{
		return JListT::kFirstEqualSecond;
	}
	else
	{
		return JListT::kFirstGreaterSecond;
	}
}
