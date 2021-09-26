/******************************************************************************
 jDirUtil.h

	Utility functions for dealing with directories.

	Copyright (C) 1996 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_jDirUtil
#define _H_jDirUtil

#include "jx-af/jcore/JStdError.h"
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ace/Default_Constants.h>
#include "jx-af/jcore/JString.h"	// need Case
#include "jx-af/jcore/jFileUtil.h"	// for convenience

class JProgressDisplay;
class JProcess;

bool	JNameUsed(const JString& name);
bool	JSameDirEntry(const JString& name1, const JString& name2);
JError	JRenameDirEntry(const JString& oldName, const JString& newName);
JError	JCreateSymbolicLink(const JString& src, const JString& dest);
JError	JGetSymbolicLinkTarget(const JString& linkFullName, JString* targetFullName);

JError	JGetModificationTime(const JString& name, time_t* modTime);
JError	JGetPermissions(const JString& name, mode_t* perms);
JError	JSetPermissions(const JString& name, const mode_t perms);
JString	JGetPermissionsString(const mode_t mode);
JError	JGetOwnerID(const JString& name, uid_t* uid);
JError	JGetOwnerGroup(const JString& name, gid_t* gid);
JError	JSetOwner(const JString& name, const uid_t uid, const gid_t gid);

bool	JDirectoryExists(const JString& dirName);
bool	JDirectoryReadable(const JString& dirName);
bool	JDirectoryWritable(const JString& dirName);
bool	JCanEnterDirectory(const JString& dirName);

JError	JCreateDirectory(const JString& dirName);
JError	JCreateDirectory(const JString& dirName, const mode_t mode);
JError	JRenameDirectory(const JString& oldName, const JString& newName);
JError	JChangeDirectory(const JString& dirName);
JError	JRemoveDirectory(const JString& dirName);
bool	JKillDirectory(const JString& dirName, const bool sync = true,
					   JProcess** p = nullptr);

JString	JGetCurrentDirectory();
bool	JGetHomeDirectory(JString* homeDir);
bool	JGetHomeDirectory(const JString& user, JString* homeDir);
bool	JGetPrefsDirectory(JString* prefsDir);
bool	JGetPrefsDirectory(const JString& user, JString* prefsDir);
bool	JGetTempDirectory(JString* tempDir);
bool	JGetTrueName(const JString& name, JString* trueName);
bool	JSearchSubdirs(const JString& startPath, const JString& name,
					   const bool isFile, const JString::Case caseSensitive,
					   JString* path, JString* newName = nullptr,
					   JProgressDisplay* pg = nullptr,
					   bool* userCancelled = nullptr);

JString		JGetUniqueDirEntryName(const JString& path, const JString& namePrefix,
								   const JUtf8Byte* nameSuffix = nullptr,
								   const JIndex startIndex = 1);
JError		JCreateTempDirectory(const JString* path, const JString* prefix,
								 JString* fullName);

JString	JCombinePathAndName(const JString& path, const JString& name);
bool	JSplitPathAndName(const JString& fullName, JString* path, JString* name);
void	JAppendDirSeparator(JString* dirName);
void	JStripTrailingDirSeparator(JString* dirName);
void	JCleanPath(JString* path);
JString	JGetRootDirectory();
bool	JIsRootDirectory(const JString& dirName);
bool	JIsAbsolutePath(const JString& path);
bool	JIsRelativePath(const JString& path);

bool	JConvertToAbsolutePath(const JString& path, const JString& base, JString* result);
JString	JConvertToRelativePath(const JString& path, const JString& base);

bool	JExpandHomeDirShortcut(const JString& path, JString* result,
							   JString* homeDir = nullptr, JSize* homeLength = nullptr);
JString	JConvertToHomeDirShortcut(const JString& path);
JString	JGetClosestDirectory(const JString& origDirName,
							 const bool requireWrite = false,
							 const JString* basePath = nullptr);

#ifdef WIN32
bool	JGetDirectoryFromCSIDL(const int folderCSIDL, JString* path);
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
