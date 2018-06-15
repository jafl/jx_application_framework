/******************************************************************************
 jFileUtil.cpp

	Useful functions for dealing with files.

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "jFileUtil.h"
#include "jStreamUtil.h"
#include "jFStreamUtil.h"
#include "JStringIterator.h"
#include "JStringMatch.h"
#include "JRegex.h"
#include "jMountUtil.h"
#include "jSysUtil.h"
#include "jErrno.h"
#include "jMissingProto.h"
#include <limits.h>
#include "jAssert.h"

/******************************************************************************
 JRenameFile

	Renames the specified file.

 ******************************************************************************/

JError
JRenameFile
	(
	const JString&	oldName,
	const JString&	newName,
	const JBoolean	forceReplace
	)
{
	if (forceReplace)
		{
		JRemoveFile(newName);	// ignore errors
		}
	return JRenameDirEntry(oldName, newName);
}

/******************************************************************************
 JCombinePathAndName

	Concatenates path and name, inserting the appropriate separator
	if necessary.

 ******************************************************************************/

JString
JCombinePathAndName
	(
	const JString& path,
	const JString& name
	)
{
	assert( !path.IsEmpty() );
	assert( !name.IsEmpty() );

	JString file = path;
	if (file.GetLastCharacter() != ACE_DIRECTORY_SEPARATOR_CHAR &&
		name.GetFirstCharacter() != ACE_DIRECTORY_SEPARATOR_CHAR)
		{
		file.Append(ACE_DIRECTORY_SEPARATOR_STR);
		}
	file += name;
	JCleanPath(&file);
	return file;
}

/******************************************************************************
 JSplitPathAndName

	Splits fullName into a path and name.

	If fullName doesn't contain a directory separator, it returns kJFalse
	and *path is the result of JGetCurrentDirectory().

 ******************************************************************************/

JBoolean
JSplitPathAndName
	(
	const JString&	fullName,
	JString*		path,
	JString*		name
	)
{
	assert( !fullName.IsEmpty() );

	JString pathAndName = fullName;
	JStripTrailingDirSeparator(&pathAndName);

	if (JIsRootDirectory(pathAndName))
		{
		*path = pathAndName;
		name->Clear();
		return kJFalse;
		}

	JStringIterator iter(pathAndName, kJIteratorStartAtEnd);
	iter.BeginMatch();
	if (iter.Prev(ACE_DIRECTORY_SEPARATOR_STR))
		{
		*name = iter.FinishMatch().GetString();

		iter.SkipNext();	// include separator
		iter.BeginMatch();
		iter.MoveTo(kJIteratorStartAtBeginning, 0);
		*path = iter.FinishMatch().GetString();

		JCleanPath(path);
		return kJTrue;
		}
	else
		{
		*path = JGetCurrentDirectory();
		*name = pathAndName;
		return kJFalse;
		}
}

/******************************************************************************
 JAppendDirSeparator

	Appends the appropriate separator to the end of *dirName, if neccessary.

 ******************************************************************************/

void
JAppendDirSeparator
	(
	JString* dirName
	)
{
	assert( !dirName->IsEmpty() );

	if (dirName->GetLastCharacter() != ACE_DIRECTORY_SEPARATOR_CHAR)
		{
		dirName->Append(ACE_DIRECTORY_SEPARATOR_STR);
		}
}

/******************************************************************************
 JPrintFileSize

	Converts the given file size into a string with a reasonable precision.
	e.g. 1003280 -> 1MB

 ******************************************************************************/

JString
JPrintFileSize
	(
	const JSize size
	)
{
	if (size < 8192)
		{
		return JString((JUInt64) size);
		}
	else if (size < 1048576)
		{
		return JString(size/1024.0, 0) + "K";
		}
	else if (size < 134217728)
		{
		return JString(size/1048576.0, 1) + "M";
		}
	else if (size < 1073741824)
		{
		return JString(size/1048576.0, 0) + "M";
		}
	else
		{
		return JString(size/1073741824.0, 1) + "G";
		}
}

/******************************************************************************
 JSearchFile

	Returns the index of the first line that contains the given string.

 ******************************************************************************/

JBoolean
JSearchFile
	(
	const JString&	fileName,
	const JString&	searchStr,
	const JBoolean	caseSensitive,
	JIndex*			lineIndex
	)
{
	std::ifstream input(fileName.GetBytes());

	*lineIndex = 0;
	while (!input.eof())
		{
		(*lineIndex)++;
		const JString line = JReadLine(input);
		if (input.fail())
			{
			break;
			}
		if (line.Contains(searchStr, caseSensitive))
			{
			return kJTrue;
			}
		}

	return kJFalse;
}

/******************************************************************************
 JExtractFileAndLine

	Extracts file name and line index from "file:line-line".

	For backward compatibility, endLineIndex can be nullptr.

 ******************************************************************************/

static const JRegex lineIndexRegex(":([0-9]+)(?:-([0-9]+))?$");

