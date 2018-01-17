/******************************************************************************
 JXKeyModifiers.h

	Interface for JXKeyModifiers class

	Copyright (C) 1996-2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXKeyModifiers
#define _H_JXKeyModifiers

#include <X11/Xlib.h>
#include <jTypes.h>

class JXDisplay;

const JSize kXModifierCount        = 8;
const JSize kJXKeyModifierMapCount = 14;

enum JXModifierKey
{
	kJXShiftKeyIndex = 1,
	kJXShiftLockKeyIndex,
	kJXControlKeyIndex,
	kJXMod1KeyIndex,
	kJXMod2KeyIndex,
	kJXMod3KeyIndex,
	kJXMod4KeyIndex,
	kJXMod5KeyIndex,

	// these are dynamically mapped to the above indices
	// (same order as kModifierKeySymList in JXDisplay.cpp)

	kJXNumLockKeyIndex,
	kJXScrollLockKeyIndex,
	kJXMetaKeyIndex,
	kJXAltKeyIndex,
	kJXSuperKeyIndex,
	kJXHyperKeyIndex
};

class JXKeyModifiers
{
public:

	JXKeyModifiers(const JXDisplay* display);
	JXKeyModifiers(const JXDisplay* display, const unsigned int state);

	JBoolean		Available(const JIndex i) const;
	JBoolean		GetState(const JIndex i) const;
	void			SetState(const JIndex i, const JBoolean pushed);
	JBoolean		AllOff() const;

	unsigned int	GetState() const;
	void			SetState(const JXDisplay* display, const unsigned int state);

	void			Clear();

	JBoolean
	key(const JIndex i)
		const
	{
		return GetState(i);
	};

	JBoolean
	shift()
		const
	{
		return itsState[ MapKey(kJXShiftKeyIndex) ];
	};

	JBoolean
	shiftLock()
		const
	{
		return itsState[ MapKey(kJXShiftLockKeyIndex) ];
	};

	JBoolean
	numLock()
		const
	{
		return itsState[ MapKey(kJXNumLockKeyIndex) ];
	};

	JBoolean
	scrollLock()
		const
	{
		return itsState[ MapKey(kJXScrollLockKeyIndex) ];
	};

	JBoolean
	control()
		const
	{
		return itsState[ MapKey(kJXControlKeyIndex) ];
	};

	JBoolean
	meta()
		const
	{
		return itsState[ MapKey(kJXMetaKeyIndex) ];
	};

	JBoolean
	alt()
		const
	{
		return itsState[ MapKey(kJXAltKeyIndex) ];
	};

	JBoolean
	super()
		const
	{
		return itsState[ MapKey(kJXSuperKeyIndex) ];
	};

	JBoolean
	hyper()
		const
	{
		return itsState[ MapKey(kJXHyperKeyIndex) ];
	};

	void
	ToggleState(const JIndex i)
	{
		SetState(i, !GetState(i));
	};

	static JBoolean
	AllOff(const JXDisplay* display, const unsigned int state)
	{
		return JConvertToBoolean( (state & 0x00FF) == 0 );
	};

	static JBoolean		GetState(const JXDisplay* display,
								 const unsigned int state, const JIndex i);
	static unsigned int	SetState(const JXDisplay* display, const unsigned int state,
								 const JIndex i, const JBoolean pushed);

	static unsigned int
	ToggleState(const JXDisplay* display, const unsigned int state, const JIndex i)
	{
		return SetState(display, state, i, !GetState(display, state, i));
	};

private:

	JBoolean	itsState[ 1+kXModifierCount ];	// [0] is always kJFalse
	const int*	itsMap;							// 1+kJXKeyModifierMapCount; element is zero if unmapped

private:

	int
	MapKey(const JIndex i)
		const
	{
		return itsMap[i];
	};
};


inline int
operator==
	(
	const JXKeyModifiers& m1,
	const JXKeyModifiers& m2
	)
{
	return (m1.GetState() == m2.GetState());
}

inline int
operator!=
	(
	const JXKeyModifiers& m1,
	const JXKeyModifiers& m2
	)
{
	return (m1.GetState() != m2.GetState());
}

#endif
