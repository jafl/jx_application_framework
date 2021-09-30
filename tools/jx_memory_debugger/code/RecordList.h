/******************************************************************************
 RecordList.h

	Copyright (C) 2010 by John Lindal.

 *****************************************************************************/

#ifndef _H_RecordList
#define _H_RecordList

#include <jx-af/jcore/JContainer.h>
#include <jx-af/jcore/JPtrArray.h>

class Record;

class RecordList : public JContainer
{
public:

	// remember to update HeaderWidget

	enum ColumnType
	{
		kRecordState = 1,
		kRecordFile,
		kRecordLine,
		kRecordSize,
		kRecordArray,
		kRecordData,

		kColumnCount = kRecordData
	};

public:

	RecordList();

	virtual ~RecordList();

	void			AddRecord(Record* record);
	const Record*	GetRecord(const JIndex index) const;
	bool			GetRecordIndex(const Record* record, JIndex *index) const;
	bool			ClosestMatch(const JString& prefix, Record** record) const;

	ColumnType	GetSortColumn() const;
	void		SetSortColumn(const JIndex index);

private:

	JPtrArray<Record>*	itsRecords;
	JPtrArray<Record>*	itsAlphaRecords;
	ColumnType				itsSortColumn;

private:

	// not allowed

	RecordList(const RecordList&) = delete;
	RecordList& operator=(const RecordList&) = delete;

public:

	static const JUtf8Byte* kPrepareForUpdate;
	static const JUtf8Byte* kListChanged;

	class PrepareForUpdate : public JBroadcaster::Message
		{
		public:

			PrepareForUpdate()
				:
				JBroadcaster::Message(kPrepareForUpdate)
				{ };
		};

	class ListChanged : public JBroadcaster::Message
		{
		public:

			ListChanged()
				:
				JBroadcaster::Message(kListChanged)
				{ };
		};
};


/******************************************************************************
 GetRecord

 ******************************************************************************/

inline const Record*
RecordList::GetRecord
	(
	const JIndex index
	)
	const
{
	return itsRecords->GetElement(index);
}

/******************************************************************************
 GetRecordIndex

 ******************************************************************************/

inline bool
RecordList::GetRecordIndex
	(
	const Record*	record,
	JIndex*			index
	)
	const
{
	return itsRecords->Find(const_cast<Record*>(record), index);
}

/******************************************************************************
 GetSortColumn

 ******************************************************************************/

inline RecordList::ColumnType
RecordList::GetSortColumn()
	const
{
	return itsSortColumn;
}

#endif