void
JExtractFileAndLine
	(
	const JString&	str,
	JString*		fileName,
	JIndex*			startLineIndex,
	JIndex*			endLineIndex
	)
{
	*fileName = str;

	JStringIterator iter(fileName);
	iter.BeginMatch();
	if (iter.Next(lineIndexRegex))
		{
		const JStringMatch& m = iter.GetLastMatch();

		JBoolean ok = m.GetSubstring(1).ConvertToUInt(startLineIndex);
		assert( ok );

		const JCharacterRange endRange = m.GetCharacterRange(2);
		if (endLineIndex != nullptr && !endRange.IsEmpty())
			{
			ok = m.GetSubstring(2).ConvertToUInt(endLineIndex);
			assert( ok );
			}
		else if (endLineIndex != nullptr)
			{
			*endLineIndex = *startLineIndex;
			}

		*fileName = iter.FinishMatch().GetString();
		}
	else
		{
		*startLineIndex = 0;
		if (endLineIndex != nullptr)
			{
			*endLineIndex = 0;
			}
		}
}

/******************************************************************************
 JCombineRootAndSuffix

	Combines the root and suffix with a period between them.

 ******************************************************************************/

JString
JCombineRootAndSuffix
	(
	const JString&		root,
	const JUtf8Byte*	suffix
	)
{
	JString name = root;
	if (suffix[0] != '.')
		{
		name.Append(".");
		}
	name.Append(suffix);
	return name;
}

/******************************************************************************
 JSplitRootAndSuffix

	The root is everything before the last period.  The suffix is everything
	after the last period.  [0-9]+ does not count as a suffix.  Neither does
	starting or ending in a period.

	Returns kJTrue if *suffix is not empty.

 ******************************************************************************/

static const JRegex suffixPattern = "\\.(.*[^0-9].*)$";

JBoolean
JSplitRootAndSuffix
	(
	const JString&	name,
	JString*		root,
	JString*		suffix
	)
{
	*root = name;
	suffix->Clear();

	JStringIterator iter(root, kJIteratorStartAtEnd);
	if (iter.Prev(suffixPattern) && !iter.AtBeginning())
		{
		const JStringMatch& m = iter.GetLastMatch();

		*suffix = m.GetSubstring(1);
		iter.RemoveAllNext();
		}

	return !suffix->IsEmpty();
}

/******************************************************************************
 File name <-> URL

	JURLToFileName() does not accept URLs without a host name because the
	whole point of using URLs is that one doesn't know if the source and
	target machines will be the same.

 ******************************************************************************/

JString
JFileNameToURL
	(
	const JString& fileName
	)
{
	assert( JIsAbsolutePath(fileName) );

	JString host, file;
	if (!JTranslateLocalToRemote(fileName, &host, &file))
		{
		host = JGetHostName();
		file = fileName;
		}

	JString url("file://", 0);
	url += host;
	url += file;
	return url;
}

JBoolean
JURLToFileName
	(
	const JString&	url,
	JString*		fileName
	)
{
	JString s(url);

	JStringIterator iter(&s);
	if (iter.Next("://"))
		{
		iter.RemoveAllPrev();

		iter.BeginMatch();
		if (!iter.Next("/") || iter.GetPrevCharacterIndex() == 1)
			{
			return kJFalse;
			}
		const JStringMatch& m1    = iter.FinishMatch();
		const JString urlHostName = m1.GetString();

		iter.SkipPrev();
		iter.BeginMatch();
		iter.MoveTo(kJIteratorStartAtEnd, 0);
		const JStringMatch& m2    = iter.FinishMatch();
		const JString urlFileName = m2.GetString();

		if (urlHostName == JGetHostName())
			{
			*fileName = urlFileName;
			return kJTrue;
			}
		else
			{
			return JTranslateRemoteToLocal(urlHostName, urlFileName, fileName);
			}
		}
	else
		{
		fileName->Clear();
		return kJFalse;
		}
}

/******************************************************************************
 JFOpen

	Wrapper for fopen() that returns JError.

 ******************************************************************************/

JError
JFOpen
	(
	const JString&		fileName,
	const JUtf8Byte*	mode,
	FILE**				stream
	)
{
	jclear_errno();
	*stream = fopen(fileName.GetBytes(), mode);
	if (*stream != nullptr)
		{
		return JNoError();
		}

	const int err = jerrno();
	if (err == EINVAL)
		{
		return JInvalidOpenMode(fileName);
		}
	else if (err == EEXIST)
		{
		return JDirEntryAlreadyExists(fileName);
		}
	else if (err == EISDIR)
		{
		return JIsADirectory();
		}
	else if (err == EACCES || err == ETXTBSY)
		{
		return JAccessDenied(fileName);
		}
	else if (err == EFAULT)
		{
		return JSegFault();
		}
	else if (err == ENAMETOOLONG)
		{
		return JNameTooLong();
		}
	else if (err == ENOENT)
		{
		return JBadPath(fileName);
		}
	else if (err == ENOTDIR)
		{
		return JComponentNotDirectory(fileName);
		}
	else if (err == EMFILE || err == ENFILE)
		{
		return JTooManyDescriptorsOpen();
		}
	else if (err == ENOMEM)
		{
		return JNoKernelMemory();
		}
	else if (err == EROFS)
		{
		return JFileSystemReadOnly();
		}
	else if (err == ELOOP)
		{
		return JPathContainsLoop(fileName);
		}
	else if (err == ENOSPC)
		{
		return JFileSystemFull();
		}
	else
		{
		return JUnexpectedError(err);
		}
}
