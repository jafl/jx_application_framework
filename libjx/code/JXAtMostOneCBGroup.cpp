/******************************************************************************
 JXAtMostOneCBGroup.cpp

	At most one checkbox can be selected.  This is almost the same as a set
	of radio buttons, except that the currently selected one can be turned
	off.

	BASE CLASS = JXCheckboxGroup

	Copyright © 2006 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXAtMostOneCBGroup.h>
#include <JXCheckbox.h>
#include <JMinMax.h>
#include <stdarg.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXAtMostOneCBGroup::JXAtMostOneCBGroup()
	:
	JXCheckboxGroup(),
	itsIgnoreChangeFlag(kJFalse)
{
}

JXAtMostOneCBGroup::JXAtMostOneCBGroup
	(
	const JPtrArray<JXCheckbox>& cbList
	)
	:
	JXCheckboxGroup(cbList),
	itsIgnoreChangeFlag(kJFalse)
{
}

JXAtMostOneCBGroup::JXAtMostOneCBGroup
	(
	const JSize	count,
	JXCheckbox*	cb1,
	JXCheckbox*	cb2,
	...
	)
	:
	JXCheckboxGroup(),
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

JXAtMostOneCBGroup::~JXAtMostOneCBGroup()
{
}

/******************************************************************************
 EnforceConstraints (virtual protected)

 ******************************************************************************/

void
JXAtMostOneCBGroup::EnforceConstraints
	(
	const JIndex cbIndex
	)
{
	// we only need to do something if all the checkboxes are off

	if (itsIgnoreChangeFlag || AllDisabled())
		{
		return;
		}
	else if ((GetCheckbox(cbIndex))->IsChecked())
		{
		const JSize count = GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			if (i != cbIndex)
				{
				(GetCheckbox(i))->SetState(kJFalse);
				}
			}
		}
}
