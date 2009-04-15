/******************************************************************************
 jDirUtil.h

	Utility functions for dealing with directories.

	Copyright © 1996 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_jDirUtil
#define _H_jDirUtil

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JStdError.h>
#include <time.h>

#if defined _MSC_VER
	#include <unistd.h>
#else
	#include <sys/types.h>
#endif

#include <sys/stat.h>
#include <ace/OS.h>
#include <jFileUtil.h>	// for convenience

class JString;
class JProgressDisplay;

JBoolean	JNameUsed(const JCharacter* name);
JBoolean	JSameDirEntry(const JCharacter* name1, const JCharacter* name2);
JError		JRenameDirEntry(const JCharacter* oldName, const JCharacter* newName);
JError		JCreateSymbolicLink(const JCharacter* src, const JCharacter* dest);
JError		JGetSymbolicLinkTarget(const JCharacter* linkFullName, JString* targetFullName);

JError		JGetModificationTime(const JCharacter* name, time_t* modTime);
JError		JGetPermissions(const JCharacter* name, mode_t* perms);
JError		JSetPermissions(const JCharacter* name, const mode_t perms);
JString		JGetPermissionsString(const mode_t mode);
JError		JGetOwnerID(const JCharacter* name, uid_t* uid);
JError		JGetOwnerGroup(const JCharacter* name, gid_t* gid);

JBoolean	JDirectoryExists(const JCharacter* dirName);
JBoolean	JDirectoryReadable(const JCharacter* dirName);
JBoolean	JDirectoryWritable(const JCharacter* dirName);
JBoolean	JCanEnterDirectory(const JCharacter* dirName);

JError		JCreateDirectory(const JCharacter* dirName);
JError		JCreateDirectory(const JCharacter* dirName, const mode_t mode);
JError		JRenameDirectory(const JCharacter* oldName, const JCharacter* newName);
JError		JChangeDirectory(const JCharacter* dirName);
JError		JRemoveDirectory(const JCharacter* dirName);
JBoolean	JKillDirectory(const JCharacter* dirName);

JString		JGetCurrentDirectory();
JBoolean	JGetHomeDirectory(JString* homeDir);
JBoolean	JGetHomeDirectory(const JCharacter* user, JString* homeDir);
JBoolean	JGetPrefsDirectory(JString* prefsDir);
JBoolean	JGetPrefsDirectory(const JCharacter* user, JString* prefsDir);
JBoolean	JGetTempDirectory(JString* tempDir);
JBoolean	JGetTrueName(const JCharacter* name, JString* trueName);
JBoolean	JSearchSubdirs(const JCharacter* startPath, const JCharacter* name,
						   const JBoolean isFile, const JBoolean caseSensitive,
						   JString* path, JString* newName = NULL,
						   JProgressDisplay* pg = NULL,
						   JBoolean* userCancelled = NULL);

JString		JGetUniqueDirEntryName(const JCharacter* path, const JCharacter* namePrefix,
								   const JCharacter* nameSuffix = NULL,
								   const JIndex startIndex = 1);
JError		JCreateTempDirectory(const JCharacter* path, const JCharacter* prefix,
								 JString* fullName);

JString		JCombinePathAndName(const JCharacter* path, const JCharacter* name);
JBoolean	JSplitPathAndName(const JCharacter* fullName, JString* path, JString* name);
void		JAppendDirSeparator(JString* dirName);
void		JStripTrailingDirSeparator(JString* dirName);
void		JCleanPath(JString* path);
JString		JGetRootDirectory();
JBoolean	JIsRootDirectory(const JCharacter* dirName);
JBoolean	JIsRelativePath(const JCharacter* path);

JBoolean	JConvertToAbsolutePath(const JCharacter* path, const JCharacter* base,
								   JString* result);
JString		JConvertToRelativePath(const JCharacter* path, const JCharacter* base);

JBoolean	JExpandHomeDirShortcut(const JCharacter* path, JString* result,
								   JString* homeDir = NULL, JSize* homeLength = NULL);
JString		JConvertToHomeDirShortcut(const JCharacter* path);
JString		JGetClosestDirectory(const JCharacter* origDirName,
								 const JBoolean requireWrite = kJFalse,
								 const JCharacter* basePath = NULL);

#ifdef WIN32
JBoolean	JGetDirectoryFromCSIDL(const int folderCSIDL, JString* path);
#endif


inline JError
JCreateTempDirectory
	(
	JString* fullName
	)
{
	return JCreateTempDirectory(NULL, NULL, fullName);
}

#endif
