/******************************************************************************
 JTableData.h

	Interface for the JTableData Class

	Copyright (C) 1996 John Lindal.

 ******************************************************************************/

#ifndef _H_JTableData
#define _H_JTableData

#include "JBroadcaster.h"
#include "JRect.h"

class JTableData : virtual public JBroadcaster
{
public:

	JTableData();
	JTableData(const JTableData& source);

	virtual ~JTableData();

	const JTableData& operator=(const JTableData& source);

	bool	IsEmpty() const;

	JSize		GetRowCount() const;
	bool	RowIndexValid(const JIndex index) const;

	JSize		GetColCount() const;
	bool	ColIndexValid(const JIndex index) const;

	bool	CellValid(const JPoint& cell) const;

protected:

	void	RowsAdded(const JSize count);
	void	RowsDeleted(const JSize count);
	void	SetRowCount(const JSize newRowCount);

	void	ColsAdded(const JSize count);
	void	ColsDeleted(const JSize count);
	void	SetColCount(const JSize newColCount);

private:

	JSize itsRowCount;
	JSize itsColCount;

public:

	// JBroadcaster messages

	static const JUtf8Byte* kRowsInserted;
	static const JUtf8Byte* kRowDuplicated;
	static const JUtf8Byte* kRowsRemoved;
	static const JUtf8Byte* kRowMoved;

	static const JUtf8Byte* kColsInserted;
	static const JUtf8Byte* kColDuplicated;
	static const JUtf8Byte* kColsRemoved;
	static const JUtf8Byte* kColMoved;

	static const JUtf8Byte* kRectChanged;

private:

	// base classes for JBroadcaster messages

	class RowColMessage : public JBroadcaster::Message
		{
		public:

			RowColMessage(const JUtf8Byte* type,
						  const JIndex firstIndex, const JSize count)
				:
				JBroadcaster::Message(type),
				itsFirstIndex(firstIndex),
				itsCount(count)
				{ };

			JIndex
			GetFirstIndex() const
			{
				return itsFirstIndex;
			};

			JIndex
			GetLastIndex() const
			{
				return itsFirstIndex + itsCount-1;
			};

			JSize
			GetCount() const
			{
				return itsCount;
			};

			bool
			Contains(const JIndex index) const
			{
				return GetFirstIndex() <= index && index <= GetLastIndex();
			};

		private:

			JIndex	itsFirstIndex;
			JSize	itsCount;
		};

	class RowColMoved : public JBroadcaster::Message
		{
		public:

			RowColMoved(const JUtf8Byte* type,
						const JIndex origIndex, const JIndex newIndex)
				:
				JBroadcaster::Message(type),
				itsOrigIndex(origIndex),
				itsNewIndex(newIndex)
				{ };

			void	AdjustIndex(JIndex* index) const;

			JIndex
			GetOrigIndex() const
			{
				return itsOrigIndex;
			};

			JIndex
			GetNewIndex() const
			{
				return itsNewIndex;
			};

		private:

			JIndex	itsOrigIndex;
			JIndex	itsNewIndex;
		};

	class RowColDupd : public JBroadcaster::Message
		{
		public:

			RowColDupd(const JUtf8Byte* type,
					   const JIndex origIndex, const JIndex newIndex)
				:
				JBroadcaster::Message(type),
				itsOrigIndex(origIndex),
				itsNewIndex(newIndex)
				{ };

			void	AdjustIndex(JIndex* index) const;

			JIndex
			GetOrigIndex() const
			{
				return itsOrigIndex;
			};

			JIndex
			GetNewIndex() const
			{
				return itsNewIndex;
			};

		private:

			JIndex	itsOrigIndex;	// index -before- row/col was duplicated
			JIndex	itsNewIndex;
		};

public:

	// row messages

	class RowsInserted : public RowColMessage
		{
		public:

			RowsInserted(const JIndex firstIndex, const JSize count)
				:
				RowColMessage(kRowsInserted, firstIndex, count)
				{ };

			void	AdjustIndex(JIndex* index) const;

			void
			AdjustCell(JPoint* cell) const
			{
				JIndex rowIndex = cell->y;
				AdjustIndex(&rowIndex);
				cell->y = rowIndex;
			};
		};

	class RowDuplicated : public RowColDupd
		{
		public:

			RowDuplicated(const JIndex origIndex, const JIndex newIndex)
				:
				RowColDupd(kRowDuplicated, origIndex, newIndex)
				{ };

			void
			AdjustCell(JPoint* cell) const
			{
				JIndex rowIndex = cell->y;
				AdjustIndex(&rowIndex);
				cell->y = rowIndex;
			};
		};

	class RowsRemoved : public RowColMessage
		{
		public:

			RowsRemoved(const JIndex firstIndex, const JSize count)
				:
				RowColMessage(kRowsRemoved, firstIndex, count)
				{ };

			bool	AdjustIndex(JIndex* index) const;

