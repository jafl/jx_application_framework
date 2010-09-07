/******************************************************************************
 MDRecord.h

	Copyright © 2010 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_MDRecord
#define _H_MDRecord

#include <JOrderedSet.h>
#include <JString.h>

class MDRecord : public JBroadcaster
{
public:

	MDRecord();
	MDRecord(const JCharacter* fileName);
	MDRecord(istream& input);	// version must already be checked

	~MDRecord();

	JBoolean		IsValid() const;
	JBoolean		IsArrayNew() const;
	const JString&	GetNewFile() const;
	JIndex			GetNewLine() const;
	JSize			GetSize() const;
	const JString&	GetData() const;

	static JOrderedSetT::CompareResult
		CompareState(MDRecord * const & r1, MDRecord * const & r2);
	static JOrderedSetT::CompareResult
		CompareFileName(MDRecord * const & r1, MDRecord * const & r2);
	static JOrderedSetT::CompareResult
		CompareSize(MDRecord * const & r1, MDRecord * const & r2);
	static JOrderedSetT::CompareResult
		CompareData(MDRecord * const & r1, MDRecord * const & r2);

private:

	JBoolean	itsIsValidFlag;
	JBoolean	itsIsArrayNewFlag;
	JString		itsNewFile;
	JIndex		itsNewLine;
	JSize		itsSize;
	JString		itsData;

private:

	// not allowed

	MDRecord(const MDRecord& source);
	const MDRecord& operator=(const MDRecord& source);
};


/******************************************************************************
 IsValid

 ******************************************************************************/

inline JBoolean
MDRecord::IsValid()
	const
{
	return itsIsValidFlag;
}

/******************************************************************************
 IsArrayNew

 ******************************************************************************/

inline JBoolean
MDRecord::IsArrayNew()
	const
{
	return itsIsArrayNewFlag;
}

/******************************************************************************
 GetNewFile

 ******************************************************************************/

inline const JString&
MDRecord::GetNewFile()
	const
{
	return itsNewFile;
}

/******************************************************************************
 GetNewLine

 ******************************************************************************/

inline JIndex
MDRecord::GetNewLine()
	const
{
	return itsNewLine;
}

/******************************************************************************
 GetSize

 ******************************************************************************/

inline JSize
MDRecord::GetSize()
	const
{
	return itsSize;
}

/******************************************************************************
 GetData

 ******************************************************************************/

inline const JString&
MDRecord::GetData()
	const
{
	return itsData;
}

#endif
