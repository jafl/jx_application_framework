/******************************************************************************
 JMMRecord.cpp

	JMMRecord objects record statistics on a block allocated through
	JMemoryManager.

	For maximum space efficiency JMMRecord does NOT have a virtual destructor.
	It isn't intended to be a base class, so if you do subclass it be careful.

	03/12/05 (Dustin Laurence)

	In fact, JMMRecord is just Plain Old Data (POD) except that it has
	constructors.  The offsetof operator can only be called on POD, so
	JMMRecordData defines a POD type that PrintLayout can call offsetof on
	without generating stupid compiler warnings.  This requires using assignment
	instead of initializers in the JMMRecord constructor, but this should not
	even incur a performance penalty because JMMRecordData contains only
	intrinsic POD types.

	Shutting up the compiler is the *ONLY* reason JMMRecordData exists, so
	don't use it for anything else.

	BASE CLASS = private JMMRecordData

	Copyright (C) 1997, 2005 by Dustin Laurence.  All rights reserved.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

//Class Header
#include <JMMRecord.h>

#include <JMemoryManager.h>
#include <JString.h>
#include <stddef.h> // For offsetof
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

JMMRecord::JMMRecord()
{
	// Defined but not initialized in JMMRecordData
	itsAddress = NULL;
	itsNewFile = NULL;
	itsDeleteFile = NULL;
	itsSize = 0;

	itsID = 0;
	itsNewLine = 0;
	itsDeleteLine = 0;

	itsMark = 0;
	itsArrayNewFlag = kJFalse;
	itsArrayDeleteFlag = 0;
	itsManagerMemoryFlag = kJFalse;
}

JMMRecord::JMMRecord
	(
	const JUInt32     id,
	const void*       address,
	const size_t      size,
	const JCharacter* file,
	const JUInt32     lineNumber,
	const JBoolean    array,
	const JBoolean    managerMemory
	)
{
	// Defined but not initialized in JMMRecordData
	itsAddress = address;
	itsNewFile = file;
	itsDeleteFile = NULL;
	itsSize = size;

	itsID = id;
	itsNewLine = lineNumber;
	itsDeleteLine = 0;

	itsMark = 0;
	itsArrayNewFlag = array;
	itsArrayDeleteFlag = 0;
	itsManagerMemoryFlag = managerMemory;

	// These values are only allowed in a default constructor, and may only
	// be changed in a copy constructor!
	assert(itsAddress != NULL);
	assert(itsSize != 0);
	assert(itsNewFile != NULL);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

// #if'd out in case stupid compilers don't know to optimize away the do-nothing
// destructor call--for JMMRecord tiny performance changes actually matter.  If
// you inherit from JMMRecord you deserve your fate anyway.
// -- Dustin 03/12/05
#if 0
JMMRecord::~JMMRecord()
{
}
#endif

/******************************************************************************
 SetDeleteLocation

 *****************************************************************************/

void
JMMRecord::SetDeleteLocation
	(
	const JCharacter* deleteFile,
	const JSize       deleteLine,
	const JBoolean    arrayDelete
	)
{
	assert(itsDeleteFile == NULL);
	assert(itsDeleteLine == 0);
	assert(deleteFile != NULL);
	itsDeleteFile = deleteFile;
	itsDeleteLine = deleteLine;
	itsArrayDeleteFlag = arrayDelete;
}

/******************************************************************************
 NewTypeName

 *****************************************************************************/

const JCharacter*
JMMRecord::NewTypeName() const
{
	return TypeName(itsArrayNewFlag);
}

/******************************************************************************
 DeleteTypeName

 *****************************************************************************/

const JCharacter*
JMMRecord::DeleteTypeName() const
{
	return TypeName(itsArrayDeleteFlag);
}

/******************************************************************************
 TypeName (static)

 *****************************************************************************/

const JCharacter*
JMMRecord::TypeName
	(
	const unsigned isArray
	)
{
	return (isArray) ? "array" : "object";
}

