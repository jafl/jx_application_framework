/******************************************************************************
 Record.h

	Copyright (C) 2010 by John Lindal.

 *****************************************************************************/

#ifndef _H_Record
#define _H_Record

#include <jx-af/jcore/JList.h>
#include <jx-af/jcore/JString.h>

class Record : public JBroadcaster
{
public:

	Record();
	Record(const JString& fileName);
	Record(std::istream& input);	// version must already be checked

	~Record() override;

	bool			IsValid() const;
	bool			IsArrayNew() const;
	int				GetBucketIndex() const;
	const JString&	GetNewFile() const;
	JIndex			GetNewLine() const;
	JSize			GetSize() const;
	const JString&	GetData() const;

	static std::weak_ordering
		CompareState(Record * const & r1, Record * const & r2);
	static std::weak_ordering
		CompareFileNames(Record * const & r1, Record * const & r2);
	static std::weak_ordering
		CompareSizes(Record * const & r1, Record * const & r2);
	static std::weak_ordering
		CompareData(Record * const & r1, Record * const & r2);

private:

	bool	itsIsValidFlag;
	bool	itsIsArrayNewFlag;
	int		itsBucketIndex;
	JString	itsNewFile;
	JIndex	itsNewLine;
	JSize	itsSize;
	JString	itsData;

private:

	// not allowed

	Record(const Record&) = delete;
	Record& operator=(const Record&) = delete;
};


/******************************************************************************
 IsValid

 ******************************************************************************/

inline bool
Record::IsValid()
	const
{
	return itsIsValidFlag;
}

/******************************************************************************
 IsArrayNew

 ******************************************************************************/

inline bool
Record::IsArrayNew()
	const
{
	return itsIsArrayNewFlag;
}

/******************************************************************************
 GetBucketIndex

 ******************************************************************************/

inline int
Record::GetBucketIndex()
	const
{
	return itsBucketIndex;
}

/******************************************************************************
 GetNewFile

 ******************************************************************************/

inline const JString&
Record::GetNewFile()
	const
{
	return itsNewFile;
}

/******************************************************************************
 GetNewLine

 ******************************************************************************/

inline JIndex
Record::GetNewLine()
	const
{
	return itsNewLine;
}

/******************************************************************************
 GetSize

 ******************************************************************************/

inline JSize
Record::GetSize()
	const
{
	return itsSize;
}

/******************************************************************************
 GetData

 ******************************************************************************/

inline const JString&
Record::GetData()
	const
{
	return itsData;
}

#endif
