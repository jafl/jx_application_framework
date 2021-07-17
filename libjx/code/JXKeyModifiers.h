/******************************************************************************
 JXKeyModifiers.h

	Interface for JXKeyModifiers class

	Copyright (C) 1996-2000 by John Lindal.

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

	bool		Available(const JIndex i) const;
	bool		GetState(const JIndex i) const;
	void			SetState(const JIndex i, const bool pushed);
	bool		AllOff() const;

	unsigned int	GetState() const;
	void			SetState(const JXDisplay* display, const unsigned int state);

	void			Clear();

	bool
	key(const JIndex i)
		const
	{
		return GetState(i);
	};

	bool
	shift()
		const
	{
		return itsState[ MapKey(kJXShiftKeyIndex) ];
	};

	bool
	shiftLock()
		const
	{
		return itsState[ MapKey(kJXShiftLockKeyIndex) ];
	};

	bool
	numLock()
		const
	{
		return itsState[ MapKey(kJXNumLockKeyIndex) ];
	};

	bool
	scrollLock()
		const
	{
		return itsState[ MapKey(kJXScrollLockKeyIndex) ];
	};

	bool
	control()
		const
	{
		return itsState[ MapKey(kJXControlKeyIndex) ];
	};

	bool
	meta()
		const
	{
		return itsState[ MapKey(kJXMetaKeyIndex) ];
	};

	bool
	alt()
		const
	{
		return itsState[ MapKey(kJXAltKeyIndex) ];
	};

	bool
	super()
		const
	{
		return itsState[ MapKey(kJXSuperKeyIndex) ];
	};

	bool
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

	static bool
	AllOff(const JXDisplay* display, const unsigned int state)
	{
		return (state & 0x00FF) == 0;
	};

	static bool		GetState(const JXDisplay* display,
								 const unsigned int state, const JIndex i);
	static unsigned int	SetState(const JXDisplay* display, const unsigned int state,
								 const JIndex i, const bool pushed);

	static unsigned int
	ToggleState(const JXDisplay* display, const unsigned int state, const JIndex i)
	{
		return SetState(display, state, i, !GetState(display, state, i));
	};

private:

	bool	itsState[ 1+kXModifierCount ];	// [0] is always false
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
