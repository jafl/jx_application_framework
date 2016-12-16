/******************************************************************************
 JProbDistr.h

	Interface for JProbDistr class.

	Copyright (C) 1994 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JProbDistr
#define _H_JProbDistr

#include <JArray.h>

class JProbDistr
{
	friend std::ostream& operator<<(std::ostream& output, const JProbDistr& source);

public:

	JProbDistr(const JSize valueCount, const JFloat initialValue = 0.0);
	JProbDistr(std::istream& input);
	JProbDistr(const JProbDistr& source);

	virtual ~JProbDistr();

	const JProbDistr& operator=(const JProbDistr& source);

	JSize	GetElementCount() const;
	JFloat	GetProbability(const JIndex index) const;
	void	SetProbability(const JIndex index, const JFloat prob);

	JIndex	GetMostProbableElement() const;

	JProbDistr&		operator+=(const JProbDistr& theProbDistr);
	JBoolean		Normalize();

private:

	JArray<JFloat>*	itsProbs;
};

/******************************************************************************
 GetElementCount

 ******************************************************************************/

inline JSize
JProbDistr::GetElementCount()
	const
{
	return itsProbs->GetElementCount();
}

/******************************************************************************
 GetProbability

 ******************************************************************************/

inline JFloat
JProbDistr::GetProbability
	(
	const JIndex index
	)
	const
{
	return itsProbs->GetElement(index);
}

/******************************************************************************
 SetProbability

 ******************************************************************************/

inline void
JProbDistr::SetProbability
	(
	const JIndex	index,
	const JFloat	prob
	)
{
	itsProbs->SetElement(index, prob);
}

#endif
