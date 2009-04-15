/******************************************************************************
 JXProgressIndicator.h

	Interface for the JXProgressIndicator class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXProgressIndicator
#define _H_JXProgressIndicator

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWidget.h>

class JXProgressIndicator : public JXWidget
{
public:

	JXProgressIndicator(JXContainer* enclosure,
						const HSizingOption hSizing, const VSizingOption vSizing,
						const JCoordinate x, const JCoordinate y,
						const JCoordinate w, const JCoordinate h);

	virtual ~JXProgressIndicator();

	JSize	GetValue() const;
	void	SetValue(const JSize value);
	void	IncrementValue(const JSize delta = 1);

	JSize	GetMaxValue() const;
	void	SetMaxValue(const JSize maxValue);

protected:

	virtual void	Draw(JXWindowPainter& p, const JRect& rect);	
	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame);

private:

	JSize		itsValue;
	JSize		itsMaxValue;
	JColorIndex	itsForeColor;
	JColorIndex	itsBackColor;

private:

	// not allowed

	JXProgressIndicator(const JXProgressIndicator& source);
	const JXProgressIndicator& operator=(const JXProgressIndicator& source);
};


/******************************************************************************
 GetValue

 ******************************************************************************/

inline JSize
JXProgressIndicator::GetValue()
	const
{
	return itsValue;
}

/******************************************************************************
 IncrementValue

 ******************************************************************************/

inline void
JXProgressIndicator::IncrementValue
	(
	const JSize delta
	)
{
	SetValue(itsValue + delta);
}

/******************************************************************************
 GetMaxValue

 ******************************************************************************/

inline JSize
JXProgressIndicator::GetMaxValue()
	const
{
	return itsMaxValue;
}

#endif
