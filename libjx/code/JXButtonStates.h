/******************************************************************************
 JXButtonStates.h

	Interface for JXButtonStates.cc

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXButtonStates
#define _H_JXButtonStates

#include <X11/Xlib.h>
#include <jTypes.h>

const JSize kXButtonCount = 7;

enum JXMouseButton
{
	kJXNoButton = -1,		// internal use

	kJXButton1 = Button1,
	kJXButton2 = Button2,
	kJXButton3 = Button3,
	kJXButton4 = Button4,
	kJXButton5 = Button5,
	kJXButton6 = 6,
	kJXButton7 = 7,

	kJXLeftButton   = kJXButton1,
	kJXMiddleButton = kJXButton2,
	kJXRightButton  = kJXButton3
};

class JXButtonStates
{
public:

	JXButtonStates();
	JXButtonStates(const unsigned int state);

	JBoolean		GetState(const JIndex i) const;
	void			SetState(const JIndex i, const JBoolean pushed);
	JBoolean		AllOff() const;

	unsigned int	GetState() const;
	void			SetState(const unsigned int state);

	void			Clear();

	JBoolean
	button(const JIndex i)
		const
	{
		return GetState(i);
	};

	JBoolean
	left()
		const
	{
		return itsState[0];
	};

	JBoolean
	middle()
		const
	{
		return itsState[1];
	};

	JBoolean
	right()
		const
	{
		return itsState[2];
	};

	void
	ToggleState(const JIndex i)
	{
		SetState(i, !GetState(i));
	};

	static JBoolean
	AllOff(const unsigned int state)
	{
		return JConvertToBoolean( (state & 0x1F00) == 0 );
	};

	static JBoolean		GetState(const unsigned int state, const JIndex i);
	static unsigned int	SetState(const unsigned int state,
								 const JIndex i, const JBoolean pushed);

	static unsigned int
	ToggleState(const unsigned int state, const JIndex i)
	{
		return SetState(state, i, !GetState(state, i));
	};

private:

	JBoolean itsState[ kXButtonCount ];
};


inline int
operator==
	(
	const JXButtonStates& b1,
	const JXButtonStates& b2
	)
{
	return (b1.GetState() == b2.GetState());
}

inline int
operator!=
	(
	const JXButtonStates& b1,
	const JXButtonStates& b2
	)
{
	return (b1.GetState() != b2.GetState());
}

#endif
