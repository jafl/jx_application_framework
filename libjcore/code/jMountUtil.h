/******************************************************************************
 jMountUtil.h

	Copyright © 2000-2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_jMountUtil
#define _H_jMountUtil

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JPtrArray-JString.h>
#include <JError.h>

class JProcess;

// Do not change the values once they are allocated, because they may be
// stored in files.

enum JMountType
{
	kJHardDisk = 0,
	kJFloppyDisk,
	kJCDROM,
	kJZipDisk,
	kJUnknownMountType
};

struct JMountPoint
{
	JString*	path;
	JMountType	type;
	dev_t		device;
	JString*	devicePath;

	JMountPoint()
		:
		path(NULL), type(kJHardDisk), device(0), devicePath(NULL)
	{ };

	JMountPoint(JString* p, const JMountType t, const dev_t d, JString* dp)
		:
		path(p), type(t), device(d), devicePath(dp)
	{ };
};

class JMountPointList : public JArray<JMountPoint>
{
public:

	JMountPointList(const JPtrArrayT::CleanUpAction action);

	virtual ~JMountPointList();

	void	CleanOut();		// safest
	void	DeleteAll();

	JPtrArrayT::CleanUpAction	GetCleanUpAction() const;
	void						SetCleanUpAction(const JPtrArrayT::CleanUpAction action);

private:

	JPtrArrayT::CleanUpAction	itsCleanUpAction;
};

JBoolean	JGetUserMountPointList(JMountPointList* list, time_t* modTime);
JMountType	JGetUserMountPointType(const JCharacter* path,
								   const JCharacter* device, const JCharacter* fsType);
JBoolean	JIsMounted(const JCharacter* path,
					   JBoolean* writable = NULL, JBoolean* isTop = NULL,
					   JString* device = NULL, JString* fsType = NULL);
JBoolean	JFindUserMountPoint(const JCharacter* path, const JMountPointList& list,
								JIndex* index);

void		JMount(const JCharacter* path, const JBoolean mount = kJTrue,
				   const JBoolean block = kJFalse);

JBoolean	JTranslateLocalToRemote(const JCharacter* localPathStr,
									JString* host, JString* remotePath);
JBoolean	JTranslateRemoteToLocal(const JCharacter* hostStr,
									const JCharacter* remotePathStr,
									JString* localPath);

JError		JFormatPartition(const JCharacter* path, const JCharacter* type,
							 JProcess** process);
JBoolean	JIsSamePartition(const JCharacter* path1, const JCharacter* path2);

#endif
