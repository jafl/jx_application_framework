/******************************************************************************
 GMailboxUtils.cc

	Utility functions for dealing with files.

	Copyright © 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include <JString.h>
#include <JDirEntry.h>
#include <JRegex.h>

#include <jDirUtil.h>
#include <jFileUtil.h>
#include <jStreamUtil.h>
#include <jFStreamUtil.h>
#include <unistd.h>
#include <jAssert.h>

/******************************************************************************
 GetFileRoot

 ******************************************************************************/

JString
JGetFileRoot
	(
	const JCharacter* file
	)
{
	JString str(file);
	JIndex index;
	if (str.LocateLastSubstring(".", &index))
		{
		str.RemoveSubstring(index, str.GetLength());
		}
	return str;
}

/******************************************************************************
 GetFileSuffix

 ******************************************************************************/

JString
JGetFileSuffix
	(
	const JCharacter* file
	)
{
	JString str(file);
	JIndex index;
	if (str.LocateLastSubstring(".", &index))
		{
		str.RemoveSubstring(1, index - 1);
		}
	return str;
}

/******************************************************************************
 JGetURLFromFile

 ******************************************************************************/

JString
JGetURLFromFile
	(
	const JCharacter* file
	)
{
	JCharacter hostname[255];
	gethostname(hostname, 255);
	JString url("file://");
	url += hostname;
	url += file;
	return url;
}

/******************************************************************************
 JGetFileFromURL

 ******************************************************************************/

JBoolean
JGetFileFromURL
	(
	const JCharacter* url,
	JString* file
	)
{
	JString str(url);
	str.RemoveSubstring(1,7);
	JIndex index;
	JBoolean ok = str.LocateSubstring("/", &index);
	if (!ok)
		{
		return kJFalse;
		}
	JString urlhost = str.GetSubstring(1, index - 1);
	JCharacter hostname[255];
	gethostname(hostname, 255);
	if (urlhost != hostname)
		{
		return kJFalse;
		}
	*file = str.GetSubstring(index, str.GetLength());
	return kJTrue;
}

/******************************************************************************
 JAbsoluteToRelativePath

 ******************************************************************************/

void
JAbsoluteToRelativePath
	(
	const JCharacter* relativeTo,
	JString* path
	)
{
	JString base(relativeTo);
	JString test(*path);
	JAppendDirSeparator(&base);
	JAppendDirSeparator(&test);
	JIndex charindex = 2;
	JBoolean success = kJTrue;
	JString compare;
	JString store;
	success = test.LocateNextSubstring("/", &charindex);
	if (success)
		{
		compare = test.GetSubstring(1, charindex);
		}
	while (success && base.BeginsWith(compare))
		{
		store = compare;
		success = test.LocateNextSubstring("/", &charindex);
		if (success)
			{
			compare = test.GetSubstring(1, charindex);
			charindex ++;
			}
		}
	if (test.GetLength() >= store.GetLength())
		{
		test.RemoveSubstring(1, store.GetLength());
		}
	if (base.GetLength() >= store.GetLength())
		{
		base.RemoveSubstring(1, store.GetLength());
		}
	success = base.LocateSubstring("/", &charindex);
	while (success)
		{
		base.RemoveSubstring(1, charindex);
		test.Prepend("../");
		success = base.LocateSubstring("/", &charindex);
		}
	*path = test;
}

/******************************************************************************
 JRelativeToAbsolutePath

 ******************************************************************************/

void
JRelativeToAbsolutePath
	(
	const JCharacter* relativeTo,
	JString* path
	)
{
}

/******************************************************************************
 MatchesCookie

 ******************************************************************************/

JBoolean
MatchesCookie
	(
	const JCharacter*	cookie,
	const JDirEntry&	entry
	)
{
	JString file = entry.GetFullName();
	if (!JFileReadable(file))
		{
		return kJFalse;
		}

	mode_t perms;
	JError err = JGetPermissions(file, &perms);
	if (!err.OK())
		{
		perms = 0600;
		}
	ifstream is(file);
	is >> ws;
	JString line1 = JReadLine(is);
	is.close();
	if (line1 == "")
		{
		return kJTrue;
		}
	JArray<JIndexRange> subList;
	JRegex regex;
	err = regex.SetPattern(cookie);
	JBoolean matched = regex.Match(line1, &subList);
	if (matched)
		{
		return kJTrue;
		}
	return kJFalse;
}
