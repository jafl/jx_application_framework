/******************************************************************************
 jFileUtil.h

	Interface for jFileUtil.cc

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_jFileUtil
#define _H_jFileUtil

#include <JError.h>
#include <jTime.h>
#include <stdio.h>
#include <jDirUtil.h>	// for convenience

class JString;
class JProcess;

JBoolean	JFileExists(const JString& fileName);
JBoolean	JFileReadable(const JString& fileName);
JBoolean	JFileWritable(const JString& fileName);
JBoolean	JFileExecutable(const JString& fileName);

JError		JGetFileLength(const JString& name, JSize* size);
JString		JPrintFileSize(const JSize size);

JError		JRenameFile(const JString& oldName, const JString& newName,
						const JBoolean forceReplace = kJFalse);
JError		JRemoveFile(const JString& fileName);
JBoolean	JKillFile(const JString& fileName);

JError		JCreateTempFile(const JString* path, const JString* prefix,
							JString* fullName);

JBoolean	JSearchFile(const JString& fileName, const JString& searchStr,
						const JBoolean caseSensitive, JIndex* lineIndex);

JError		JUncompressFile(const JString& origFileName, JString* newFileName,
							const JString* dirName = NULL,
							JProcess** process = NULL);

JError		JFOpen(const JString& fileName, const JUtf8Byte* mode, FILE** stream);

void		JExtractFileAndLine(const JString& str,
								JString* fileName, JIndex* startLineIndex,
								JIndex* endLineIndex = NULL);

JString		JCombineRootAndSuffix(const JString& root, const JUtf8Byte* suffix);
JString		JCombineRootAndSuffix(const JString& root, const JString& suffix);
JBoolean	JSplitRootAndSuffix(const JString& name, JString* root, JString* suffix);

JString		JFileNameToURL(const JString& fileName);
JBoolean	JURLToFileName(const JString& url, JString* fileName);


inline JError
JCreateTempFile
	(
	JString* fullName
	)
{
	return JCreateTempFile(NULL, NULL, fullName);
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
