/******************************************************************************
 jDirUtil.h

	Utility functions for dealing with directories.

	Copyright (C) 1996 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_jDirUtil
#define _H_jDirUtil

#include "JStdError.h"
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ace/Default_Constants.h>
#include "jFileUtil.h"	// for convenience

class JString;
class JProgressDisplay;
class JProcess;

JBoolean	JNameUsed(const JString& name);
JBoolean	JSameDirEntry(const JString& name1, const JString& name2);
JError		JRenameDirEntry(const JString& oldName, const JString& newName);
JError		JCreateSymbolicLink(const JString& src, const JString& dest);
JError		JGetSymbolicLinkTarget(const JString& linkFullName, JString* targetFullName);

JError		JGetModificationTime(const JString& name, time_t* modTime);
JError		JGetPermissions(const JString& name, mode_t* perms);
JError		JSetPermissions(const JString& name, const mode_t perms);
JString		JGetPermissionsString(const mode_t mode);
JError		JGetOwnerID(const JString& name, uid_t* uid);
JError		JGetOwnerGroup(const JString& name, gid_t* gid);
JError		JSetOwner(const JString& name, const uid_t uid, const gid_t gid);

JBoolean	JDirectoryExists(const JString& dirName);
JBoolean	JDirectoryReadable(const JString& dirName);
JBoolean	JDirectoryWritable(const JString& dirName);
JBoolean	JCanEnterDirectory(const JString& dirName);

JError		JCreateDirectory(const JString& dirName);
JError		JCreateDirectory(const JString& dirName, const mode_t mode);
JError		JRenameDirectory(const JString& oldName, const JString& newName);
JError		JChangeDirectory(const JString& dirName);
JError		JRemoveDirectory(const JString& dirName);
JBoolean	JKillDirectory(const JString& dirName, const JBoolean sync = kJTrue,
						   JProcess** p = nullptr);

JString		JGetCurrentDirectory();
JBoolean	JGetHomeDirectory(JString* homeDir);
JBoolean	JGetHomeDirectory(const JString& user, JString* homeDir);
JBoolean	JGetPrefsDirectory(JString* prefsDir);
JBoolean	JGetPrefsDirectory(const JString& user, JString* prefsDir);
JBoolean	JGetTempDirectory(JString* tempDir);
JBoolean	JGetTrueName(const JString& name, JString* trueName);
JBoolean	JSearchSubdirs(const JString& startPath, const JString& name,
						   const JBoolean isFile, const JBoolean caseSensitive,
						   JString* path, JString* newName = nullptr,
						   JProgressDisplay* pg = nullptr,
						   JBoolean* userCancelled = nullptr);

JString		JGetUniqueDirEntryName(const JString& path, const JString& namePrefix,
								   const JUtf8Byte* nameSuffix = nullptr,
								   const JIndex startIndex = 1);
JError		JCreateTempDirectory(const JString* path, const JString* prefix,
								 JString* fullName);

JString		JCombinePathAndName(const JString& path, const JString& name);
JBoolean	JSplitPathAndName(const JString& fullName, JString* path, JString* name);
void		JAppendDirSeparator(JString* dirName);
void		JStripTrailingDirSeparator(JString* dirName);
void		JCleanPath(JString* path);
JString		JGetRootDirectory();
JBoolean	JIsRootDirectory(const JString& dirName);
JBoolean	JIsAbsolutePath(const JString& path);
JBoolean	JIsRelativePath(const JString& path);

JBoolean	JConvertToAbsolutePath(const JString& path, const JString* base, JString* result);
JString		JConvertToRelativePath(const JString& path, const JString& base);

JBoolean	JExpandHomeDirShortcut(const JString& path, JString* result,
								   JString* homeDir = nullptr, JSize* homeLength = nullptr);
JString		JConvertToHomeDirShortcut(const JString& path);
JString		JGetClosestDirectory(const JString& origDirName,
								 const JBoolean requireWrite = kJFalse,
								 const JString* basePath = nullptr);

#ifdef WIN32
JBoolean	JGetDirectoryFromCSIDL(const int folderCSIDL, JString* path);
#endif


inline JError
JCreateTempDirectory
	(
	JString* fullName
	)
{
	return JCreateTempDirectory(nullptr, nullptr, fullName);
}

#endif
