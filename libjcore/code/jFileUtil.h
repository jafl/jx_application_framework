/******************************************************************************
 jFileUtil.h

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_jFileUtil
#define _H_jFileUtil

#include "JError.h"
#include "jTime.h"
#include <stdio.h>
#include "JString.h"	// need Case
#include "jDirUtil.h"	// for convenience

class JString;
class JProcess;

bool	JFileExists(const JString& fileName);
bool	JFileReadable(const JString& fileName);
bool	JFileWritable(const JString& fileName);
bool	JFileExecutable(const JString& fileName);

JError	JGetFileLength(const JString& name, JSize* size);
JString	JPrintFileSize(const JSize size);

JError	JRenameFile(const JString& oldName, const JString& newName,
					const bool forceReplace = false);
JError	JRemoveFile(const JString& fileName);
bool	JKillFile(const JString& fileName);

JError	JCreateTempFile(const JString* path, const JString* prefix,
						JString* fullName);

JError	JUncompressFile(const JString& origFileName, JString* newFileName,
						const JString* dirName = nullptr,
						JProcess** process = nullptr);

JError	JFOpen(const JString& fileName, const JUtf8Byte* mode, FILE** stream);

void	JExtractFileAndLine(const JString& str,
							JString* fileName, JIndex* startLineIndex,
							JIndex* endLineIndex = nullptr);

JString	JCombineRootAndSuffix(const JString& root, const JUtf8Byte* suffix);
JString	JCombineRootAndSuffix(const JString& root, const JString& suffix);
bool	JSplitRootAndSuffix(const JString& name, JString* root, JString* suffix);

JString	JFileNameToURL(const JString& fileName);
bool	JURLToFileName(const JString& url, JString* fileName);


inline JError
JCreateTempFile
	(
	JString* fullName
	)
{
	return JCreateTempFile(nullptr, nullptr, fullName);
}

inline JString
JCombineRootAndSuffix
	(
	const JString& root,
	const JString& suffix
	)
{
	return JCombineRootAndSuffix(root, suffix.GetBytes());
}

#endif
