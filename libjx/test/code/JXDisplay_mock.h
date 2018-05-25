/******************************************************************************
 JXDisplay.h

	Mock for the JXDisplay class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_JXDisplay
#define _H_JXDisplay

#include <JXCursor.h>
#include <JArray.h>
#include <jXEventUtil.h>
#include <JXKeyModifiers.h>	// need defn of kJXKeyModifierMapCount
#include <JRect.h>
#include <JString.h>

class JXDisplay : virtual public JBroadcaster
{
public:

	JXDisplay(const JBoolean isOSX);

	JBoolean	KeycodeToModifier(const KeyCode keycode, JIndex* modifierIndex) const;

private:

	JString					itsName;
	void*					itsXDisplay;
	void*					itsColorManager;
	const JSize				itsMaxStringByteCount;
	JBoolean				itsIsOSXFlag;
	mutable JArray<JRect>*	itsBounds;
	JSize					itsShrinkDisplayToScreenRefCount;

	Time		itsLastEventTime;
	Time		itsLastIdleTime;
	Time		itsLastMotionNotifyTime;

	JPoint				itsLatestMouseLocation;
	JXButtonStates		itsLatestButtonStates;
	JXKeyModifiers		itsLatestKeyModifiers;
	XModifierKeymap*	itsModifierKeymap;
	int					itsJXKeyModifierMapping [ 1+kJXKeyModifierMapCount ];
};

#endif
