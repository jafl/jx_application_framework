/******************************************************************************
 jFileUtil.cpp

	Useful functions for dealing with files.

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <jFileUtil.h>
#include <jStreamUtil.h>
#include <jFStreamUtil.h>
#include <JRegex.h>
#include <jMountUtil.h>
#include <jSysUtil.h>
#include <jErrno.h>
#include <jMissingProto.h>
#include <limits.h>
#include <jAssert.h>

/******************************************************************************
 JRenameFile

	Renames the specified file.

 ******************************************************************************/

JError
JRenameFile
	(
	const JCharacter* oldName,
	const JCharacter* newName
	)
{
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
	const JCharacter* path,
	const JCharacter* name
	)
{
	assert( !JStringEmpty(path) );
	assert( !JStringEmpty(name) );

	JString file = path;
	if (file.GetLastCharacter() != ACE_DIRECTORY_SEPARATOR_CHAR &&
		name[0] != ACE_DIRECTORY_SEPARATOR_CHAR)
		{
		file.AppendCharacter(ACE_DIRECTORY_SEPARATOR_CHAR);
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
	const JCharacter*	fullName,
	JString*			path,
	JString*			name
	)
{
	assert( !JStringEmpty(fullName) );

	JString pathAndName = fullName;
	if (pathAndName.GetLastCharacter() == ACE_DIRECTORY_SEPARATOR_CHAR)
		{
		JStripTrailingDirSeparator(&pathAndName);
		}

	JIndex i;
	if (pathAndName.LocateLastSubstring(ACE_DIRECTORY_SEPARATOR_STR, &i) &&
		i < pathAndName.GetLength())
		{
		*path = pathAndName.GetSubstring(1,i);
		*name = pathAndName.GetSubstring(i+1, pathAndName.GetLength());

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
		dirName->AppendCharacter(ACE_DIRECTORY_SEPARATOR_CHAR);
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
		return JString(size, JString::kBase10);
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
	const JCharacter*	fileName,
	const JCharacter*	searchStr,
	const JBoolean		caseSensitive,
	JIndex*				lineIndex
	)
{
	ifstream input(fileName);

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

	For backward compatibility, endLineIndex can be NULL.

 ******************************************************************************/

void
JExtractFileAndLine
	(
	const JCharacter*	str,
	JString*			fileName,
	JIndex*				startLineIndex,
	JIndex*				endLineIndex
	)
{
	static JRegex lineIndexRegex(":([0-9]+)(-([0-9]+))?$");

	*fileName = str;

	JArray<JIndexRange> matchList;
	if (lineIndexRegex.Match(*fileName, &matchList))
		{
		JString s   = fileName->GetSubstring(matchList.GetElement(2));
		JBoolean ok = s.ConvertToUInt(startLineIndex);
		assert( ok );

		const JIndexRange endRange = matchList.GetElement(4);
		if (endLineIndex != NULL && !endRange.IsEmpty())
			{
			s  = fileName->GetSubstring(endRange);
			ok = s.ConvertToUInt(endLineIndex);
			assert( ok );
			}
		else if (endLineIndex != NULL)
			{
			*endLineIndex = *startLineIndex;
			}

		fileName->RemoveSubstring(matchList.GetElement(1));
		}
	else
		{
		*startLineIndex = 0;
		if (endLineIndex != NULL)
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
	const JCharacter* root,
	const JCharacter* suffix
	)
{
	JString name = root;
	if (suffix[0] != '.')
		{
		name.AppendCharacter('.');
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

JBoolean
JSplitRootAndSuffix
	(
	const JCharacter*	name,
	JString*			root,
	JString*			suffix
	)
{
	*root = name;
	suffix->Clear();

	JIndex dotIndex;
	if (root->LocateLastSubstring(".", &dotIndex) && dotIndex > 1)
		{
		const JSize length = root->GetLength();
		if (dotIndex < length)
			{
			JBoolean isSuffix = kJFalse;
			for (JIndex i=dotIndex+1; i<=length; i++)
				{
				if (!isdigit(root->GetCharacter(i)))
					{
					isSuffix = kJTrue;
					break;
					}
				}

			if (isSuffix)
				{
				*suffix = root->GetSubstring(dotIndex+1, length);
				root->RemoveSubstring(dotIndex, length);
				}
			}
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
	const JCharacter* fileName
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

JBoolean
JURLToFileName
	(
	const JCharacter*	url,
	JString*			fileName
	)
{
	JString s(url);

	JIndex index;
	if (s.LocateSubstring("://", &index))
		{
		s.RemoveSubstring(1, index+2);

		if (!s.LocateSubstring("/", &index) || index == 1)
			{
			return kJFalse;
			}
		const JString urlHostName = s.GetSubstring(1, index-1);
		const JString urlFileName = s.GetSubstring(index, s.GetLength());

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
	const JCharacter*	fileName,
	const JCharacter*	mode,
	FILE**				stream
	)
{
	jclear_errno();
	*stream = fopen(fileName, mode);
	if (*stream != NULL)
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