/******************************************************************************
 StreamForDebug

	Dual function is MDRecord constructor.

 *****************************************************************************/

void
JMMRecord::StreamForDebug
	(
	ostream& output
	)
	const
{
	output << JNegate(IsDeleted());
	output << ' ' << ArrayNew();
	output << ' ' << JString(itsNewFile);
	output << ' ' << itsNewLine;
	output << ' ' << itsSize;
	output << ' ' << JString((JCharacter*) itsAddress);
}

#if 0
/******************************************************************************
 Print

 *****************************************************************************/

JString
JMMRecord::Print()
	const
{
	JString string;

	string += "ID: ";
	string += itsID;
	string += "\nAddress: ";
	string += static_cast(unsigned long, itsAddress);
	string += "\nSize: ";
	string += itsSize;
	string += "\nNew  File: ";
	string += itsNewFile;
	string += "\n     Line: ";
	string += itsNewLine;
	string += "\nDelete  File: ";
	string += itsDeleteFile;
	string += "\n        Line: ";
	string += itsDeleteLine;
	string += "\n";

	return string;
}
#endif

/******************************************************************************
 PrintLayout

	Prints the layout of a JMMRecord in memory.  Useful for optimizing the
	memory manager's memory usage on a particular architecture, which can be
	important considering how many records can potentially be stored.

	This method ought to be static but isn't to accomodate certain compilers
	implement sizeof() and/or offsetof().  It isn't very hard to use the default
	constructor to create a throwaway object anyway.

	03/12/05 (Dustin)

	PrintLayout now prints the layout of JMMRecordData so picky compilers won't
	complain about applying offsetof to a non-POD type.  It would be legal
	(though stupid) according to the standard for JMMRecord to not be identical
	to JMMRecordData, but that doesn't matter because the JMMRecordData part of
	JMMRecord would surely be bitwise identical to JMMRecordData anyway and
	therefore optimizing the packing of JMMRecordData amounts to optimizing of
	JMMRecord.

 *****************************************************************************/

void
JMMRecord::PrintLayout()
{
	cout << "Memory layout of JMMRecord:" << endl;

	cout << "\n      itsAddress = " << offsetof(JMMRecordData, itsAddress) << "-"
		 << offsetof(JMMRecordData, itsAddress)+sizeof(JMMRecordData::itsAddress)-1 << endl;
	cout << "      itsNewFile = " << offsetof(JMMRecordData, itsNewFile) << "-"
		 << offsetof(JMMRecordData, itsNewFile)+sizeof(JMMRecordData::itsNewFile)-1 << endl;
	cout << "   itsDeleteFile = " << offsetof(JMMRecordData, itsDeleteFile) << "-"
		 << offsetof(JMMRecordData, itsDeleteFile)+sizeof(JMMRecordData::itsDeleteFile)-1 << endl;
	cout << "         itsSize = " << offsetof(JMMRecordData, itsSize) << "-"
		 << offsetof(JMMRecordData, itsSize)+sizeof(JMMRecordData::itsSize)-1 << endl;

	cout << "\n           itsID = " << offsetof(JMMRecordData, itsID) << "-"
		 << offsetof(JMMRecordData, itsID)+sizeof(JMMRecordData::itsID)-1 << endl;
	cout << "      itsNewLine = " << offsetof(JMMRecordData, itsNewLine) << "-"
		 << offsetof(JMMRecordData, itsNewLine)+sizeof(JMMRecordData::itsNewLine)-1 << endl;
	cout << "   itsDeleteLine = " << offsetof(JMMRecordData, itsDeleteLine) << "-"
		 << offsetof(JMMRecordData, itsDeleteLine)+sizeof(JMMRecordData::itsDeleteLine)-1 << endl;

	cout << "\n       bitfields = "
		 << offsetof(JMMRecordData, itsDeleteLine)+sizeof(JMMRecordData::itsDeleteLine) << "-"
		 << sizeof(JMMRecordData)-1 << "\n" << endl;
}
