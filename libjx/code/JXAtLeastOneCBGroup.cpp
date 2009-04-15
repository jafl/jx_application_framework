/******************************************************************************
 JXAtLeastOneCBGroup.cpp

	At least one checkbox must be selected.  If the last one is turned off,
	the active, adjacent one is turned on.  The exact algorithm is explained
	in "Tog on Interface".  The checkboxes are assumed to be ordered in the
	order they were added.

	BASE CLASS = JXCheckboxGroup

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXAtLeastOneCBGroup.h>
#include <JXCheckbox.h>
#include <JMinMax.h>
#include <stdarg.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXAtLeastOneCBGroup::JXAtLeastOneCBGroup()
	:
	JXCheckboxGroup(),
	itsDirection(kSlideUp),
	itsIgnoreChangeFlag(kJFalse)
{
}

JXAtLeastOneCBGroup::JXAtLeastOneCBGroup
	(
	const JPtrArray<JXCheckbox>& cbList
	)
	:
	JXCheckboxGroup(cbList),
	itsDirection(kSlideUp),
	itsIgnoreChangeFlag(kJFalse)
{
}

JXAtLeastOneCBGroup::JXAtLeastOneCBGroup
	(
	const JSize	count,
	JXCheckbox*	cb1,
	JXCheckbox*	cb2,
	...
	)
	:
	JXCheckboxGroup(),
	itsDirection(kSlideUp),
	itsIgnoreChangeFlag(kJFalse)
{
	assert( count >= 2 );

	Add(cb1);
	Add(cb2);

	va_list argList;
	va_start(argList, cb2);

	for (JIndex i=3; i<=count; i++)
		{
		Add(va_arg(argList, JXCheckbox*));
		}

	va_end(argList);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXAtLeastOneCBGroup::~JXAtLeastOneCBGroup()
{
}

/******************************************************************************
 EnforceConstraints (virtual protected)

 ******************************************************************************/

void
JXAtLeastOneCBGroup::EnforceConstraints
	(
	const JIndex cbIndex
	)
{
	// we only need to do something if all the checkboxes are off

	if (itsIgnoreChangeFlag || AllDisabled())
		{
		return;
		}
	else if ((GetCheckbox(cbIndex))->IsChecked() || !AllUnchecked())
		{
		itsDirection = kSlideUp;
		return;
		}

	// slide to the adjacent checkbox, skipping over disabled ones

	JIndex newIndex = GetNextActiveIndex(cbIndex);
	if (newIndex == cbIndex)						// may have hit the end and bounced back
		{
		newIndex = GetNextActiveIndex(cbIndex);		// if newID == id, then there is only one enabled
		}

	// turn the new checkbox on

	itsIgnoreChangeFlag = kJTrue;
	(GetCheckbox(newIndex))->SetState(kJTrue);
	itsIgnoreChangeFlag = kJFalse;
}

/******************************************************************************
 GetNextActiveIndex (private)

 ******************************************************************************/

JIndex
JXAtLeastOneCBGroup::GetNextActiveIndex
	(
	const JIndex cbIndex
	)
{
	const JSize count = GetElementCount();
	if (count < 2)
		{
		return cbIndex;
		}

	JIndex newIndex = cbIndex;
	do
		{
		if (newIndex == 1)
			{
			itsDirection = kSlideDown;
			}
		else if (newIndex == count)
			{
			itsDirection = kSlideUp;
			}

		if (itsDirection == kSlideUp)
			{
			newIndex = JMax((JIndex) 1, newIndex-1);
			}
		else
			{
			newIndex = JMin(count, newIndex+1);
			}
		}
		while (CheckboxDisabled(newIndex));

	return newIndex;
}