			bool
			AdjustCell(JPoint* cell) const
			{
				JIndex rowIndex   = cell->y;
				const bool ok = AdjustIndex(&rowIndex);
				cell->y           = rowIndex;
				return ok;
			};
		};

	class RowMoved : public RowColMoved
		{
		public:

			RowMoved(const JIndex origIndex, const JIndex newIndex)
				:
				RowColMoved(kRowMoved, origIndex, newIndex)
				{ };

			void
			AdjustCell(JPoint* cell) const
			{
				JIndex rowIndex = cell->y;
				AdjustIndex(&rowIndex);
				cell->y = rowIndex;
			};
		};

	// column messages

	class ColsInserted : public RowColMessage
		{
		public:

			ColsInserted(const JIndex firstIndex, const JSize count)
				:
				RowColMessage(kColsInserted, firstIndex, count)
				{ };

			void	AdjustIndex(JIndex* index) const;

			void
			AdjustCell(JPoint* cell) const
			{
				JIndex colIndex = cell->x;
				AdjustIndex(&colIndex);
				cell->x = colIndex;
			};
		};

	class ColDuplicated : public RowColDupd
		{
		public:

			ColDuplicated(const JIndex origIndex, const JIndex newIndex)
				:
				RowColDupd(kColDuplicated, origIndex, newIndex)
				{ };

			void
			AdjustCell(JPoint* cell) const
			{
				JIndex colIndex = cell->x;
				AdjustIndex(&colIndex);
				cell->x = colIndex;
			};
		};

	class ColsRemoved : public RowColMessage
		{
		public:

			ColsRemoved(const JIndex firstIndex, const JSize count)
				:
				RowColMessage(kColsRemoved, firstIndex, count)
				{ };

			bool	AdjustIndex(JIndex* index) const;

			bool
			AdjustCell(JPoint* cell) const
			{
				JIndex colIndex   = cell->x;
				const bool ok = AdjustIndex(&colIndex);
				cell->x           = colIndex;
				return ok;
			};
		};

	class ColMoved : public RowColMoved
		{
		public:

			ColMoved(const JIndex origIndex, const JIndex newIndex)
				:
				RowColMoved(kColMoved, origIndex, newIndex)
				{ };

			void
			AdjustCell(JPoint* cell) const
			{
				JIndex colIndex = cell->x;
				AdjustIndex(&colIndex);
				cell->x = colIndex;
			};
		};

	// element messages

	class RectChanged : public JBroadcaster::Message
		{
		public:

			RectChanged(const JRect& r)
				:
				JBroadcaster::Message(kRectChanged),
				itsRect(r)
				{ };

			// single cell

			RectChanged(const JIndex row, const JIndex col)
				:
				JBroadcaster::Message(kRectChanged),
				itsRect(row, col, row+1, col+1)
				{ };

			const JRect&
			GetRect() const
			{
				return itsRect;
			};

		private:

			JRect	itsRect;
		};
};


/******************************************************************************
 IsEmpty

	Returns TRUE if there are no items in the JTableData.

 ******************************************************************************/

inline bool
JTableData::IsEmpty()
	const
{
	return itsRowCount == 0 || itsColCount == 0;
}

/******************************************************************************
 GetRowCount

 ******************************************************************************/

inline JSize
JTableData::GetRowCount()
	const
{
	return itsRowCount;
}

/******************************************************************************
 RowIndexValid

 ******************************************************************************/

inline bool
JTableData::RowIndexValid
	(
	const JIndex index
	)
	const
{
	return 1 <= index && index <= itsRowCount;
}

/******************************************************************************
 RowsAdded (protected)

 ******************************************************************************/

inline void
JTableData::RowsAdded
	(
	const JSize count
	)
{
	itsRowCount += count;
}

/******************************************************************************
 SetRowCount (protected)

 ******************************************************************************/

inline void
JTableData::SetRowCount
	(
	const JSize newRowCount
	)
{
	itsRowCount = newRowCount;
}

/******************************************************************************
 GetColCount

 ******************************************************************************/

inline JSize
JTableData::GetColCount()
	const
{
	return itsColCount;
}

/******************************************************************************
 ColIndexValid

 ******************************************************************************/

inline bool
JTableData::ColIndexValid
	(
	const JIndex index
	)
	const
{
	return 1 <= index && index <= itsColCount;
}

/******************************************************************************
 CellValid

 ******************************************************************************/

inline bool
JTableData::CellValid
	(
	const JPoint& cell
	)
	const
{
	return RowIndexValid(cell.y) && ColIndexValid(cell.x);
}

/******************************************************************************
 ColsAdded (protected)

 ******************************************************************************/

inline void
JTableData::ColsAdded
	(
	const JSize count
	)
{
	itsColCount += count;
}

/******************************************************************************
 SetColCount (protected)

 ******************************************************************************/

inline void
JTableData::SetColCount
	(
	const JSize newColCount
	)
{
	itsColCount = newColCount;
}

#endif
