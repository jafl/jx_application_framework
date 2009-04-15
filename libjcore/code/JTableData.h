/******************************************************************************
 JTableData.h

	Interface for the JTableData Class

	Copyright © 1996 John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JTableData
#define _H_JTableData

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JBroadcaster.h>
#include <JRect.h>

class JTableData : virtual public JBroadcaster
{
public:

	JTableData();
	JTableData(const JTableData& source);

	virtual ~JTableData();

	const JTableData& operator=(const JTableData& source);

	JBoolean	IsEmpty() const;

	JSize		GetRowCount() const;
	JBoolean	RowIndexValid(const JIndex index) const;

	JSize		GetColCount() const;
	JBoolean	ColIndexValid(const JIndex index) const;

	JBoolean	CellValid(const JPoint& cell) const;

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

	static const JCharacter* kRowsInserted;
	static const JCharacter* kRowDuplicated;
	static const JCharacter* kRowsRemoved;
	static const JCharacter* kRowMoved;

	static const JCharacter* kColsInserted;
	static const JCharacter* kColDuplicated;
	static const JCharacter* kColsRemoved;
	static const JCharacter* kColMoved;

	static const JCharacter* kRectChanged;

private:

	// base classes for JBroadcaster messages

	class RowColMessage : public JBroadcaster::Message
		{
		public:

			RowColMessage(const JCharacter* type,
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

			JBoolean
			Contains(const JIndex index) const
			{
				return JConvertToBoolean(
						GetFirstIndex() <= index && index <= GetLastIndex() );
			};

		private:

			JIndex	itsFirstIndex;
			JSize	itsCount;
		};

	class RowColMoved : public JBroadcaster::Message
		{
		public:

			RowColMoved(const JCharacter* type,
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

			RowColDupd(const JCharacter* type,
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

			JBoolean	AdjustIndex(JIndex* index) const;

			JBoolean
			AdjustCell(JPoint* cell) const
			{
				JIndex rowIndex   = cell->y;
				const JBoolean ok = AdjustIndex(&rowIndex);
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

			JBoolean	AdjustIndex(JIndex* index) const;

			JBoolean
			AdjustCell(JPoint* cell) const
			{
				JIndex colIndex   = cell->x;
				const JBoolean ok = AdjustIndex(&colIndex);
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

			RectChanged(const JPoint& cell)
				:
				JBroadcaster::Message(kRectChanged),
				itsRect(cell.y, cell.x, cell.y+1, cell.x+1)
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

inline JBoolean
JTableData::IsEmpty()
	const
{
	return JConvertToBoolean( itsRowCount == 0 || itsColCount == 0 );
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

inline JBoolean
JTableData::RowIndexValid
	(
	const JIndex index
	)
	const
{
	return JConvertToBoolean( 1 <= index && index <= itsRowCount );
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

inline JBoolean
JTableData::ColIndexValid
	(
	const JIndex index
	)
	const
{
	return JConvertToBoolean( 1 <= index && index <= itsColCount );
}

/******************************************************************************
 CellValid

 ******************************************************************************/

inline JBoolean
JTableData::CellValid
	(
	const JPoint& cell
	)
	const
{
	return JConvertToBoolean( RowIndexValid(cell.y) && ColIndexValid(cell.x) );
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
