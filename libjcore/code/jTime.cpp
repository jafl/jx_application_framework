/******************************************************************************
 jTime.cpp

	Routines to augment time.c

	Copyright (C) 1995 John Lindal. All rights reserved.

 ******************************************************************************/

#include <jTime.h>
#include <jMath.h>
#include <unistd.h>
#include <jMissingProto.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 JGetTimeStamp

	Returns the time stamp from ctime (minus the line feed) as a JString.

 ******************************************************************************/

JString
JGetTimeStamp()
{
	return JConvertToTimeStamp(time(NULL));
}

/******************************************************************************
 JConvertToTimeStamp

	Returns the time stamp from ctime (minus the line feed) as a JString.

 ******************************************************************************/

JString
JConvertToTimeStamp
	(
	const time_t t
	)
{
	JString timeStamp  = ctime(&t);
	const JSize length = timeStamp.GetLength();
	timeStamp.RemoveSubstring(length, length);
	return timeStamp;
}

/******************************************************************************
 JPrintTimeInterval

	Converts the given time interval in seconds into a string with a
	reasonable precision.

 ******************************************************************************/

JString
JPrintTimeInterval
	(
	const JUInt delta
	)
{
	if (delta < 60)
		{
		return JString(delta, JString::kBase10) + " sec";
		}
	else if (delta < 60*60)
		{
		return JString(delta/60.0, 0) + " min";
		}
	else if (delta < 60*60*24)
		{
		return JString(delta/3600.0, 1) + " hours";
		}
	else if (delta < 60*60*24*7)
		{
		return JString(delta/86400.0, 1) + " days";
		}
	else if (delta < 60*60*24*7*30)
		{
		return JString(delta/604800.0, 1) + " weeks";
		}
	else if (delta < 60*60*24*7*365)
		{
		return JString(delta/18144000.0, 1) + " months";
		}
	else
		{
		return JString(delta/220752000.0, 1) + " years";
		}
}

/******************************************************************************
 JCheckExpirationDate

	Warns user if time is within two weeks of expiration.  Calls exit()
	if time is beyond expiration.

	map must define:  date, name, version, url, email
	*** date must be first!

	*** The application string data must define WarnExpire::jTime and
		Expired::jTime, because these messages should be customized.

 ******************************************************************************/

void
JCheckExpirationDate
	(
	const time_t		expireTime,
	const JCharacter*	map[],
	const JSize			size
	)
{
	const time_t t = time(NULL);
	if (t > expireTime)
		{
		map[1] = "";
		const JString msg = JGetString("Expired::jTime", map, size);
		(JGetUserNotification())->DisplayMessage(msg);
		exit(0);
		}
	else if (t > expireTime - 14*24*3600)
		{
		JCharacter date[100];
		strftime(date, 100, "%B %e, %Y", localtime(&expireTime));
		map[1] = date;
		const JString msg = JGetString("WarnExpire::jTime", map, size);
		(JGetUserNotification())->DisplayMessage(msg);
		}
}

/******************************************************************************
 JGetTimezoneOffset

	Returns the offset to convert GMT to local time.

 ******************************************************************************/

long
JGetTimezoneOffset()
{
	static long delta = 0;
	if (delta == 0)
		{
		time_t t  = time(NULL);
		time_t t1 = mktime(gmtime(&t));
		time_t t2 = mktime(localtime(&t));
		delta     = t2 - t1;
		}

	return delta;
}
