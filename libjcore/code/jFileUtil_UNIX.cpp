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

	Returns true if the specified file exists.

 ******************************************************************************/

bool
JFileExists
	(
	const JString& fileName
	)
{
	ACE_stat info;
	return (ACE_OS::lstat(fileName.GetBytes(), &info) == 0 &&
			ACE_OS::stat( fileName.GetBytes(), &info) == 0 &&
			S_ISREG(info.st_mode));
}

/******************************************************************************
 JFileReadable

	Returns true if the specified file can be read from.

 ******************************************************************************/

bool
JFileReadable
	(
	const JString& fileName
	)
{
	return (JFileExists(fileName) &&
			(getuid() == 0 || access(fileName.GetBytes(), R_OK) == 0));
}

/******************************************************************************
 JFileWritable

	Returns true if the specified file can be written to.

 ******************************************************************************/

bool
JFileWritable
	(
	const JString& fileName
	)
{
	return (JFileExists(fileName) &&
			(getuid() == 0 || access(fileName.GetBytes(), W_OK) == 0));
}

/******************************************************************************
 JFileExecutable

	Returns true if the specified file can be executed.

	Readability is not checked, because this is only an issue for scripts,
	and I can't tell the difference between a script and a binary.

 ******************************************************************************/

bool
JFileExecutable
	(
	const JString& fileName
	)
{
	if (getuid() == 0)
	{
		ACE_stat stbuf;
		return (ACE_OS::stat(fileName.GetBytes(), &stbuf) == 0 &&
				(stbuf.st_mode & S_IXUSR) != 0);
	}
	else
	{
		return (JFileExists(fileName) &&
				access(fileName.GetBytes(), X_OK) == 0);
	}
}

/******************************************************************************
 JGetFileLength

	Sets *size to the the length of the specified file.
	Can return JDirEntryDoesNotExist.

 ******************************************************************************/

bool
JGetFileLength
	(
	const JString&	name,
	JSize*			size,
	JError*			err
	)
{
	ACE_stat info;
	if (ACE_OS::stat(name.GetBytes(), &info) == 0)
	{
		*size = info.st_size;
		if (err != nullptr)
		{
			*err = JNoError();
		}
		return true;
	}

	*size = 0;

	if (err != nullptr)
	{
		const int e = jerrno();
		if (e == ENOENT)
		{
			*err = JDirEntryDoesNotExist(name);
		}
		else
		{
			*err = JUnexpectedError(e);
		}
	}
	return false;
}

/******************************************************************************
 JRemoveFile

	Removes the specified file.

	Can return JSegFault, JAccessDenied, JNameTooLong, JBadPath,
	JComponentNotDirectory, JTriedToRemoveDirectory, JNoKernelMemory,
	JFileSystemReadOnly.

 ******************************************************************************/

bool
JRemoveFile
	(
	const JString&	fileName,
	JError*			err
	)
{
	jclear_errno();
	if (remove(fileName.GetBytes()) == 0)
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
		if (e == EFAULT)
		{
			*err = JSegFault();
		}
		else if (e == EACCES || e == EPERM)
		{
			*err = JAccessDenied(fileName);
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
		else if (e == EISDIR)
		{
			*err = JTriedToRemoveDirectory();
		}
		else if (e == ENOMEM)
		{
			*err = JNoKernelMemory();
		}
		else if (e == EROFS)
		{
			*err = JFileSystemReadOnly();
		}
		else
		{
			*err = JUnexpectedError(e);
		}
	}
	return false;
}

/******************************************************************************
 JKillFile

	Tries as hard as it can to delete the file.
	Returns true if successful.

 ******************************************************************************/

bool
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
		return false;
	}
}

/******************************************************************************
 JCreateTempFile

	Creates a unique file in the specified directory.
	If path is empty, it uses the system's scratch directory.
	If prefix is empty, uses temp_file_.

 ******************************************************************************/

bool
JCreateTempFile
	(
	const JString*	path,
	const JString*	prefix,
	JString*		fullName,
	JError*			err
	)
{
	// inside function to ensure initialization
	static const JString theTmpDirForError("/tmp");
	static const JString theTmpFilePrefix("temp_file_");
	static const JString theTmpFileTemplate("XXXXXX");

	JString p;
	if (!JString::IsEmpty(path))
	{
		p = *path;
	}
	else if (!JGetTempDirectory(&p))
	{
		if (err != nullptr)
		{
			*err = JDirEntryDoesNotExist(theTmpDirForError);
		}
		return false;
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
		if (err != nullptr)
		{
			*err = JNoError();
		}
		return true;
	}

	fullName->Clear();
	jdelete [] s;

	// EINVAL counts as unexpected

	if (err != nullptr)
	{
		const int e = jerrno();
		if (e == EEXIST)
		{
			*err = JAccessDenied(p);
		}
		else
		{
			*err = JUnexpectedError(e);
		}
	}
	return false;
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

	JStringIterator iter(dirName, JStringIterator::kStartAtEnd);
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

bool
JUncompressFile
	(
	const JString&	origFileName,
	JString*		newFileName,
	const JString&	dirName,
	JProcess**		process,
	JError*			err
	)
{
	// generate a file name if one is not provided

	if (newFileName->IsEmpty())
	{
		if (!JCreateTempFile(&dirName, nullptr, newFileName, err))
		{
			return false;
		}
	}
	else if (!dirName.IsEmpty())
	{
		*newFileName = JCombinePathAndName(dirName, *newFileName);
	}

	// construct the command

	JString cmd("gunzip --to-stdout ");
	cmd += JPrepArgForExec(origFileName);
	cmd += " >| ";
	cmd += JPrepArgForExec(*newFileName);

	cmd = JString("/bin/sh -c ") + JPrepArgForExec(cmd);

	// run the command

	JError e = JNoError();
	if (process != nullptr)
	{
		e = JProcess::Create(process, cmd);
	}
	else
	{
		JString errText;
		e = JRunProgram(cmd, &errText);
	}

	if (err != nullptr)
	{
		*err = e;
	}
	return e.OK();
}
