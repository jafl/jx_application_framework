/******************************************************************************
 jFileUtil_Win32.cpp

	File utilities implemented for the Windows System.

	Copyright © 2005 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <jFileUtil.h>
#include <JString.h>
#include <jSysUtil.h>
#include <jFStreamUtil.h>
#include <ace/OS.h>
#include <jMissingProto.h>
#include <JStdError.h>
#include <jErrno.h>
#include <jAssert.h>

/*******************************************************************************
 JFileExists

	Returns kJTrue if the specified file exists.

 *******************************************************************************/

JBoolean
JFileExists
	(
	const JCharacter* fileName
	)
{
	JString s = fileName;
	JStripTrailingDirSeparator(&s);

	ACE_stat info;
	return JI2B(ACE_OS::stat(s, &info) == 0 &&
				S_ISREG(info.st_mode));
}

/******************************************************************************
 JFileReadable

	Returns kJTrue if the specified file can be read from.

 ******************************************************************************/

JBoolean
JFileReadable
	(
	const JCharacter* fileName
	)
{
	return JI2B(JFileExists(fileName) &&
				(JUserIsAdmin() || _access(fileName, R_OK) == 0));
}

/******************************************************************************
 JFileWritable

	Returns kJTrue if the specified file can be written to.

 ******************************************************************************/

JBoolean
JFileWritable
	(
	const JCharacter* fileName
	)
{
	return JI2B(JFileExists(fileName) &&
				(JUserIsAdmin() || _access(fileName, W_OK) == 0));
}

/******************************************************************************
 JFileExecutable

	Returns kJTrue if the specified file can be executed.

	Readability is not checked, because this is only an issue for scripts,
	and I can't tell the difference between a script and a binary.

 ******************************************************************************/

JBoolean
JFileExecutable
	(
	const JCharacter* fileName
	)
{
	const JString s = fileName;
	return JI2B(s.EndsWith(".exe") || s.EndsWith(".com") || s.EndsWith(".bat"));
}

/******************************************************************************
 JGetFileLength

	Sets *size to the the length of the specified file.
	Can return JDirEntryDoesNotExist.

 ******************************************************************************/

JError
JGetFileLength
	(
	const JCharacter*	name,
	JSize*				size
	)
{
	JString s = name;
	JStripTrailingDirSeparator(&s);

	ACE_stat info;
	if (ACE_OS::stat(s, &info) == 0)
		{
		*size = info.st_size;
		return JNoError();
		}

	*size = 0;

	const int err = jerrno();
	if (err == ENOENT)
		{
		return JDirEntryDoesNotExist(name);
		}
	else
		{
		return JUnexpectedError(err);
		}
}

/******************************************************************************
 JRemoveFile

	Removes the specified file.

	Can return JAccessDenied, JBadPath.

 ******************************************************************************/

JError
JRemoveFile
	(
	const JCharacter* fileName
	)
{
	jclear_errno();
	if (remove(fileName) == 0)
		{
		return JNoError();
		}

	const int err = jerrno();
	if (err == EACCES)
		{
		return JAccessDenied(fileName);
		}
	else if (err == ENOENT)
		{
		return JBadPath(fileName);
		}
	else
		{
		return JUnexpectedError(err);
		}
}

/******************************************************************************
 JKillFile

	Tries as hard as it can to delete the file.
	Returns kJTrue if successful.

 ******************************************************************************/

JBoolean
JKillFile
	(
	const JCharacter* fileName
	)
{
	SetFileAttributes(fileName, FILE_ATTRIBUTE_NORMAL);
	_chmod(fileName, _S_IREAD | _S_IWRITE);

	return (JRemoveFile(fileName)).OK();
}

/******************************************************************************
 JCreateTempFile

	Creates a unique file in the specified directory.
	If path is empty, it uses the system's scratch directory.
	If prefix is empty, uses temp_file_.

 ******************************************************************************/

JError
JCreateTempFile
	(
	const JCharacter*	path,
	const JCharacter*	prefix,
	JString*			fullName
	)
{
	JString p;
	if (!JStringEmpty(path))
		{
		p = path;
		}
	else if (!JGetTempDirectory(&p))
		{
		return JDirEntryDoesNotExist("/tmp");
		}

	JString f;
	if (!JStringEmpty(prefix))
		{
		f = prefix;
		}
	else
		{
		f = "temp_file_";
		}

	*fullName = JGetUniqueDirEntryName(p, f);
	ofstream output(*fullName);
	return JNoError();
}

/******************************************************************************
 JStripTrailingDirSeparator

 ******************************************************************************/

void
JStripTrailingDirSeparator
	(
	JString* dirName
	)
{
	assert( !dirName->IsEmpty() );

	while (dirName->GetLength() > 3 &&
		   dirName->GetLastCharacter() == ACE_DIRECTORY_SEPARATOR_CHAR)
		{
		dirName->RemoveSubstring(dirName->GetLength(), dirName->GetLength());
		}
}
