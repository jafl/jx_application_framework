/******************************************************************************
 jFileUtil.h

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_jFileUtil
#define _H_jFileUtil

#include "JString.h"	// need Case
#include "jDirUtil.h"	// for convenience

class JError;
class JString;
class JProcess;

bool	JFileExists(const JString& fileName);
bool	JFileReadable(const JString& fileName);
bool	JFileWritable(const JString& fileName);
bool	JFileExecutable(const JString& fileName);

bool	JGetFileLength(const JString& name, JSize* size, JError* err = nullptr);
JString	JPrintFileSize(const JSize size);

bool	JRenameFile(const JString& oldName, const JString& newName,
					const bool forceReplace = false, JError* err = nullptr);
bool	JRemoveFile(const JString& fileName, JError* err = nullptr);
bool	JKillFile(const JString& fileName);

bool	JCreateTempFile(const JString* path, const JString* prefix,
						JString* fullName, JError* err = nullptr);

bool	JUncompressFile(const JString& origFileName, JString* newFileName,
						const JString* dirName = nullptr,
						JProcess** process = nullptr,
						JError* err = nullptr);

bool	JFOpen(const JString& fileName, const JUtf8Byte* mode, FILE** stream, JError* err = nullptr);

void	JExtractFileAndLine(const JString& str,
							JString* fileName, JIndex* startLineIndex,
							JIndex* endLineIndex = nullptr);

JString	JCombineRootAndSuffix(const JString& root, const JUtf8Byte* suffix);
JString	JCombineRootAndSuffix(const JString& root, const JString& suffix);
bool	JSplitRootAndSuffix(const JString& name, JString* root, JString* suffix);

JString	JFileNameToURL(const JString& fileName);
bool	JURLToFileName(const JString& url, JString* fileName);


inline bool
JCreateTempFile
	(
	JString*	fullName,
	JError*		err = nullptr
	)
{
	return JCreateTempFile(nullptr, nullptr, fullName, err);
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
