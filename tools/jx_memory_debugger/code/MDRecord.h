/******************************************************************************
 MDRecord.h

	Copyright (C) 2010 by John Lindal.

 *****************************************************************************/

#ifndef _H_MDRecord
#define _H_MDRecord

#include <JList.h>
#include <JString.h>

class MDRecord : public JBroadcaster
{
public:

	MDRecord();
	MDRecord(const JString& fileName);
	MDRecord(std::istream& input);	// version must already be checked

	~MDRecord();

	bool		IsValid() const;
	bool		IsArrayNew() const;
	const JString&	GetNewFile() const;
	JIndex			GetNewLine() const;
	JSize			GetSize() const;
	const JString&	GetData() const;

	static JListT::CompareResult
		CompareState(MDRecord * const & r1, MDRecord * const & r2);
	static JListT::CompareResult
		CompareFileName(MDRecord * const & r1, MDRecord * const & r2);
	static JListT::CompareResult
		CompareSize(MDRecord * const & r1, MDRecord * const & r2);
	static JListT::CompareResult
		CompareData(MDRecord * const & r1, MDRecord * const & r2);

private:

	bool	itsIsValidFlag;
	bool	itsIsArrayNewFlag;
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

inline bool
MDRecord::IsValid()
	const
{
	return itsIsValidFlag;
}

/******************************************************************************
 IsArrayNew

 ******************************************************************************/

inline bool
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
