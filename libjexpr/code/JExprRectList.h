/******************************************************************************
 JExprRectList.h

	Interface for the JExprRectList class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JExprRectList
#define _H_JExprRectList

#include <jx-af/jcore/JContainer.h>
#include <jx-af/jcore/JPtrArray.h>
#include <jx-af/jcore/JRect.h>

class JFunction;

class JExprRectList : public JContainer
{
public:

	JExprRectList();

	~JExprRectList() override;

	JIndex		AddRect(const JRect& rect, const JCoordinate midline,
						const JSize fontSize, JFunction* f);
	JRect		GetRect(const JIndex index) const;
	JRect		GetBoundsRect() const;
	void		ShiftRect(const JIndex index, const JCoordinate dx, const JCoordinate dy);
	JCoordinate	GetMidline(const JIndex index) const;
	void		SetMidline(const JIndex index, const JCoordinate y);

	JSize	GetFontSize(const JIndex index) const;

	JFunction*			GetFunction(const JIndex index);
	const JFunction*	GetFunction(const JIndex index) const;
	bool				FindFunction(const JFunction* f, JIndex* index) const;

	JIndex	GetSelection(const JPoint& startPt, const JPoint& currPt) const;
	bool	SelectionValid(const JIndex index) const;

	JIndex	GetParent(const JIndex index) const;

private:

	struct ExtraInfo
	{
		JCoordinate	midline;
		JSize		fontSize;
	};

private:

	JArray<JRect>*			itsRects;
	JArray<ExtraInfo>*		itsExtraInfo;
	JPtrArray<JFunction>*	itsFunctions;

private:

	// not allowed

	JExprRectList(const JExprRectList&) = delete;
	JExprRectList& operator=(const JExprRectList&) = delete;
};

/******************************************************************************
 GetRect

 ******************************************************************************/

inline JRect
JExprRectList::GetRect
	(
	const JIndex index
	)
	const
{
	return itsRects->GetItem(index);
}

/******************************************************************************
 GetBoundsRect

 ******************************************************************************/

inline JRect
JExprRectList::GetBoundsRect()
	const
{
	return itsRects->GetLastItem();
}

/******************************************************************************
 GetMidline

 ******************************************************************************/

inline JCoordinate
JExprRectList::GetMidline
	(
	const JIndex index
	)
	const
{
	const ExtraInfo info = itsExtraInfo->GetItem(index);
	return info.midline;
}

/******************************************************************************
 SetMidline

 ******************************************************************************/

inline void
JExprRectList::SetMidline
	(
	const JIndex		index,
	const JCoordinate	y
	)
{
	ShiftRect(index, 0, y - GetMidline(index));
}

/******************************************************************************
 GetFontSize

 ******************************************************************************/

inline JSize
JExprRectList::GetFontSize
	(
	const JIndex index
	)
	const
{
	const ExtraInfo info = itsExtraInfo->GetItem(index);
	return info.fontSize;
}

/******************************************************************************
 GetFunction

 ******************************************************************************/

inline JFunction*
JExprRectList::GetFunction
	(
	const JIndex index
	)
{
	return itsFunctions->GetItem(index);
}

inline const JFunction*
JExprRectList::GetFunction
	(
	const JIndex index
	)
	const
{
	return itsFunctions->GetItem(index);
}

/******************************************************************************
 FindFunction

 ******************************************************************************/

inline bool
JExprRectList::FindFunction
	(
	const JFunction*	f,
	JIndex*				index
	)
	const
{
	return itsFunctions->Find(f, index);
}

/******************************************************************************
 SelectionValid

 ******************************************************************************/

inline bool
JExprRectList::SelectionValid
	(
	const JIndex index
	)
	const
{
	return 1 <= index && index <= itsRects->GetItemCount();
}

#endif
