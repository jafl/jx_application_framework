/******************************************************************************
 MDRecordList.h

	Copyright (C) 2010 by John Lindal.

 *****************************************************************************/

#ifndef _H_MDRecordList
#define _H_MDRecordList

#include <JContainer.h>
#include <JPtrArray.h>

class MDRecord;

class MDRecordList : public JContainer
{
public:

	// remember to update MDHeaderWidget

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

	MDRecordList();

	virtual ~MDRecordList();

	void			AddRecord(MDRecord* record);
	const MDRecord*	GetRecord(const JIndex index) const;
	JBoolean		GetRecordIndex(const MDRecord* record, JIndex *index) const;
	JBoolean		ClosestMatch(const JCharacter* prefix, MDRecord** record) const;

	ColumnType	GetSortColumn() const;
	void		SetSortColumn(const JIndex index);

private:

	JPtrArray<MDRecord>*	itsRecords;
	JPtrArray<MDRecord>*	itsAlphaRecords;
	ColumnType				itsSortColumn;

private:

	// not allowed

	MDRecordList(const MDRecordList& source);
	const MDRecordList& operator=(const MDRecordList& source);

public:

	static const JCharacter* kPrepareForUpdate;
	static const JCharacter* kListChanged;

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

inline const MDRecord*
MDRecordList::GetRecord
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

inline JBoolean
MDRecordList::GetRecordIndex
	(
	const MDRecord*	record,
	JIndex*			index
	)
	const
{
	return itsRecords->Find(const_cast<MDRecord*>(record), index);
}

/******************************************************************************
 GetSortColumn

 ******************************************************************************/

inline MDRecordList::ColumnType
MDRecordList::GetSortColumn()
	const
{
	return itsSortColumn;
}

#endif
