/******************************************************************************
 jMountUtil.h

	Copyright (C) 2000-2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_jMountUtil
#define _H_jMountUtil

#include "jx-af/jcore/JPtrArray-JString.h"
#include "jx-af/jcore/JError.h"

class JProcess;

// Do not change the values once they are allocated, because they may be
// stored in files.

enum JMountType
{
	kJHardDisk = 0,
	kJFloppyDisk,
	kJCDROM,
	kJObsolete1,
	kJUnknownMountType
};

enum JFileSystemType
{
	kOtherFSType = 0,
	kVFATType
};

struct JMountPoint
{
	JString*		path;
	JMountType		type;
	dev_t			device;
	JString*		devicePath;
	JFileSystemType	fsType;

	JMountPoint()
		:
		path(nullptr), type(kJHardDisk), device(0), devicePath(nullptr), fsType(kOtherFSType)
	{ };

	JMountPoint(JString* p, const JMountType t, const dev_t d, JString* dp, const JFileSystemType fst)
		:
		path(p), type(t), device(d), devicePath(dp), fsType(fst)
	{ };
};

union JMountState
{
	time_t		modTime;
	JString*	mountCmdOutput;

	JMountState()
		:
		mountCmdOutput(nullptr)	// can only init one member, and ptr is critical
	{ };

	~JMountState();
};

class JMountPointList : public JArray<JMountPoint>
{
public:

	JMountPointList(const JPtrArrayT::CleanUpAction action);

	~JMountPointList() override;

	void	CleanOut();		// safest
	void	DeleteAll();

	JPtrArrayT::CleanUpAction	GetCleanUpAction() const;
	void						SetCleanUpAction(const JPtrArrayT::CleanUpAction action);

private:

	JPtrArrayT::CleanUpAction	itsCleanUpAction;
};

bool		JGetUserMountPointList(JMountPointList* list, JMountState* state);
JMountType	JGetUserMountPointType(const JString& path,
								   const JString& device, const JString& fsType);
bool	JIsMounted(const JString& path,
				   bool* writable = nullptr, bool* isTop = nullptr,
				   JString* device = nullptr,
				   JFileSystemType* fsType = nullptr, JString* fsTypeString = nullptr);
bool	JFindUserMountPoint(const JString& path, const JMountPointList& list,
							JIndex* index);

void	JMount(const JString& path, const bool mount = true,
			   const bool block = false);

bool	JTranslateLocalToRemote(const JString& localPathStr,
								JString* host, JString* remotePath);
bool	JTranslateRemoteToLocal(const JString& hostStr,
								const JString& remotePathStr,
								JString* localPath);

JError	JFormatPartition(const JString& path, const JString& type,
						 JProcess** process);
bool	JIsSamePartition(const JString& path1, const JString& path2);

inline bool
JMountSupportsExecFlag
	(
	const JFileSystemType type
	)
{
	return type != kVFATType;
}

#endif
