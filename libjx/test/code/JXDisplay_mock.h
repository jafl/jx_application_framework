/******************************************************************************
 JXDisplay.h

	Mock for the JXDisplay class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_JXDisplay
#define _H_JXDisplay

#include <jx-af/jx/JXCursor.h>
#include <jx-af/jcore/JArray.h>
#include <jx-af/jx/jXEventUtil.h>
#include <jx-af/jx/JXKeyModifiers.h>	// need defn of kJXKeyModifierMapCount
#include <jx-af/jcore/JRect.h>
#include <jx-af/jcore/JString.h>

class JXDisplay : virtual public JBroadcaster
{
public:

	JXDisplay(const bool isOSX);

	bool	KeycodeToModifier(const KeyCode keycode, JIndex* modifierIndex) const;

private:

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
#endif

	JString					itsName;
	void*					itsXDisplay;
	void*					itsColorManager;
	const JSize				itsMaxStringByteCount;
	bool				itsIsOSXFlag;
	mutable JArray<JRect>*	itsBounds;
	JSize					itsShrinkDisplayToScreenRefCount;

	Time		itsLastEventTime;
	Time		itsLastIdleTime;
	Time		itsLastMotionNotifyTime;

	JPoint				itsLatestMouseLocation;
	JXButtonStates		itsLatestButtonStates;
	JXKeyModifiers		itsLatestKeyModifiers;
	XModifierKeymap*	itsModifierKeymap;

#ifdef __clang__
#pragma clang diagnostic pop
#endif

	int					itsJXKeyModifierMapping [ 1+kJXKeyModifierMapCount ];
};

#endif
