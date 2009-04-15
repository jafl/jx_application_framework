/******************************************************************************
 jFileUtil.h

	Interface for jFileUtil.cc

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_jFileUtil
#define _H_jFileUtil

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JError.h>
#include <jTime.h>
#include <stdio.h>
#include <jDirUtil.h>	// for convenience

class JString;
class JProcess;

JBoolean	JFileExists(const JCharacter* fileName);
JBoolean	JFileReadable(const JCharacter* fileName);
JBoolean	JFileWritable(const JCharacter* fileName);
JBoolean	JFileExecutable(const JCharacter* fileName);

JError		JGetFileLength(const JCharacter* name, JSize* size);
JString		JPrintFileSize(const JSize size);

JError		JRenameFile(const JCharacter* oldName, const JCharacter* newName);
JError		JRemoveFile(const JCharacter* fileName);
JBoolean	JKillFile(const JCharacter* fileName);

JError		JCreateTempFile(const JCharacter* path, const JCharacter* prefix,
							JString* fullName);

JBoolean	JSearchFile(const JCharacter* fileName, const JCharacter* searchStr,
						const JBoolean caseSensitive, JIndex* lineIndex);

JError		JUncompressFile(const JCharacter* origFileName, JString* newFileName,
							const JCharacter* dirName = NULL,
							JProcess** process = NULL);

JError		JFOpen(const JCharacter* fileName, const JCharacter* mode, FILE** stream);

void		JExtractFileAndLine(const JCharacter* str,
								JString* fileName, JIndex* startLineIndex,
								JIndex* endLineIndex = NULL);

JString		JCombineRootAndSuffix(const JCharacter* root, const JCharacter* suffix);
JBoolean	JSplitRootAndSuffix(const JCharacter* name, JString* root, JString* suffix);

JString		JFileNameToURL(const JCharacter* fileName);
JBoolean	JURLToFileName(const JCharacter* url, JString* fileName);


inline JError
JCreateTempFile
	(
	JString* fullName
	)
{
	return JCreateTempFile(NULL, NULL, fullName);
}

#endif
