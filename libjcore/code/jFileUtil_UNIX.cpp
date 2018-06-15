/******************************************************************************
 jFileUtil_UNIX.cpp

	File utilities implemented for the UNIX System.

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "jFileUtil.h"
#include "jDirUtil.h"
#include "JStdError.h"
#include "JStringIterator.h"
#include "JProcess.h"
#include <unistd.h>
#include <sys/stat.h>
#include <ace/OS_NS_sys_stat.h>
#include <stdio.h>
#include "jErrno.h"
#include "jAssert.h"

/******************************************************************************
 JFileExists

	Returns kJTrue if the specified file exists.

 ******************************************************************************/

JBoolean
JFileExists
	(
	const JString& fileName
	)
{
	ACE_stat info;
	return JI2B(
			ACE_OS::lstat(fileName.GetBytes(), &info) == 0 &&
			ACE_OS::stat( fileName.GetBytes(), &info) == 0 &&
			S_ISREG(info.st_mode) );
}

/******************************************************************************
 JFileReadable

	Returns kJTrue if the specified file can be read from.

 ******************************************************************************/

JBoolean
JFileReadable
	(
	const JString& fileName
	)
{
	return JI2B(JFileExists(fileName) &&
				(getuid() == 0 || access(fileName.GetBytes(), R_OK) == 0));
}

/******************************************************************************
 JFileWritable

	Returns kJTrue if the specified file can be written to.

 ******************************************************************************/

JBoolean
JFileWritable
	(
	const JString& fileName
	)
{
	return JI2B(JFileExists(fileName) &&
				(getuid() == 0 || access(fileName.GetBytes(), W_OK) == 0));
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
	const JString& fileName
	)
{
	if (getuid() == 0)
		{
		ACE_stat stbuf;
		return JI2B( ACE_OS::stat(fileName.GetBytes(), &stbuf) == 0 &&
					 (stbuf.st_mode & S_IXUSR) != 0 );
		}
	else
		{
		return JI2B(JFileExists(fileName) &&
					access(fileName.GetBytes(), X_OK) == 0);
		}
}

/******************************************************************************
 JGetFileLength

	Sets *size to the the length of the specified file.
	Can return JDirEntryDoesNotExist.

 ******************************************************************************/

JError
JGetFileLength
	(
	const JString&	name,
	JSize*			size
	)
{
	ACE_stat info;
	if (ACE_OS::stat(name.GetBytes(), &info) == 0)
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

	Can return JSegFault, JAccessDenied, JNameTooLong, JBadPath,
	JComponentNotDirectory, JTriedToRemoveDirectory, JNoKernelMemory,
	JFileSystemReadOnly.

 ******************************************************************************/

JError
JRemoveFile
	(
	const JString& fileName
	)
{
	jclear_errno();
	if (remove(fileName.GetBytes()) == 0)
		{
		return JNoError();
		}

	const int err = jerrno();
	if (err == EFAULT)
		{
		return JSegFault();
		}
	else if (err == EACCES || err == EPERM)
		{
		return JAccessDenied(fileName);
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
	else if (err == EISDIR)
		{
		return JTriedToRemoveDirectory();
		}
	else if (err == ENOMEM)
		{
		return JNoKernelMemory();
		}
	else if (err == EROFS)
		{
		return JFileSystemReadOnly();
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
	const JString& fileName
	)
{
	const JUtf8Byte* argv[] = {"rm", "-f", fileName.GetBytes(), nullptr};
	const JError err = JExecute(argv, sizeof(argv), nullptr,
								kJIgnoreConnection, nullptr,
								kJIgnoreConnection, nullptr,
								kJTossOutput,       nullptr);
	if (err.OK())
		{
		return !JNameUsed(fileName);
		}
	else
		{
		return kJFalse;
		}
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
	const JString*	path,
	const JString*	prefix,
	JString*		fullName
	)
{
	// inside function to ensure initialization
	static const JString theTmpDirForError("/tmp", kJFalse);
	static const JString theTmpFilePrefix("temp_file_", kJFalse);
	static const JString theTmpFileTemplate("XXXXXX", kJFalse);

	JString p;
	if (!JString::IsEmpty(path))
		{
		p = *path;
		}
	else if (!JGetTempDirectory(&p))
		{
		return JDirEntryDoesNotExist(theTmpDirForError);
		}

	if (!JString::IsEmpty(prefix))
		{
		p = JCombinePathAndName(p, *prefix);
		}
	else
		{
		p = JCombinePathAndName(p, theTmpFilePrefix);
		}

	p      += theTmpFileTemplate;
	char* s = p.AllocateBytes();

	jclear_errno();
	int fd = mkstemp(s);
	if (fd != -1)
		{
		close(fd);
		*fullName = s;
		jdelete [] s;
		return JNoError();
		}

	fullName->Clear();
	jdelete [] s;

	// EINVAL counts as unexpected

	const int err = jerrno();
	if (err == EEXIST)
		{
		return JAccessDenied(p);
		}
	else
		{
		return JUnexpectedError(err);
		}
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

	JStringIterator iter(dirName, kJIteratorStartAtEnd);
	JUtf8Character c;
	while (iter.Prev(&c))
		{
		if (dirName->GetCharacterCount() > 1 && c == ACE_DIRECTORY_SEPARATOR_CHAR)
			{
			iter.RemoveNext();
			}
		else
			{
			break;
			}
		}
}

/******************************************************************************
 JUncompressFile

	Uncompresses the file named origFileName.

	If newFileName is empty, we generate a unique name and return it.
	If dirName is not empty, we place the new file there.
	If process is not nullptr,  we return the one we create without blocking.
		Otherwise, we block until the process finishes.

 ******************************************************************************/

JError
JUncompressFile
	(
	const JString&	origFileName,
	JString*		newFileName,
	const JString&	dirName,
	JProcess**		process
	)
{
	// generate a file name if one is not provided

	if (newFileName->IsEmpty())
		{
		const JError err = JCreateTempFile(&dirName, nullptr, newFileName);
		if (!err.OK())
			{
			return err;
			}
		}
	else if (!dirName.IsEmpty())
		{
		*newFileName = JCombinePathAndName(dirName, *newFileName);
		}

	// construct the command

	JString cmd("gunzip --to-stdout ", 0);
	cmd += JPrepArgForExec(origFileName);
	cmd += " >| ";
	cmd += JPrepArgForExec(*newFileName);

	cmd = JString("/bin/sh -c ", 0) + JPrepArgForExec(cmd);

	// run the command

	if (process != nullptr)
		{
		return JProcess::Create(process, cmd);
		}
	else
		{
		JString errText;
		return JRunProgram(cmd, &errText);
		}
}
