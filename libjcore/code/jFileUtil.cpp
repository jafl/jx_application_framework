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
#include "JStdError.h"
#include "jMountUtil.h"
#include "jSysUtil.h"
#include "jErrno.h"
#include <limits.h>
#include "jAssert.h"

/******************************************************************************
 JRenameFile

	Renames the specified file.

 ******************************************************************************/

bool
JRenameFile
	(
	const JString&	oldName,
	const JString&	newName,
	const bool		forceReplace,
	JError*			err
	)
{
	if (forceReplace)
	{
		JRemoveFile(newName);	// ignore errors
	}
	return JRenameDirEntry(oldName, newName, err);
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

	If fullName doesn't contain a directory separator, it returns false
	and *path is the result of JGetCurrentDirectory().

 ******************************************************************************/

bool
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
		return false;
	}

	JStringIterator iter(pathAndName, JStringIterator::kStartAtEnd);
	iter.BeginMatch();
	if (iter.Prev(ACE_DIRECTORY_SEPARATOR_STR))
	{
		*name = iter.FinishMatch().GetString();

		iter.SkipNext();	// include separator
		iter.BeginMatch();
		iter.MoveTo(JStringIterator::kStartAtBeginning, 0);
		*path = iter.FinishMatch().GetString();

		JCleanPath(path);
		return true;
	}
	else
	{
		*path = JGetCurrentDirectory();
		*name = pathAndName;
		return false;
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
		return JString(size);
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

		bool ok = m.GetSubstring(1).ConvertToUInt(startLineIndex);
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

	Returns true if *suffix is not empty.

 ******************************************************************************/

static const JRegex suffixPattern("\\.([^.]*[^.0-9][^.]*)$");

bool
JSplitRootAndSuffix
	(
	const JString&	name,
	JString*		root,
	JString*		suffix
	)
{
	*root = name;
	suffix->Clear();

	JStringIterator iter(root, JStringIterator::kStartAtEnd);
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

	JString url("file://");
	url += host;
	url += file;
	return url;
}

bool
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
			return false;
		}
		const JStringMatch& m1    = iter.FinishMatch();
		const JString urlHostName = m1.GetString();

		iter.SkipPrev();
		iter.BeginMatch();
		iter.MoveTo(JStringIterator::kStartAtEnd, 0);
		const JStringMatch& m2    = iter.FinishMatch();
		const JString urlFileName = m2.GetString();

		if (urlHostName == JGetHostName())
		{
			*fileName = urlFileName;
			return true;
		}
		else
		{
			return JTranslateRemoteToLocal(urlHostName, urlFileName, fileName);
		}
	}
	else
	{
		fileName->Clear();
		return false;
	}
}

/******************************************************************************
 JFOpen

	Wrapper for fopen() that returns JError.

 ******************************************************************************/

bool
JFOpen
	(
	const JString&		fileName,
	const JUtf8Byte*	mode,
	FILE**				stream,
	JError*				err
	)
{
	jclear_errno();
	*stream = fopen(fileName.GetBytes(), mode);
	if (*stream != nullptr)
	{
		if (err != nullptr)
		{
			*err = JNoError();
		}
		return true;
	}

	if (err != nullptr)
	{
		const int e = jerrno();
		if (e == EINVAL)
		{
			*err = JInvalidOpenMode(fileName);
		}
		else if (e == EEXIST)
		{
			*err = JDirEntryAlreadyExists(fileName);
		}
		else if (e == EISDIR)
		{
			*err = JIsADirectory();
		}
		else if (e == EACCES || e == ETXTBSY)
		{
			*err = JAccessDenied(fileName);
		}
		else if (e == EFAULT)
		{
			*err = JSegFault();
		}
		else if (e == ENAMETOOLONG)
		{
			*err = JNameTooLong();
		}
		else if (e == ENOENT)
		{
			*err = JBadPath(fileName);
		}
		else if (e == ENOTDIR)
		{
			*err = JComponentNotDirectory(fileName);
		}
		else if (e == EMFILE || e == ENFILE)
		{
			*err = JTooManyDescriptorsOpen();
		}
		else if (e == ENOMEM)
		{
			*err = JNoKernelMemory();
		}
		else if (e == EROFS)
		{
			*err = JFileSystemReadOnly();
		}
		else if (e == ELOOP)
		{
			*err = JPathContainsLoop(fileName);
		}
		else if (e == ENOSPC)
		{
			*err = JFileSystemFull();
		}
		else
		{
			*err = JUnexpectedError(e);
		}
	}
	return false;
}
